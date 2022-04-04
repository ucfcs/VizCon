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

  // Compile Failure - Check that file compilation correctly reports errors.
  test('Compile Failure', async () =>
  {
    // Open a testing file.
    console.log('Please select "compile-fail.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...
    
    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the console to appear, which indicates compilation failure,
    // and then wait for the text to appear in the console (since there is a delay).
    // If the console doesn't appear, the consoleLoc.textContent will time out.
    let consoleLoc = window.locator('#ide div.output-container:visible');
    while ((await consoleLoc.textContent({ timeout: 15000 })) == '');

    // Check for the two known errors.
    // (Only two are present because Playwright can only check the text currently visible in the xterm window.)
    // (If more errors are added, there needs to be a way to scroll or something.)
    expect(consoleLoc).toContainText("compile-fail.c:7:25: error: parameter 1 ('param') has incomplete type");
    expect(consoleLoc).toContainText("compile-fail.c:17:5: error: 'countr' undeclared (first use in this function); did you mean 'counter'?");
  });

  // Run - Check that the executable runner works correctly.
  test('Run', async () =>
  {
    // Open a testing file.
    console.log('Please select "run.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...
    
    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success,
    // then create locators and click "Start Simulation".
    let runStatus = window.locator('#visualizer div.control:has-text("Status:")');
    let consoleOut = window.locator("#visualizer div.view-lines.monaco-mouse-cursor-text");
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();

    // Check that the status correctly changes as the program is started.
    // In all cases, wait for a change, then check that the change is correct.
    while ((await runStatus.textContent()) == 'Status: Not started');
    expect(runStatus).toContainText("Status: Starting...");
    while ((await runStatus.textContent()) == 'Status: Starting...');
    expect(runStatus).toContainText("Status: Running");

    // Check that the pause and force-quit buttons appear and that the start button disappears.
    expect(await window.isVisible('#visualizer div.control.has-action:has-text("Start Simulation")')).toBeFalsy();
    expect(await window.isVisible('#visualizer div.control.has-action:has-text("Pause Simulation")')).toBeTruthy();
    expect(await window.isVisible('#visualizer div.control.has-action:has-text("Force Quit Simulation")')).toBeTruthy();

    // Wait for the console to print something, then check that it was "Hello World!".
    while ((await consoleOut.textContent()) == '');
    expect(consoleOut).toContainText("Hello, World!");

    // Check that the status correctly changes when the program ends
    // and that the start/pause/force-quit buttons disappear as needed.
    while ((await runStatus.textContent()) == 'Status: Running');
    expect(runStatus).toContainText("Status: Finished");
    expect(await window.isVisible('#visualizer div.control.has-action:has-text("Start Simulation")')).toBeTruthy();
    expect(await window.isVisible('#visualizer div.control.has-action:has-text("Pause Simulation")')).toBeFalsy();
    expect(await window.isVisible('#visualizer div.control.has-action:has-text("Force Quit Simulation")')).toBeFalsy();
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