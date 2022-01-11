import { BrowserWindow, ipcMain } from "electron";

ipcMain.handle('minimize', (e) => {
  const window = BrowserWindow.fromWebContents(e.sender);
  window.minimize();
});

ipcMain.handle('maximize', (e) => {
  const window = BrowserWindow.fromWebContents(e.sender);
  window.maximize();
});

ipcMain.handle('close', (e) => {
  const window = BrowserWindow.fromWebContents(e.sender);
  window.close();
});
