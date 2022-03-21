import { ElectronApplication, Locator, _electron as electron } from "playwright";
import { test, expect, Page } from "@playwright/test";
let electronApp: ElectronApplication;
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
  
  // After All - Exit app.
  test.afterAll(async () =>
  {
    window = null;
    await electronApp.close();
  });
});