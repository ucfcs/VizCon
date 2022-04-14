import { BrowserWindow, ipcMain, dialog, app } from 'electron';
import { readFileSync, writeFileSync } from 'fs';
import { exec, spawn } from 'child_process';
import { cwd } from 'process';
import { sep as pathSep } from 'path';
import split2 from 'split2';
import * as pty from 'node-pty';
import { filePathToFileName } from '../util/utils';

// determine where the concurrency folder is
let resourcesDir = '.';
if (app.isPackaged) {
  // replace \\\\ (two escaped \\) with / to simplify things
  resourcesDir = process.resourcesPath.replace(/\\\\/g, pathSep);
}

const concurrencyFolder = resourcesDir + pathSep + 'concurrency' + pathSep;

const library = ['vcuserlibrary.c', 'lldb_lib.c', 'utils.c', 'mutexes.c', 'semaphores.c', 'threads.c'];
const libraryPaths = library.map(file => {
  return '"' + concurrencyFolder + file + '"';
});

// TODO: Save this to file and read it in at application start. This is going to involve using the electron path where it saves all the temp data and what not WOW this line got long
let lastSuccessfulFile: string;

function updateLastSuccessfulFile(files: string[]): void {
  const filePath = files[files.length - 1].replace(/\\/g, '/');
  const dirPath = filePath.substring(0, filePath.lastIndexOf('/'));
  if (!dirPath.endsWith('examples')) {
    lastSuccessfulFile = dirPath;
  }
}

ipcMain.handle('minimize', e => {
  const window = BrowserWindow.fromWebContents(e.sender);
  window.minimize();
});

ipcMain.handle('maximize', e => {
  const window = BrowserWindow.fromWebContents(e.sender);
  window.maximize();
});

ipcMain.handle('restore', e => {
  const window = BrowserWindow.fromWebContents(e.sender);
  window.restore();
});

ipcMain.handle('close', e => {
  const window = BrowserWindow.fromWebContents(e.sender);
  window.close();
});

ipcMain.handle('isMaximized', e => {
  const window = BrowserWindow.fromWebContents(e.sender);
  return window.isMaximized();
});

ipcMain.handle('openFileDialog', e => {
  const window = BrowserWindow.fromWebContents(e.sender);
  const results = dialog.showOpenDialogSync(window, {
    filters: [{ name: 'C Language File', extensions: ['c'] }],
    properties: ['openFile', 'multiSelections'],
    defaultPath: lastSuccessfulFile,
  });
  if (!results) {
    return ['EMPTY: No file selected'];
  }
  updateLastSuccessfulFile(results);
  return results;
});

ipcMain.handle('openExampleFileDialog', e => {
  const window = BrowserWindow.fromWebContents(e.sender);
  const results = dialog.showOpenDialogSync(window, {
    filters: [{ name: 'C Language File', extensions: ['c'] }],
    properties: ['openFile', 'multiSelections'],
    defaultPath: resourcesDir + pathSep + 'examples',
  });
  if (!results) {
    return ['EMPTY: No file selected'];
  }
  updateLastSuccessfulFile(results);
  return results;
});

ipcMain.handle('readFilesSync', (e, files: string[]) => {
  return files.map(file => {
    try {
      const contents = readFileSync(file, 'utf-8');
      return contents;
    } catch (error) {
      return 'ERROR:' + error;
    }
  });
});

ipcMain.handle('saveFileToDisk', (e, path: string, content: string, forceDialog?: boolean) => {
  if (forceDialog || path.includes('tracking://')) {
    const window = BrowserWindow.fromWebContents(e.sender);
    const newPath = dialog.showSaveDialogSync(window, {
      filters: [{ name: 'C Language File', extensions: ['c'] }],
      properties: ['createDirectory', 'showOverwriteConfirmation'],
    });

    // if cancelled, dont do anything
    if (!newPath) {
      return path;
    }

    path = newPath;
  }

  writeFileSync(path, content, 'utf-8');

  return path;
});

ipcMain.handle('compileFile', async (e, path: string) => {
  const files = [`"${path}"`, ...libraryPaths];
  const outputFile = app.getPath('temp') + pathSep + filePathToFileName(path) + (process.platform === 'win32' ? '.exe' : '');

  const commandString = `gcc -gdwarf-4 ${files.join(' ')} -I ${concurrencyFolder} -o "${outputFile}" -Wall`;
  console.log('CompileString:', commandString);

  const prom = new Promise(resolve => {
    exec(commandString, (err, stdout, stderr) => {
      if (err && err.code !== 0) {
        resolve({ out: stdout, err: stderr });
        return;
      }
      resolve({ out: stdout, err: stderr });
    });
  });

  return await prom;
});

function launchProgram(path: string, port: Electron.MessagePortMain): void {
  const extension = process.platform === 'win32' ? '.exe' : '';
  const exeFile = app.getPath('temp') + pathSep + filePathToFileName(path) + extension;
  const controllerDir = resourcesDir + pathSep + 'concurrency' + pathSep + 'controller';
  console.log(`Current directory: ${cwd()}`);
  const lldb = resourcesDir + pathSep + 'platform' + pathSep + 'lldb' + pathSep + 'bin' + pathSep + 'lldb' + extension;
  let term: any = null;
  if (process.platform !== 'win32') {
    term = (pty as any).open({ name: 'dumb' });
    term.on('data', (data: any) => {
      console.log(`pty data ${data}`);
      port.postMessage({ type: 'stdout', data: data + '' });
    });
  }
  const terminalOutputFile = term == null ? 'None' : `'${btoa(term._pty)}'`;
  const env = Object.assign({}, process.env);
  delete env.PYTHONPATH;
  delete env.PYTHONHOME;
  const child = spawn(lldb, {
    stdio: ['pipe', 'pipe', 'pipe', 'pipe'],
    env
  });
  child.stdin.write(
    `script import sys; import base64; sys.path.append(base64.b64decode('${btoa(
      controllerDir
    )}').decode()); import script; script.start('${btoa(exeFile)}', ${terminalOutputFile}, True)\n`
  );
  child.on('close', code => {
    if (term) term.destroy();
    console.log(`child process exited with code ${code}`);
  });

  port.on('message', evt => {
    if (evt.data.type === 'stop') {
      console.log('Stopping child');
      const res = child.kill();
      port.postMessage({ type: 'kill_result', result: res });
      return;
    }
    if (evt.data.type !== 'request') {
      throw new Error('Invalid message type');
    }
    //console.log('Writing');
    child.stdin.write(JSON.stringify({ type: 'request' }) + '\n');
  });

  child.stdio[3].pipe(split2()).on('data', (data: string) => {
    //console.log(`child process data: "${data}"`);
    const msg = JSON.parse(data);
    if (msg.type === 'process_end') {
      console.log('Terminating the lldb process.\n');
      child.kill();
    }
    port.postMessage(msg);
  });

  let haveSeenLldbMessage = false;
  child.stdout.on('data', (data: string) => {
    console.log(`child process stdout: "${data}"`);
    const str = data + '';
    if (!haveSeenLldbMessage && str.startsWith('(lldb) script import sys;')) {
      haveSeenLldbMessage = true;
      return;
    }
    port.postMessage({ type: 'stdout', data: str });
  });
  child.stderr.on('data', (data: string) => {
    console.log(`child process stderr: "${data}"`);
    const str = data + '';
    if (!haveSeenLldbMessage && str.startsWith('(lldb) script import sys;')) {
      haveSeenLldbMessage = true;
      return;
    }
    port.postMessage({ type: 'stdout', data: str });
  });
}

ipcMain.on('launchProgram', (event, msg) => {
  const port = event.ports[0];
  port.start();
  launchProgram(msg.path, port);
});

ipcMain.handle('showUnsavedSaveDialog', async (e, name: string): Promise<UnsavedChangesResponse> => {
  const result = await dialog.showMessageBox(BrowserWindow.fromWebContents(e.sender), {
    message: 'Do you want to save the changes you made to ' + name + '?',
    detail: 'Your changes will be lost if you dont save them.',
    title: 'VizCon',
    buttons: ['Save', "Don't Save", 'Cancel'],
    type: 'warning',
    noLink: true,
  });

  switch (result.response) {
    case 0:
      return 'save';
    case 1:
      return 'dontsave';
    default:
      return 'cancel';
  }
});

ipcMain.handle('showUnsavedCompileDialog', async (e, name: string): Promise<UnsavedChangesResponse> => {
  const result = await dialog.showMessageBox(BrowserWindow.fromWebContents(e.sender), {
    message: 'Do you want to save the changes you made to ' + name + ' before you compile?',
    detail: "Your changes will be not be compiled if you don't save them.",
    title: 'VizCon',
    buttons: ['Save', "Don't Save", 'Cancel'],
    type: 'warning',
    noLink: true,
  });

  switch (result.response) {
    case 0:
      return 'save';
    case 1:
      return 'dontsave';
    default:
      return 'cancel';
  }
});
