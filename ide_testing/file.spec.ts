import { ElectronApplication, Locator, _electron as electron } from "playwright";
import { test, expect, Page } from "@playwright/test";
import { readFileSync, writeFileSync } from "fs";
import { join } from "path";
const testStringSize: number = 5;
let electronApp: ElectronApplication;
let window: Page;

// makeString - Generates a random string of the given length for use by the tests
function makeString(length: number): string
{
  let result: string = '';
  const characters: string = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  const charactersLength: number = characters.length;
  for (let i: number = 0; i < length; i++)
    result += characters.charAt(Math.floor(Math.random() * charactersLength));
  return result;
}

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

    // If the visualizer is open, select "Return to Editor" to open the editor.
    if(await window.isVisible('[title="Return to Editor"]'))
      await window.locator('[title="Return to Editor"]').click();
  });

  // New File - Check that a new file is opened.
  test('New File', async () =>
  {
    // Select "New File".
    await window.locator('div.menu-item:has-text("File")').click();
    await window.locator('span.action-label:has-text("New File")').click();

    // Check that a blank file was opened.
    // Use '#ide' to specify root since the locator also checks #visualizer.
    const tab_title: string = await window.locator('#ide div.tab-label').textContent();
    expect(tab_title).toBe("Untitled-1");
    const file_contents: string = await window.locator('#ide div.view-line').textContent();
    expect(file_contents).toBe("");
  });

  // Open File - Check that an existing file is opened.
  test('Open File', async () =>
  {
    // The tester will have to select "dummy.c" here.
    // Playwright cannot access the file chooser since it is spawned by
    // the Electron process, not the page itself.
    // There are ways to circumvent the file picker and just send the desired file path
    // (see https://kubeshop.io/blog/testing-electron-apps-with-playwright#automate-on-github
    // and https://github.com/wsw0108/playwright-fake-dialog)
    // but I had trouble getting them to work.
    console.log("Please select \"dummy.c\".")

    // Select "Open File".
    await window.locator('div.menu-item:has-text("File")').click();
    await window.locator('span.action-label:has-text("Open File")').click();

    // Ensure that a file is loaded (i.e. that it didn't just open a blank file)
    // Use '#ide' to specify root since the locator also checks #visualizer.
    // The awaits on the expects are needed since toHaveText pulls the contents at runtime.
    const tab_title: Locator = window.locator('#ide div.tab-label');
    await expect(tab_title).not.toHaveText("Untitled-1");
    const file_contents: Locator = window.locator('#ide div.view-line');
    expect(await file_contents.textContent()).not.toBe("");

    // Ensure the tab name and contents are correct.
    await expect(tab_title).toHaveText("dummy.c");
    await expect(file_contents).toHaveText("// This is a dummy file for the purposes of checking the IDE.");
  });

  // Save Existing File - Check that an existing file is saved.
  test('Save Existing File', async () =>
  {
    // Open a file.
    console.log("Please select \"editable.c\".")
    await window.locator('div.menu-item:has-text("File")').click();
    await window.locator('span.action-label:has-text("Open File")').click();

    // File is loaded by tester here...

    // Save the file contents for later use.
    // Remove any spaces to avoid issues with encoding.
    const original_contents: string = (await window.locator('#ide div.view-line').textContent()).replace(/\s/g, "");

    // Append random string.
    const rand: string = makeString(testStringSize);
    await window.locator('#ide div.view-line').click();
    await window.press('#ide div.view-line', 'Control+A');
    await window.press('#ide div.view-line', 'ArrowRight');
    await window.type('#ide div.view-line', rand);
    
    // Select "Save File".
    await window.locator('div.menu-item:has-text("File")').click();
    await window.locator('span.action-label:has-text("Save File")').click();

    // Load the file externally and check that the appended string appears.
    // Remove any spaces to avoid issues with encoding.
    const file_contents: string = readFileSync(join(__dirname, 'editable.c')).toString().replace(/\s/g, "");
    expect(file_contents).toBe(original_contents + rand);
  });

  // Close Unedited File - Close a file.
  test('Close Unedited File', async () =>
  {
    // Open a file.
    console.log("Please select \"dummy.c\".")
    await window.locator('div.menu-item:has-text("File")').click();
    await window.locator('span.action-label:has-text("Open File")').click();

    // File is loaded by tester here...

    // Close the file. Check that the tab disappears immediately.
    await window.locator('#ide a.action-label.codicon.codicon-close').click();
    expect(await window.isVisible('#ide div.tab-label')).toBeFalsy();
    expect(await window.isVisible('#ide div.view-line')).toBeFalsy();
  });

  // After Each - Close all open files.
  test.afterEach(async () =>
  {
    //await window.pause();
    while(true)
    {
      if(await window.isVisible('#ide a.action-label.codicon.codicon-close'))
        await window.locator('#ide a.action-label.codicon.codicon-close').first().click();
      else
        break;
    }
  });
  
  // After All - Reset test files and exit app.
  test.afterAll(async () =>
  {
    // Reset edited test files.
    writeFileSync(join(__dirname, 'editable.c'), "// Edit test");

    // Exit app.
    window = null;
    await electronApp.close();
  });
});