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
  readFilesSync: async (files: string[]): Promise<string[]> => {
    return await ipcRenderer.invoke('readFilesSync', files);
  },
  openFileDialog: async (): Promise<string[]> => {
    const test = await ipcRenderer.invoke('openFileDialog');
    return test;
  },
  saveFileToDisk: async (path: string, content: string, forceDialog?: boolean): Promise<string> => {
    return await ipcRenderer.invoke('saveFileToDisk', path, content, forceDialog);
  },
  compileFile: async (path: string): Promise<string> => {
    return await ipcRenderer.invoke('compileFile', path);
  },
  _temp_launchProgram: async (path: string, stdoutHandler: (data: string) => void): Promise<any> => {
    const channel = new MessageChannel();
    channel.port1.onmessage = (e) => {
      console.log("Port message", e);
      stdoutHandler(e.data)
    }
    ipcRenderer.postMessage('launchProgram', {path}, [channel.port2])
  },
  _temp_doStep: async (): Promise<any> => {
    // Returns an object representing a message from the debugger
    // TODO: add type
    return await ipcRenderer.invoke('_temp_doStep');
  },
});
