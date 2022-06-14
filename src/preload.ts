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
    return await ipcRenderer.invoke('openFileDialog');
  },
  openExampleFileDialog: async (): Promise<string[]> => {
    return await ipcRenderer.invoke('openExampleFileDialog');
  },
  openUserGuide: () => {
    ipcRenderer.invoke('openUserGuide');
  },
  saveFileToDisk: async (path: string, content: string, forceDialog?: boolean): Promise<string> => {
    return await ipcRenderer.invoke('saveFileToDisk', path, content, forceDialog);
  },
  compileFile: async (path: string): Promise<{ out: string; err: string }> => {
    return await ipcRenderer.invoke('compileFile', path);
  },
  launchProgram: (path: string, stdoutHandler: (data: string) => void): Promise<DebuggerHandle> => {
    return new Promise(resolve => {
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
              return new Promise(resolveStep => {
                if (waitingHandler !== null) {
                  throw new Error('Attempted to step again before previous step completed');
                }

                channel.port1.postMessage({ type: 'request' });
                waitingHandler = msg => {
                  resolveStep(msg);
                  waitingHandler = null;
                };
              });
            },
            stop: () => {
              return new Promise(resolveStop => {
                channel.port1.postMessage({ type: 'stop' });
                const oldHandler = waitingHandler;

                const killHandler = (msg: any) => {
                  if (msg.type === 'kill_result') {
                    resolveStop(msg);
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
    ipcRenderer.invoke('disableMenu', menuParent, menuItem, disabled);
  },
  showUnsavedSaveDialog: async (name: string): Promise<UnsavedChangesResponse> => {
    return await ipcRenderer.invoke('showUnsavedSaveDialog', name);
  },
  showUnsavedCompileDialog: async (name: string): Promise<UnsavedChangesResponse> => {
    return await ipcRenderer.invoke('showUnsavedCompileDialog', name);
  },
});
