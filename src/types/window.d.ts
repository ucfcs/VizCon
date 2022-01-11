export {};

declare global {
  interface Window {
    platform: {
      getPlatform: () => string,
      minimize: () => void,
      maximize: () => void,
      close: () => void
    }
  }
}
