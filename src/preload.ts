import { contextBridge, ipcRenderer } from 'electron';
import { platform } from 'process';

console.log('Hello from preload.ts');

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
  close: (): void => {
    ipcRenderer.invoke('close');
  },
});
