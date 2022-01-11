import { contextBridge, ipcRenderer } from 'electron';
import { platform } from 'process';

console.log('Hello from preload.ts');

contextBridge.exposeInMainWorld('platform', {
  getPlatform: (): string => { return platform },
});
