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
    const runStatus: Locator = window.locator('#visualizer div.control:has-text("Status:"):visible');
    const consoleOut: Locator = window.locator("#visualizer div.view-lines.monaco-mouse-cursor-text");
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();
    while ((await runStatus.textContent()) != 'Status: Finished');

    // Once the program finishes, compare the console output to the names in the thread list.
    const outputNames = (await consoleOut.textContent()).split('|');
    const tableNames = await window.locator("#visualizer td.thread-name").allTextContents();
    expect(tableNames.length).toBe(outputNames.length + 1);
    expect(tableNames[0]).toBe("#main_thread");
    for(let i = 0; i < outputNames.length; i++)
    {
      // Add 1 to the tableNames index since the first table entry is #main_thread.
      expect(outputNames[i]).toBe(tableNames[i + 1]);
    }
  });

  // Thread Status - All thread statuses are correct and up-to-date.
  test('Thread Status', async () =>
  {
    // Open a testing file that generates a set of threads with random names.
    console.log('Please select "threadstatus.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...
    
    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success, and then run the program.
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();
    const consoleOut: Locator = window.locator("#visualizer div.view-lines.monaco-mouse-cursor-text");

    // Wait for the first ID to be printed, then get the number of threads.
    // Use that to create an array of statuses for each thread: true means the thread has finished.
    while ((await consoleOut.textContent({ timeout: 15000 })) == '');
    const numThreads = (await window.locator("#visualizer td.thread-name").count()) - 1;
    let threads: Array<Boolean> = new Array(numThreads);
    for(let i = 0; i < numThreads; i++) threads[i] = false;
    
    // When an ID number is printed, check that the corresponding thread is highlighted and that 'ACTIVE' is true.
    // Also check that the others' ACTIVE and FINISHED values are correct.
    let lastOut = '';
    for(let i = 0; i < numThreads; i++)
    {
      // Wait for something to be printed.
      while ((await consoleOut.textContent()) == lastOut);
      lastOut = await consoleOut.textContent();

      // Get the most recent ID number.
      let lastThreadID = parseInt(lastOut.charAt(lastOut.length - 1));

      // Check every thread's ACTIVE and FINISH values.
      for(let j = 0; j < numThreads; j++)
      {
        // If this is the active thread, make sure that this is the highlighted row.
        // Also make sure that ACTIVE is true and FINISH is false.
        // Then, mark it as having been active.
        if((j + 1) == lastThreadID)
        {
          let tableRow: Locator = window.locator('#visualizer tr.thread-row.current');
          await expect(tableRow).toContainText('Thread ' + (j + 1));
          await expect(tableRow).toContainText('truefalse');
          threads[j] = true;
        }

        // If threads[j] is true, the corresponding thread has finished.
        // Also make sure that ACTIVE is false and FINISH is true.
        else if(threads[j] == true)
        {
          let tableRow: Locator = window.locator('#visualizer tr.thread-row:has-text("Thread ' + (j + 1) + '")');
          await expect(tableRow).toContainText('falsetrue');
        }

        // If threads[j] is false, the corresponding thread has not finished.
        // Also make sure that ACTIVE is false and FINISH is false.
        else
        {
          let tableRow: Locator = window.locator('#visualizer tr.thread-row:has-text("Thread ' + (j + 1) + '")');
          await expect(tableRow).toContainText('falsefalse');
        }
      }
    }
  });

  // Variable List - All in-scope variables are listed with the correct type.
  test('Variable List', async () =>
  {
    // Open a testing file that generates a set of threads with random names.
    console.log('Please select "varlist.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...
    
    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success, and then run the program.
    const runStatus: Locator = window.locator('#visualizer div.control:has-text("Status:"):visible');
    const consoleOut: Locator = window.locator("#visualizer div.view-lines.monaco-mouse-cursor-text");
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();

    // Wait for the "loop start" statement to be printed, then pause.
    while ((await consoleOut.textContent({ timeout: 15000 })) == '');
    await window.locator('#visualizer div.control.has-action:has-text("Pause Simulation")').click();

    // Check the entire variable list.
    // For each entry, make sure it's present and that the type is correct.
    // Check with expectedVars, which is an array of [name, type] tuples.
    const expectedVars: [string, string][] = [['numVar', 'int'], ['numPtr', 'int *'],
    ['tinyVar', 'short'], ['tinyPtr', 'short *'], ['bigVar', 'long'], ['decimalVar', 'float'],
    ['bigDecVar', 'double'], ['letterVar', 'char'], ['testPtr', 'TestStruct *'], ['testVar', 'TestStruct'],
    ['theArray', 'int[5]'], ['iterator', 'int'], ['loopNum', 'short']];
    expectedVars.forEach(async ([name, type]) => {
      const tableRow: Locator = window.locator('#visualizer tr.variable-row:has-text("' + name + '")');
      expect(await tableRow.isVisible()).toBeTruthy();
      expect(tableRow).toContainText(type);
    });

    // Resume the program. Once it finishes, check that iterator and loopNum disappeared because their scope ended.
    await window.locator('#visualizer div.control.has-action:has-text("Resume Simulation")').click();
    while((await runStatus.textContent()) != 'Status: Finished');
    expect(await window.isVisible('#visualizer tr.variable-row:has-text("iterator")')).toBeFalsy();
    expect(await window.isVisible('#visualizer tr.variable-row:has-text("loopNum")')).toBeFalsy();
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