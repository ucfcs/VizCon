import { _electron as electron } from "playwright";
import { test, expect, Page } from "@playwright/test";
var electronApp;
let window: Page;

// File Menu - The list of tests begins below.
test.describe("File Menu", async () =>
{
  // Before All - Launch the app and get the first window.
  test.beforeAll(async () =>
  {
    // Launch Electron app.
    electronApp = await electron.launch({ args: ['.'] , executablePath: "./out/vizcon-win32-x64/vizcon.exe" });

    // Get the first window that the app opens.
    window = await electronApp.firstWindow();

    // Select "Return to Editor" to open the editor.
    await window.locator('[title="Return to Editor"]').click();
  });

  // New File - Check that a new file is opened.
  test('New File', async () =>
  {
    // Select "New File"
    await window.locator('div.menu-item:has-text("File")').click();
    await window.locator('span.action-label:has-text("New File")').click();

    // Check that a blank file was opened.
    // Use '#ide' to specify root since the locator also checks #visualizer.
    const tab_title = await window.locator('#ide div.tab-label').textContent();
    expect(tab_title).toBe("Untitled-1");
    const file_contents = await window.locator('#ide div.view-line').textContent();
    expect(file_contents).toBe("");

    // Write a word to the blank file space and check that.
    // (This is more a demonstration for future testing.)
    await window.locator('#ide div.view-lines.monaco-mouse-cursor-text').click();
    await window.keyboard.insertText("Hello!");
    const file_contents2 = await window.locator('#ide div.view-lines.monaco-mouse-cursor-text').textContent();
    expect(file_contents2).toBe("Hello!");

    // Write a second word to a second line and check that.
    // (This is more a demonstration for future testing.)
    await window.keyboard.press('Enter');
    await window.keyboard.insertText("Goodbye!");
    const file_contents3 = await window.locator('#ide div.view-lines.monaco-mouse-cursor-text').textContent();
    expect(file_contents3).toBe("Hello!Goodbye!");
  });

  // After All - Exit app.
  test.afterAll(async () =>
  {
    window = null;
    await electronApp.close();
  });
});