import { ElectronApplication, Locator, _electron as electron } from 'playwright';
import { test, expect, Page } from '@playwright/test';
let electronApp: ElectronApplication;
let window: Page;

// Visualizer - The list of tests begins below.
test.describe('Visualizer', async () => {
  // Before All - Launch the app, get the first window, and set execution speed to max.
  test.beforeAll(async () => {
    // Launch Electron app.
    electronApp = await electron.launch({ args: ['.'], executablePath: './out/vizcon-win32-x64/vizcon.exe' });

    // Get the first window that the app opens.
    window = await electronApp.firstWindow();
  });

  // Thread List - All threads are listed with the correct names.
  test('Thread List', async () => {
    // Set the timeout to 45 seconds because randomization is slow.
    test.setTimeout(45000);

    // Open a testing file that generates a set of threads with random names.
    console.log('Please select "threadlist.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...

    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success, and then run the program.
    const runStatus = window.locator('#visualizer div.control:has-text("Status:"):visible');
    const consoleOut = window.locator('#visualizer div.view-lines.monaco-mouse-cursor-text');
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();
    while ((await runStatus.textContent()) != 'Status: Finished');

    // Once the program finishes, compare the console output to the names in the thread list.
    const outputNames = (await consoleOut.textContent()).split('|');
    const tableNames = await window.locator('#visualizer td.thread-name').allTextContents();
    expect(tableNames.length).toBe(outputNames.length + 1);
    expect(tableNames[0]).toBe('Main Thread');
    for (let i = 0; i < outputNames.length; i++) {
      // Add 1 to the tableNames index since the first table entry is Main Thread.
      expect(outputNames[i]).toBe(tableNames[i + 1]);
    }
  });

  // Thread Status - All thread statuses are correct and up-to-date.
  test('Thread Status', async () => {
    // Open the test file, which creates a set of threads that take turns being active.
    console.log('Please select "threadstatus.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...

    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success, and then run the program.
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();
    const consoleOut = window.locator('#visualizer div.view-lines.monaco-mouse-cursor-text');

    // Wait for the first ID to be printed, then get the number of threads.
    // Use that to create an array of statuses for each thread: true means the thread has finished.
    while ((await consoleOut.textContent({ timeout: 15000 })) == '');
    const numThreads = (await window.locator('#visualizer td.thread-name').count()) - 1;
    const threads = new Array<boolean>(numThreads);
    for (let i = 0; i < numThreads; i++) {
      threads[i] = false;
    }

    // When an ID number is printed, check that the corresponding thread is highlighted and that 'ACTIVE' is true.
    // Also check that the others' ACTIVE and FINISHED values are correct.
    let lastOut = '';
    for (let i = 0; i < numThreads; i++) {
      // Wait for something to be printed.
      while ((await consoleOut.textContent()) == lastOut);
      lastOut = await consoleOut.textContent();

      // Get the most recent ID number.
      const lastThreadID = parseInt(lastOut.charAt(lastOut.length - 1));

      // Check every thread's ACTIVE and FINISH values.
      for (let j = 0; j < numThreads; j++) {
        // If this is the active thread, make sure that this is the highlighted row.
        // Also make sure that ACTIVE is true and FINISH is false.
        // Then, mark it as having been active.
        if (j + 1 == lastThreadID) {
          const tableRow = window.locator('#visualizer tr.thread-row.current');
          expect(await tableRow.isVisible()).toBeTruthy();
          expect(tableRow.locator('td.thread-name')).toContainText('Thread ' + (j + 1));
          expect(tableRow.locator('td.thread-active')).toContainText('true');
          expect(tableRow.locator('td.thread-complete')).toContainText('false');
          threads[j] = true;
        }

        // If threads[j] is true, the corresponding thread has finished.
        // Also make sure that ACTIVE is false and FINISH is true.
        else if (threads[j] == true) {
          const tableRow = window.locator('#visualizer tr.thread-row:has-text("Thread ' + (j + 1) + '")');
          expect(await tableRow.isVisible()).toBeTruthy();
          expect(tableRow.locator('td.thread-name')).toContainText('Thread ' + (j + 1));
          expect(tableRow.locator('td.thread-active')).toContainText('false');
          expect(tableRow.locator('td.thread-complete')).toContainText('true');
        }

        // If threads[j] is false, the corresponding thread has not finished.
        // Also make sure that ACTIVE is false and FINISH is false.
        else {
          const tableRow = window.locator('#visualizer tr.thread-row:has-text("Thread ' + (j + 1) + '")');
          expect(await tableRow.isVisible()).toBeTruthy();
          expect(tableRow.locator('td.thread-name')).toContainText('Thread ' + (j + 1));
          expect(tableRow.locator('td.thread-active')).toContainText('false');
          expect(tableRow.locator('td.thread-complete')).toContainText('false');
        }
      }
    }
  });

  // Variable List - All in-scope variables are listed with the correct type.
  test('Variable List', async () => {
    // Open the test file, which creates a set of threads that take turns locking a mutex.
    console.log('Please select "varlist.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...

    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success, and then run the program.
    const runStatus = window.locator('#visualizer div.control:has-text("Status:"):visible');
    const consoleOut = window.locator('#visualizer div.view-lines.monaco-mouse-cursor-text');
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();

    // Wait for the "loop start" statement to be printed, then pause.
    while ((await consoleOut.textContent({ timeout: 15000 })) == '');
    await window.locator('#visualizer div.control.has-action:has-text("Pause Simulation")').click();

    // Check the entire variable list.
    // For each entry, make sure it's present and that the type is correct.
    // Check with expectedVars, which is an array of [name, type] tuples.
    const expectedVars: [string, string][] = [
      ['numVar', 'int'],
      ['numPtr', 'int *'],
      ['tinyVar', 'short'],
      ['tinyPtr', 'short *'],
      ['bigVar', 'long'],
      ['decimalVar', 'float'],
      ['bigDecVar', 'double'],
      ['letterVar', 'char'],
      ['testPtr', 'TestStruct *'],
      ['testVar', 'TestStruct'],
      ['theArray', 'int[5]'],
      ['iterator', 'int'],
      ['loopNum', 'short'],
    ];
    expectedVars.forEach(async ([name, type]) => {
      const tableRow = window.locator('#visualizer tr.variable-row:has-text("' + name + '")');
      expect(await tableRow.isVisible()).toBeTruthy();
      expect(tableRow.locator('td.variable-name')).toContainText(name);
      expect(tableRow.locator('td.variable-type')).toContainText(type);
    });

    // Resume the program. Once it finishes, check that iterator and loopNum disappeared because their scope ended.
    await window.locator('#visualizer div.control.has-action:has-text("Resume Simulation")').click();
    while ((await runStatus.textContent()) != 'Status: Finished');
    expect(await window.isVisible('#visualizer tr.variable-row:has-text("iterator")')).toBeFalsy();
    expect(await window.isVisible('#visualizer tr.variable-row:has-text("loopNum")')).toBeFalsy();
  });

  // Variable Thread List - All in-scope variables are listed under the correct thread.
  test('Variable Thread List', async () => {
    // Open the test file, which creates a set of threads with specific contained variables.
    console.log('Please select "varthreadlist.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...

    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success, and then run the program.
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();
    const consoleOut = window.locator('#visualizer div.view-lines.monaco-mouse-cursor-text');

    // Wait for the number of threads to be printed.
    while ((await consoleOut.textContent({ timeout: 15000 })).split('|').length <= 1);
    const numThreads = parseInt((await consoleOut.textContent()).split('|')[0]);

    // Wait for all the characters to be printed to the console, and then force quit.
    while ((await consoleOut.textContent({ timeout: 15000 })).split('|')[1].length != numThreads);
    await window.locator('#visualizer div.control.has-action:has-text("Force Quit Simulation")').click();
    const threadNames = (await consoleOut.textContent()).split('|')[1];

    // Check the globals.
    const accordionGlobals = window.locator('#visualizer div.accordion-parent:has-text("Globals")');
    const expectedGlobals: [string, string][] = [
      ['dummy', 'int'],
      ['global1', 'int'],
      ['global2', 'char'],
    ];
    expectedGlobals.forEach(async ([name, type]) => {
      const tableRow = accordionGlobals.locator('tr.variable-row:has-text("' + name + '")');
      expect(await tableRow.isVisible()).toBeTruthy();
      expect(tableRow.locator('td.variable-name')).toContainText(name);
      expect(tableRow.locator('td.variable-type')).toContainText(type);
    });

    // Check the main thread.
    const accordionMain = window.locator('#visualizer div.accordion-parent:has-text("Main Thread")');
    const expectedMain: [string, string][] = [
      ['str', 'char[' + numThreads + '][4]'],
      ['local', 'int'],
    ];
    expectedMain.forEach(async ([name, type]) => {
      const tableRow = accordionMain.locator('tr.variable-row:has-text("' + name + '")');
      expect(await tableRow.isVisible()).toBeTruthy();
      expect(tableRow.locator('td.variable-name')).toContainText(name);
      expect(tableRow.locator('td.variable-type')).toContainText(type);
    });

    // Check each spawned thread.
    // Also check the value of paramChar to make sure it's the variable part of the thread name.
    const expectedThread: [string, string][] = [
      ['paramPtr', 'void *'],
      ['paramChar', 'char'],
    ];
    for (let i = 0; i < threadNames.length; i++) {
      const threadLetter = threadNames.charAt(i);
      const accordionThread = window.locator('#visualizer div.accordion-parent:has-text("|' + threadLetter + '|")');
      expectedThread.forEach(async ([name, type]) => {
        const tableRow = accordionThread.locator('tr.variable-row:has-text("' + name + '")');
        expect(await tableRow.isVisible()).toBeTruthy();
        expect(tableRow.locator('td.variable-name')).toContainText(name);
        expect(tableRow.locator('td.variable-type')).toContainText(type);
      });
      const tableRow = accordionThread.locator('tr.variable-row:has-text("paramChar")');
      expect(tableRow.locator('td.variable-value')).toContainText("'" + threadLetter + "'");
    }
  });

  // Variable List Values - All in-scope variables list the correct values.
  test('Variable List Values', async () => {
    // Open the test file, which creates a set of variables and randomly changes their values.
    console.log('Please select "varvaluelist.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...

    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success, and then run the program.
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();
    const consoleOut = window.locator('#visualizer div.view-lines.monaco-mouse-cursor-text');
    const runStatus = window.locator('#visualizer div.control:has-text("Status:")');

    // Whenever a change is made, check whether the program finished.
    // If it didn't, check that the currently-listed values are correct.
    let lastOut = '';
    while (true) {
      while ((await consoleOut.textContent()) == lastOut);
      lastOut = await consoleOut.textContent();

      // Get the most recent output.
      const lastLine = lastOut.split('|')[lastOut.split('|').length - 1];

      // If this last line is "Done.", leave.
      if (lastLine == 'Done.') break;

      // Split the line into the constituent values. Check each value.
      const lineVals = lastLine.split('~');
      const expectedVars: [string, string, string][] = [
        ['testInt', 'int', lineVals[0]],
        ['testDouble', 'double', lineVals[1]],
        ['testChar', 'char', lineVals[2]],
        ['testPtr', 'void *', lineVals[3]],
      ];
      expectedVars.forEach(async ([name, type, expectedVal]) => {
        const tableRow = window.locator('#visualizer tr.variable-row:has-text("' + name + '")');
        expect(await tableRow.isVisible()).toBeTruthy();
        expect(tableRow.locator('td.variable-name')).toContainText(name);
        expect(tableRow.locator('td.variable-type')).toContainText(type);

        // Since the double output is rounded, round the value before comparing.
        if (name == 'testDouble') {
          const rawDouble = await tableRow.locator('td.variable-value').textContent();
          expect(parseFloat(rawDouble).toFixed(2)).toEqual(expectedVal);
        } else {
          expect(tableRow.locator('td.variable-value')).toContainText(expectedVal);
        }
      });
    }
    while ((await runStatus.textContent()) != 'Status: Finished');
  });

  // Semaphore List Values - Check that the value of a semaphore is correctly viewed and updated.
  test('Semaphore List Values', async () => {
    // Set the timeout to 45 seconds because waiting and signaling multiple times is slow.
    test.setTimeout(45000);

    // Open the test file, which regularly waits on and signals random amounts of permits.
    console.log('Please select "semvaluelist.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...

    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success, and then run the program.
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();
    const consoleOut = window.locator('#visualizer div.view-lines.monaco-mouse-cursor-text');
    const runStatus = window.locator('#visualizer div.control:has-text("Status:")');
    const tableRow = window.locator('#visualizer tr.variable-row:has-text("sem")');

    // Whenever a change is made, check whether the sem was deleted.
    // If not, check that the currently-listed values are correct.
    let lastOut = '';
    while (true) {
      while ((await consoleOut.textContent()) == lastOut);
      lastOut = await consoleOut.textContent();

      // Get the most recent output.
      const lastSegment = lastOut.split('|')[lastOut.split('|').length - 1];

      // If this last segment is "new", break for the last check.
      if (lastSegment == 'new') break;

      // Compare to the value of the sem.
      expect(await tableRow.isVisible()).toBeTruthy();
      expect(tableRow.locator('td.variable-name')).toContainText('sem');
      expect(tableRow.locator('td.variable-type')).toContainText('vcSem');
      expect(parseInt(await tableRow.locator('td.variable-value').textContent())).toEqual(parseInt(lastSegment));
    }

    // The last number is the max permit, but the semaphore only has half that number available.
    // Compare this last number to half of the given one.
    // Use Math.floor to ensure it works like C division.
    while ((await consoleOut.textContent()) == lastOut);
    lastOut = await consoleOut.textContent();
    const lastSegment = lastOut.split('|')[lastOut.split('|').length - 1];
    expect(parseInt(await tableRow.locator('td.variable-value').textContent())).toEqual(Math.floor(parseInt(lastSegment) / 2));

    while ((await runStatus.textContent()) != 'Status: Finished');
  });

  // Mutex List Values - Check that the value of a mutex is correctly viewed and updated.
  test('Mutex List Values', async () => {
    // Open the test file, which randomly locks and unlocks a mutex.
    console.log('Please select "mutexvaluelist.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...

    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success, and then run the program.
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();
    const consoleOut = window.locator('#visualizer div.view-lines.monaco-mouse-cursor-text');
    const runStatus = window.locator('#visualizer div.control:has-text("Status:")');
    const tableRow = window.locator('#visualizer tr.variable-row:has-text("mutex")');

    // Whenever a change is made, check whether the sem was deleted.
    // If not, check that the currently-listed values are correct.
    let lastOut = '';
    while (true) {
      while ((await consoleOut.textContent()) == lastOut);
      lastOut = await consoleOut.textContent();

      // When the program ends, it prints something with a '|' in it.
      if (lastOut.split('|').length > 1) break;

      // Compare the output to the value given in the table.
      expect(await tableRow.isVisible()).toBeTruthy();
      expect(tableRow.locator('td.variable-name')).toContainText('mutex');
      expect(tableRow.locator('td.variable-type')).toContainText('vcMutex');
      const lastStatus = lastOut.charAt(lastOut.length - 1);
      if (lastStatus == '1') {
        expect(tableRow.locator('td.variable-value')).toContainText('Unlocked');
      } else {
        expect(tableRow.locator('td.variable-value')).toContainText('Locked by Main Thread');
      }
    }

    while ((await runStatus.textContent()) != 'Status: Finished');
  });

  // Mutex List Owners - Check that the listed owner of a mutex is correctly viewed and updated.
  test('Mutex List Owners', async () => {
    // Open the test file, which creates a set of threads that take turns locking a mutex.
    console.log('Please select "mutexownerlist.c".');
    await window.locator('div.menu-item:has-text("FileNew File")').click();
    await window.locator('span.action-label:text("Open File")').click();

    // File is loaded by tester here...

    // Select Compile > Compile And Run File.
    await window.locator('div.menu-item:has-text("CompileCompile")').click();
    await window.locator('span.action-label:text("Compile And Run File")').click();

    // Wait for the visualizer to appear, which indicates compilation success, and then run the program.
    await window.locator('#visualizer div.control.has-action:has-text("Start Simulation")').click();
    const consoleOut = window.locator('#visualizer div.view-lines.monaco-mouse-cursor-text');
    const runStatus = window.locator('#visualizer div.control:has-text("Status:")');
    const tableRow = window.locator('#visualizer tr.variable-row:has-text("mutex")');

    // Whenever a change is made, check whether the sem was deleted.
    // If not, check that the currently-listed values are correct.
    let lastOut: string = '';
    while (true) {
      while ((await consoleOut.textContent()) == lastOut);
      lastOut = await consoleOut.textContent();

      // When the program ends, it prints something with a '|' in it.
      if (lastOut.split('|').length > 1) break;

      // Compare the output to the value given in the table.
      expect(await tableRow.isVisible()).toBeTruthy();
      expect(tableRow.locator('td.variable-name')).toContainText('mutex');
      expect(tableRow.locator('td.variable-type')).toContainText('vcMutex');
      const curOwner = lastOut.charAt(lastOut.length - 1);
      expect(tableRow.locator('td.variable-value')).toContainText('Locked by |' + curOwner + '|');
    }

    while ((await runStatus.textContent()) != 'Status: Finished');
  });

  // After Each - Quit the program if needed, return to the editor and close all open files.
  test.afterEach(async () => {
    // Quit the program if it is open and return to the editor.
    if (await window.isVisible('[title="Return to Editor"]')) {
      if (await window.isVisible('#visualizer div.control.has-action:has-text("Force Quit Simulation")')) {
        await window.locator('#visualizer div.control.has-action:has-text("Force Quit Simulation")').click();
        while ((await window.locator('#visualizer div.control:has-text("Status:")').textContent()) != 'Status: Terminated');
      }
      await window.locator('[title="Return to Editor"]').click();
    }

    // Close all open files.
    while (true) {
      if (await window.isVisible('#ide a.action-label.codicon.codicon-close'))
        await window.locator('#ide a.action-label.codicon.codicon-close').first().click();
      else break;
    }
  });

  // After All - Exit app.
  test.afterAll(async () => {
    window = null;
    await electronApp.close();
  });
});
