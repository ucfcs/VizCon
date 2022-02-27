import { BrowserWindow, ipcMain, dialog, app } from 'electron';
import { readFileSync, writeFileSync } from 'fs';
import child_process, { exec } from 'child_process';
import {cwd} from 'process';
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

// TODO: add vcuserlibrary.c
const library = ['utils.c', 'mutexes.c', 'semaphores.c', 'threads.c'];
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

  const commandString = `gcc -g ${files.join(' ')} -I ${concurrencyFolder} -o ${outputFile}`;
  console.log(outputFile, commandString);

  const prom = new Promise(resolve => {
    exec(commandString, (err, stdout, stderr) => {
      console.log('err:', err, 'out:', stdout, 'err:', stderr);
      if (err && err.code !== 0) {
        resolve(stderr);
        return;
      }
      resolve(stdout);
    });
  });

  return await prom;
});

// TODO: refactor out this global if it's convenient
let child: child_process.ChildProcess;
let doStepResolveFunc: (msg: any) => void = null;
let handleLldbMsg: (msg: any) => void;
ipcMain.handle('_temp_launchProgram', (e, path: string) => {
  return new Promise((resolve, reject) => {
    // TODO: use executable path
    console.log(`Current directory: ${cwd()}`);
    child = child_process.spawn('python', ['script.py', 'addsem.exe'])
    child.on('close', (code) => {
        console.log(`child process exited with code ${code}`);
    });

    handleLldbMsg = (msg) => {
      console.log(`Received process hello`, msg);
      handleLldbMsg = (msg) => {
        if (doStepResolveFunc === null)
          throw new Error("No response was requested!");
        doStepResolveFunc(msg);
        doStepResolveFunc = null;
      };
      if (msg.type !== 'hello') {
        throw new Error(`Expected type 'hello', was '${msg.type}'`);
      }
      resolve(msg);
    };
    child.stdout.pipe(split2()).on('data', (data: string) => {
      console.log(`child process data: "${data}"`);
      const msg = JSON.parse(data);
      handleLldbMsg(msg);  
    })
    child.stderr.on('data', (data) => {
      console.log(`child process error: "${data}"`);
    })
  });
});

ipcMain.handle('_temp_doStep', (e, msg: any) => {
  return new Promise((resolve, reject) => {
    doStepResolveFunc = resolve;
    child.stdin.write(JSON.stringify({type: 'request'})+"\n");
  });
});
