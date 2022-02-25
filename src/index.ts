import installExtension, { REACT_DEVELOPER_TOOLS } from 'electron-devtools-installer';
import { app, BrowserWindow, session } from 'electron';
// This allows TypeScript to pick up the magic constant that's auto-generated by Forge's Webpack
// plugin that tells the Electron app where to look for the Webpack-bundled app code (depending on
// whether you're running in development or production).
declare const MAIN_WINDOW_WEBPACK_ENTRY: string;
declare const MAIN_WINDOW_PRELOAD_WEBPACK_ENTRY: string;

// Handle creating/removing shortcuts on Windows when installing/uninstalling.
if (require('electron-squirrel-startup')) {
  // eslint-disable-line global-require
  app.quit();
}

const createWindow = (): void => {
  // only install dev tools in developement mode
  if (!app.isPackaged) {
    installExtension(REACT_DEVELOPER_TOOLS);

    session.defaultSession.webRequest.onHeadersReceived((details, callback) => {
      callback({
        responseHeaders: {
          ...details.responseHeaders,
          'Content-Security-Policy':
            "script-src 'self' 'unsafe-eval' 'unsafe-inline' file: blob: https://cdn.jsdelivr.net/ http://localhost:3000",
        },
      });
    });
  }

  // Create the browser window.
  const mainWindow = new BrowserWindow({
    height: 600,
    minHeight: 300,
    width: 800,
    minWidth: 400,
    webPreferences: {
      preload: MAIN_WINDOW_PRELOAD_WEBPACK_ENTRY,
    },
    frame: process.platform === 'darwin',
    titleBarStyle: process.platform === 'darwin' ? 'hidden' : 'default',
  });

  // and load the index.html of the app.
  mainWindow.loadURL(MAIN_WINDOW_WEBPACK_ENTRY);

  // Open the DevTools when in dev mode
  if (!app.isPackaged) {
    mainWindow.webContents.openDevTools();
  }

  // track the fullsreen state of the window for darwin
  mainWindow.on('enter-full-screen', () => {
    mainWindow.webContents.executeJavaScript(`
    document.getElementById('vizcon').classList.add('fullscreen')
    document.getElementById('vizcon').classList.remove('normal')
    `);
  });

  mainWindow.on('leave-full-screen', () => {
    mainWindow.webContents.executeJavaScript(`
    document.getElementById('vizcon').classList.remove('fullscreen')
    document.getElementById('vizcon').classList.add('normal')
    `);
  });
};

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', createWindow);

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow();
  }
});

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and import them here.

// import the ipc handlers setup in other files
import './main/platform.ts';
