export {};

declare global {
  interface Window {
    platform: {
      getPlatform: () => string
    }
  }
}
