import { ElectronApplication, Locator, _electron as electron } from "playwright";
import { test, expect, Page } from "@playwright/test";
let electronApp: ElectronApplication;
let window: Page;

// Visualizer - The list of tests begins below.
test.describe("Visualizer", async () =>
{
  // Before All - Launch the app, get the first window, and set execution speed to max.
  test.beforeAll(async () =>
  {
    // Launch Electron app.
    electronApp = await electron.launch({ args: ['.'] , executablePath: "./out/vizcon-win32-x64/vizcon.exe" });

    // Get the first window that the app opens.
    window = await electronApp.firstWindow();

    // Switch to the visualizer and set the speed to its fastest setting. Then go back.
    await window.locator('div.menu-item:has-text("ViewShow")').click();
    await window.locator('span.action-label:text("Show Visualizer")').click();
    await window.locator('#visualizer input[type="range"]:visible').fill('0');
    await window.locator('div.menu-item:has-text("ViewShow")').click();
    await window.locator('span.action-label:text("Show Editor")').click();
  });

  // Thread List - All threads are listed with the correct names.
  test('Thread List', async () =>
  {
    // Open a testing file that generates a set of threads with random names.
    console.log('Please select "threadlist.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...
    
    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success, and then run the program.
    let runStatus: Locator = window.locator('#visualizer div.control:has-text("Status:"):visible');
    let consoleOut: Locator = window.locator("#visualizer div.view-lines.monaco-mouse-cursor-text");
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();
    while ((await runStatus.textContent()) != 'Status: Finished');

    // Once the program finishes, compare the console output to the names in the thread list.
    let outputNames = (await consoleOut.textContent()).split('|');
    let tableNames = await window.locator("#visualizer td.thread-name").allTextContents();
    expect(tableNames.length).toBe(outputNames.length + 1);
    expect(tableNames[0]).toBe("#main_thread");
    for(let i = 0; i < outputNames.length; i++)
    {
      // Add 1 to the tableNames index since the first table entry is #main_thread.
      expect(outputNames[i]).toBe(tableNames[i + 1]);
    }
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