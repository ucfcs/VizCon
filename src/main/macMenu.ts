import Electron, { app, shell, Menu, BrowserWindow, Dialog, dialog } from 'electron';

function clickCallback(title: string, optName: string) {
  BrowserWindow.getAllWindows()[0].webContents.executeJavaScript(`
(function() {
  let event = new Event('Nav-${title}-${optName}');
  console.log(event);
  window.dispatchEvent(event);
})();
`);
}

// TODO: disabling of some elements of the menu based on state of application
const template: Electron.MenuItemConstructorOptions[] = [
  // { role: 'appMenu' }
  {
    label: app.name,
    submenu: [
      { role: 'about' },
      { type: 'separator' },
      { role: 'services' },
      { type: 'separator' },
      { role: 'hide' },
      { role: 'hideOthers' },
      { role: 'unhide' },
      { type: 'separator' },
      { role: 'quit' },
    ],
  },
  // { role: 'fileMenu' }
  {
    label: 'File',
    submenu: [
      {
        label: 'New File',
        accelerator: 'CmdOrCtrl+N',
        click: () => {
          clickCallback('File', 'New File');
        },
      },
      { type: 'separator' },
      {
        label: 'Open File',
        accelerator: 'CmdOrCtrl+O',
        click: () => {
          clickCallback('File', 'Open File');
        },
      },
      { type: 'separator' },
      {
        label: 'Save File',
        accelerator: 'CmdOrCtrl+S',
        click: () => {
          clickCallback('File', 'Save File');
        },
      },
      {
        label: 'Save As',
        accelerator: 'CmdOrCtrl+Shift+S',
        click: () => {
          clickCallback('File', 'Save As');
        },
      },
      {
        label: 'Save All',
        accelerator: 'CmdOrCtrl+Option+S',
        click: () => {
          clickCallback('File', 'Save All');
        },
      },
      { type: 'separator' },
      {
        label: 'Close File',
        accelerator: 'CmdOrCtrl+W',
        click: () => {
          clickCallback('File', 'Close File');
        },
      },
      { role: 'close', label: 'Close Window', accelerator: 'CmdOrCtrl+Option+W' },
    ],
  },
  // { role: 'editMenu' }
  {
    label: 'Edit',
    submenu: [
      { role: 'undo' },
      { role: 'redo' },
      { type: 'separator' },
      { role: 'cut' },
      { role: 'copy' },
      { role: 'paste' },
      { role: 'delete' },
      { role: 'selectAll' },
      { type: 'separator' },
      {
        label: 'Speech',
        submenu: [{ role: 'startSpeaking' }, { role: 'stopSpeaking' }],
      },
    ],
  },
  {
    label: 'Compile',
    submenu: [
      {
        label: 'Compile File',
        accelerator: '',
        click: () => {
          clickCallback('Compile', 'Compile File');
        },
      },
      {
        label: 'Compile And Run File',
        accelerator: '',
        click: () => {
          dialog.showMessageBox(BrowserWindow.getAllWindows()[0], { message: 'This feature is not yet implemented.' });
          clickCallback('Compile', 'Compile And Run File');
        },
      },
    ],
  },
  // { role: 'viewMenu' }
  {
    label: 'View',
    submenu: [
      {
        label: 'Show Compile Output',
        accelerator: '',
        click: () => {
          clickCallback('View', 'Show Compile Output');
        },
      },
      {
        label: 'Show Editor',
        accelerator: '',
        click: () => {
          clickCallback('View', 'Show Editor');
        },
      },
      {
        label: 'Show Visualizer',
        accelerator: '',
        click: () => {
          clickCallback('View', 'Show Visualizer');
        },
      },
      { type: 'separator' },
      { role: 'resetZoom' },
      { role: 'zoomIn' },
      { role: 'zoomOut' },
      { type: 'separator' },
      { role: 'togglefullscreen' },
    ],
  },
  // { role: 'windowMenu' }
  {
    label: 'Window',
    submenu: [{ role: 'minimize' }, { role: 'zoom' }, { type: 'separator' }, { role: 'front' }, { type: 'separator' }, { role: 'window' }],
  },
  {
    role: 'help',
    submenu: [
      { role: 'toggleDevTools' },
      {
        label: 'Learn More',
        click: async () => {
          await shell.openExternal('https://electronjs.org');
        },
      },
    ],
  },
];

if (process.platform === 'darwin') {
  const menu = Menu.buildFromTemplate(template);
  Menu.setApplicationMenu(menu);
}