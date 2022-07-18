import { BrowserWindow, ipcMain, dialog } from 'electron';

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