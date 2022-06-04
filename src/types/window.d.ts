export {};

declare global {
  interface Window {
    platform: {
      getPlatform: () => string;
      minimize: () => void;
      maximize: () => void;
      restore: () => void;
      close: () => void;
      isMaximized: () => Promise<boolean>;
      readFilesSync: (files: string[]) => Promise<string[]>;
      openFileDialog: () => Promise<string[]>;
      openExampleFileDialog: () => Promise<string[]>;
      openUserGuide: () => void;
      saveFileToDisk: (path: string, content: string, forceDialog?: boolean) => Promise<string>;
      compileFile: (path: string) => Promise<{ out: string; err: string }>;
      launchProgram: (path: string, stdoutHandler: (data: string) => void) => Promise<DebuggerHandle | DebuggerError>;
      zoomIn: () => void;
      zoomOut: () => void;
      resetZoom: () => void;
      disableMenu: (menuParent: string, menuItem: string, disabled: boolean) => void;
      showUnsavedSaveDialog: (name: string) => Promise<UnsavedChangesResponse>;
      showUnsavedCompileDialog: (name: string) => Promise<UnsavedChangesResponse>;
    };
  }
}
