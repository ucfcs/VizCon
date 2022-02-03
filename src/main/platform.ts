import { BrowserWindow, ipcMain, dialog } from 'electron';
import { readFileSync, writeFileSync } from 'fs';

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
