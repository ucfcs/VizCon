import { BrowserWindow, ipcMain, dialog } from 'electron';
import { readFileSync } from 'fs';

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

ipcMain.handle('openFileDialog', (e) => {
  const window = BrowserWindow.fromWebContents(e.sender);
  const results = dialog.showOpenDialogSync(window, {
    filters: [{name: 'C Language File', extensions: ['c']}],
    properties: ['openFile']
  });
  if (!results) {
    return 'EMPTY: No file selected';
  }
  // log something so we know if there are more than one got returned. This is unlikely, but just incase
  if (results.length > 1) {
    console.log('Got more than one file, not sure what to do. Blindly returning the first');
  }
  return results[0];
});

ipcMain.handle('readFileSync', (e, file: string) => {
  try {
    const contents = readFileSync(file, 'utf-8');
    return contents;
  } catch (error) {
    return 'ERROR:' + error;
  }
});
