import { contextBridge, ipcRenderer } from 'electron';
import { platform } from 'process';

contextBridge.exposeInMainWorld('platform', {
  getPlatform: (): string => {
    return platform;
  },
  minimize: (): void => {
    ipcRenderer.invoke('minimize');
  },
  maximize: (): void => {
    ipcRenderer.invoke('maximize');
  },
  restore: (): void => {
    ipcRenderer.invoke('restore');
  },
  close: (): void => {
    ipcRenderer.invoke('close');
  },
  isMaximized: async (): Promise<boolean> => {
    return await ipcRenderer.invoke('isMaximized');
  },
  readFileSync: async (file: string): Promise<string> => {
    return await ipcRenderer.invoke('readFileSync', file);
  },
  openFileDialog: async (): Promise<string> => {
    const test = await ipcRenderer.invoke('openFileDialog');
    return test;
  },
});
