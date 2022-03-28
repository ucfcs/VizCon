import { ElectronApplication, Locator, _electron as electron } from "playwright";
import { test, expect, Page } from "@playwright/test";
import { readFileSync, writeFileSync } from "fs";
import { join } from "path";
const testStringSize: number = 5;
let electronApp: ElectronApplication;
let window: Page;

// Infrastructure - The list of tests begins below.
test.describe("Infrastructure", async () =>
{
  // Before All - Launch the app and get the first window.
  test.beforeAll(async () =>
  {
    // Launch Electron app.
    electronApp = await electron.launch({ args: ['.'] , executablePath: "./out/vizcon-win32-x64/vizcon.exe" });

    // Get the first window that the app opens.
    window = await electronApp.firstWindow();
  });

  // Compile Success - Check that file compilation works correctly.
  test('Compile Success', async () =>
  {
    // Open a testing file.
    console.log('Please select "compile.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...
    
    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success,
    // and make sure the file is loaded.
    // If the visualizer doesn't appear, the window.locator will time out.
    let vizHeader = window.locator('#visualizer div.control.pad-r:visible');
    await expect(vizHeader).toContainText('compile.c', { timeout: 15000 });
  });

  // After Each - Return to the editor and close all open files.
  test.afterEach(async () =>
  {
    // Return to the editor if needed.
    if (await window.isVisible('[title="Return to Editor"]'))
      await window.locator('[title="Return to Editor"]').click();

    // Close all open files.
    while(true)
    {
      if(await window.isVisible('#ide a.action-label.codicon.codicon-close'))
        await window.locator('#ide a.action-label.codicon.codicon-close').first().click();
      else
        break;
    }
  });
  
  // After All - Exit app.
  test.afterAll(async () =>
  {
    window = null;
    await electronApp.close();
  });
});