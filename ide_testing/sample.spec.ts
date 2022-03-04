import { _electron as electron } from "playwright";
import { test, expect, Page } from "@playwright/test";
var electronApp;
let window: Page;

// Sample Tests - The list of tests begins below.
test.describe("Sample Tests", async () => {
  // Before All - Launch the app, print the path, and get the first window.
  test.beforeAll(async () => {
    // Launch Electron app.
    electronApp = await electron.launch({ args: ['.'] , executablePath: "./out/vizcon-win32-x64/vizcon.exe" });

    // Evaluation expression in the Electron context.
    const appPath = await electronApp.evaluate(async ({ app }) => {
      // This runs in the main Electron process, parameter here is always
      // the result of the require('electron') in the main app script.
      return app.getAppPath();
    });

    // Get the first window that the app opens.
    window = await electronApp.firstWindow();
  });

  // Known Pass - Passes by checking for a known window title.
  test('Known Pass', async () => {
    const title = await window.title();
    expect(title).toBe("VizCon");
  });

  // Known Fail - Fails by checking for a known incorrect window title.
  test('Known Fail', async () => {
    const title = await window.title();
    expect(title).toBe("Not VizCon");
  });

  // After All - Exit app.
  test.afterAll(async () => {
    window = null;
    await electronApp.close();
  });
});