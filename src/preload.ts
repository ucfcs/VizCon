import { contextBridge, ipcRenderer, webFrame } from 'electron';
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
  launchProgram: (path: string, stdoutHandler: (data: string) => void): Promise<DebuggerHandle> => {
    return new Promise((resolve) => {
      const channel = new MessageChannel();
      let hasStarted = false;
      let waitingHandler: (state: any) => void = null;
      channel.port1.onmessage = e => {
        const msg = e.data;
        if (msg.type === 'hello') {
          if (hasStarted) {
            throw new Error('Already started');
          }
          hasStarted = true;
          resolve({
            doStep: () => {
              return new Promise((resolve2) => {
                if (waitingHandler !== null) {
                  throw new Error('Attempted to step again before previous step completed');
                }
                channel.port1.postMessage({ type: 'request' });
                waitingHandler = msg => {
                  resolve2(msg);
                  waitingHandler = null;
                };
              });
            },
            stop: () => {
              return new Promise((resolve2) => {
                channel.port1.postMessage({ type: 'stop' });
                const oldHandler = waitingHandler;
                const killHandler = (msg: any) => {
                  if (msg.type === 'kill_result') {
                    resolve2(msg);
                    if (oldHandler) {
                      oldHandler({ type: 'process_killed' });
                      waitingHandler = killHandler;
                    }
                  }
                };
                waitingHandler = killHandler;
              });
            },
          });
          return;
        }
        console.log('Message from visualizer', msg);
        if (msg.type === 'stdout') {
          stdoutHandler(msg.data);
          return;
        }
        if (waitingHandler === null) {
          throw new Error('Unprompted response from the debugger');
        }
        waitingHandler(msg);
      };
      ipcRenderer.postMessage('launchProgram', { path }, [channel.port2]);
    });
  },
  zoomIn: (): void => {
    webFrame.setZoomLevel(webFrame.getZoomLevel() + 1);
  },
  zoomOut: (): void => {
    webFrame.setZoomLevel(webFrame.getZoomLevel() - 1);
  },
  resetZoom: (): void => {
    webFrame.setZoomLevel(0);
  },
  disableMenu: (menuParent: string, menuItem: string, disabled: boolean): void => {
    console.log(`Disabling ${menuParent}-${menuItem} to ${disabled}`);
    ipcRenderer.invoke('disableMenu', menuParent, menuItem, disabled);
  },
  showUnsavedChangesDialog: async (name: string): Promise<UnsavedChangesResponse> => {
    return await ipcRenderer.invoke('showUnsavedChangesDialog', name);
  }
});
