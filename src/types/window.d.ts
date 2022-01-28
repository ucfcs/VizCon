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
    };
  }
}
