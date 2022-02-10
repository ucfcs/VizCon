import { BrowserWindow, ipcMain, dialog, app } from 'electron';
import { readFileSync, writeFileSync } from 'fs';
import { exec } from 'child_process';
import { sep as pathSep } from 'path';
import { filePathToFileName } from '../util/utils';

// determine where the concurrency folder is
let resourcesPrefix = '.';
if (app.isPackaged) {
  // replace \\\\ (two escaped \\) with / to simplify things
  resourcesPrefix = process.resourcesPath.replace(/\\\\/g, pathSep);
}

// TODO: add vcuserlibrary.c
const library = ['utils.c', 'mutexes.c', 'semaphores.c', 'threads.c'];
const libraryPaths = library.map(file => {
  return resourcesPrefix + pathSep + 'concurrency' + pathSep + file;
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
});

ipcMain.handle('compileFile', async (e, path: string) => {
  const files = [path, ...libraryPaths];
  const outputFile = app.getPath('temp') + pathSep + filePathToFileName(path) + (process.platform === 'win32' ? '.exe' : '');
  
  const commandString = `gcc -g ${files.join(' ')} -o ${outputFile}`;
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
