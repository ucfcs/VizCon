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
      saveFileToDisk: (path: string, content: string, forceDialog?: boolean) => Promise<string>;
      compileFile: (path: string) => Promise<string>;
      _temp_launchProgram: (path: string, stdoutHandler: (data: string) => void) => Promise<any>;
      _temp_doStep: () => Promise<any>;
    };
  }
}
