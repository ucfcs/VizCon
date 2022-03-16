import { _electron as electron } from "playwright";
import { test, expect, Page } from "@playwright/test";
import { clipboard } from "electron";
const testStringSize = 5;
var electronApp;
let window: Page;

// makeString - Generates a random string of the given length for use by the tests
function makeString(length) {
  var result = '';
  const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  const charactersLength = characters.length;
  for (var i = 0; i < length; i++)
    result += characters.charAt(Math.floor(Math.random() * charactersLength));
  return result;
}

// Clipboard - The list of tests begins below.
test.describe("Clipboard", async () =>
{
  // Before Each - Launch the app and get the first window.
  test.beforeEach(async () =>
  {
    // Launch Electron app.
    electronApp = await electron.launch({ args: ['.'] , executablePath: "./out/vizcon-win32-x64/vizcon.exe" });

    // Get the first window that the app opens.
    window = await electronApp.firstWindow();

    // Select "Return to Editor" to open the editor.
    await window.locator('[title="Return to Editor"]').click();

    // Select "New File" so there is a canvas to work on.
    // Wait for the text window to open and move the cursor there.
    await window.locator('div.menu-item:has-text("File")').click();
    await window.locator('span.action-label:has-text("New File")').click();
    await window.locator('#ide div.view-line').click();
  });

  // Cut - Check that the cut keyboard shortcut both removes text and adds it to the clipboard.
  test('Cut', async () =>
  {
    // Generate a random string, type it, and select it.
    const rand = makeString(testStringSize);
    await window.type('#ide div.view-line', rand);
    await window.keyboard.press('Control+A');

    // Press Ctrl + X.
    await window.keyboard.press('Control+X');

    // Check the clipboard and compare it to the random string.
    const clipboardText = await window.evaluate(() => navigator.clipboard.readText())
    expect(clipboardText).toBe(rand);

    // Check that the string is not in the text area.
    const file_contents = await window.locator('#ide div.view-line').textContent();
    expect(file_contents).toBe("");
  });

  // After Each - Exit app.
  test.afterEach(async () =>
  {
    window = null;
    await electronApp.close();
  });
});