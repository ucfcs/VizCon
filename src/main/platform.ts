import { BrowserWindow, ipcMain, dialog, app } from 'electron';
import { readFileSync, writeFileSync } from 'fs';
import child_process, { exec } from 'child_process';
import { cwd } from 'process';
import { sep as pathSep } from 'path';
import split2 from 'split2';
import { filePathToFileName } from '../util/utils';

// determine where the concurrency folder is
let resourcesPrefix = '.';
if (app.isPackaged) {
  // replace \\\\ (two escaped \\) with / to simplify things
  resourcesPrefix = process.resourcesPath.replace(/\\\\/g, pathSep);
}

const concurrencyFolder = resourcesPrefix + pathSep + 'concurrency' + pathSep;

const library = ['vcuserlibrary.c', 'lldb_lib.c', 'utils.c', 'mutexes.c', 'semaphores.c', 'threads.c'];
const libraryPaths = library.map(file => {
  return concurrencyFolder + file;
});

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
  });
  if (!results) {
    return ['EMPTY: No file selected'];
  }
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
  const files = [path, ...libraryPaths];
  const outputFile = app.getPath('temp') + pathSep + filePathToFileName(path) + (process.platform === 'win32' ? '.exe' : '');

  const commandString = `clang -g ${files.join(' ')} -I ${concurrencyFolder} -o ${outputFile}`;
  console.log('CompileString:', commandString);

  const prom = new Promise(resolve => {
    exec(commandString, (err, stdout, stderr) => {
      if (err && err.code !== 0) {
        resolve(stderr);
        return;
      }
      resolve(stdout);
    });
  });

  return await prom;
});

function launchProgram(path: string, port: Electron.MessagePortMain): void {
  const exeFile = app.getPath('temp') + pathSep + filePathToFileName(path) + (process.platform === 'win32' ? '.exe' : '');

  console.log(`Current directory: ${cwd()}`);
  const child = child_process.spawn('python', [concurrencyFolder + 'controller' + pathSep + 'script.py', exeFile], {
    stdio: ['pipe', 'pipe', 'pipe', 'pipe'],
  });
  child.on('close', code => {
    console.log(`child process exited with code ${code}`);
  });

  port.on('message', evt => {
    if (evt.data.type === 'stop') {
      console.log('Stopping child');
      const res = child.kill();
      port.postMessage({ result: res });
      return;
    }
    if (evt.data.type !== 'request') {
      throw new Error('Invalid message type');
    }
    console.log('Writing');
    child.stdin.write(JSON.stringify({ type: 'request' }) + '\n');
  });

  child.stdio[3].pipe(split2()).on('data', (data: string) => {
    console.log(`child process data: "${data}"`);
    const msg = JSON.parse(data);
    port.postMessage(msg);
  });

  child.stdout.on('data', (data: string) => {
    console.log(`child process stdout: "${data}"`);
    port.postMessage({ type: 'stdout', data: data + '' });
  });

  child.stderr.on('data', data => {
    console.log(`child process error: "${data}"`);
  });
}

ipcMain.on('launchProgram', (event, msg) => {
  const port = event.ports[0];
  port.start();
  launchProgram(msg.path, port);
});
