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
  // Before All - Launch the app and get the first window.
  test.beforeAll(async () =>
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
    const clipboardText = await window.evaluate(() => navigator.clipboard.readText());
    expect(clipboardText).toBe(rand);

    // Check that the string is not in the text area.
    const file_contents = await window.locator('#ide div.view-line').textContent();
    expect(file_contents).toBe("");
  });

  // Cut Overwrite - Check that only the most-recently cut string is saved.
  test('Cut Overwrite', async () =>
  {
    // Generate a random string, type it, and select it.
    const rand = makeString(testStringSize);
    await window.type('#ide div.view-line', rand);
    await window.keyboard.press('Control+A');

    // Press Ctrl + X.
    await window.keyboard.press('Control+X');

    // Check the clipboard and compare it to the random string.
    const clipboardText = await window.evaluate(() => navigator.clipboard.readText());
    expect(clipboardText).toBe(rand);

    // Generate another random string, type it, and select it.
    const rand2 = makeString(testStringSize);
    await window.type('#ide div.view-line', rand2);
    await window.keyboard.press('Control+A');

    // Press Ctrl + X.
    await window.keyboard.press('Control+X');

    // Check the clipboard and compare it to both the second string and previous contents.
    const clipboardText2 = await window.evaluate(() => navigator.clipboard.readText())
    expect(clipboardText2).toBe(rand2);
    expect(clipboardText2).not.toBe(clipboardText);
  });

  // Cut Portion - Check that the cut function only saves and removes selected text.
  test('Cut Portion', async () =>
  {
    // Generate three random strings and type all of them in order.
    const rand1 = makeString(testStringSize);
    const rand2 = makeString(testStringSize);
    const rand3 = makeString(testStringSize);
    await window.type('#ide div.view-line', rand1 + " " + rand2 + " " + rand3);

    // Select the second string.
    // Move the cursor to the word and double-click.
    await window.keyboard.press('ArrowRight');
    for(var i = 0; i < testStringSize + 1; i++)
      await window.keyboard.press('ArrowLeft');
    await window.keyboard.press('Control+D');

    // Press Ctrl + X.
    await window.keyboard.press('Control+X');

    // Check the clipboard and compare it to the second string.
    const clipboardText = await window.evaluate(() => navigator.clipboard.readText())
    expect(clipboardText).toBe(rand2);

    // Check that the text area still contains the first and third string but not the second.
    const file_contents = await window.locator('#ide div.view-line');
    await expect(file_contents).toHaveText(rand1 + "  " + rand3);
    await expect(file_contents).not.toHaveText(rand2);
  });

  // Copy - Check that the copy keyboard shortcut adds text to the clipboard without removing it.
  test('Copy', async () =>
  {
    // Generate a random string, type it, and select it.
    const rand = makeString(testStringSize);
    await window.type('#ide div.view-line', rand);
    await window.keyboard.press('Control+A');

    // Press Ctrl + C.
    await window.keyboard.press('Control+C');

    // Check the clipboard and compare it to the random string.
    const clipboardText = await window.evaluate(() => navigator.clipboard.readText());
    expect(clipboardText).toBe(rand);

    // Check that the string is in the text area.
    const file_contents = await window.locator('#ide div.view-line');
    expect(file_contents).toContainText(rand);
  });

  // Copy Overwrite - Check that only the most-recently copied string is saved.
  test('Copy Overwrite', async () =>
  {
    // Generate a random string, type it, and select it.
    const rand = makeString(testStringSize);
    await window.type('#ide div.view-line', rand);
    await window.keyboard.press('Control+A');

    // Press Ctrl + C.
    await window.keyboard.press('Control+C');

    // Check the clipboard and compare it to the random string.
    const clipboardText = await window.evaluate(() => navigator.clipboard.readText());
    expect(clipboardText).toBe(rand);

    // Generate another random string and type it.
    // Then select the full text, which should be rand and rand2 together.
    const rand2 = makeString(testStringSize);
    await window.keyboard.press('ArrowRight');
    await window.type('#ide div.view-line', rand2);
    await window.keyboard.press('Control+A');

    // Press Ctrl + C.
    await window.keyboard.press('Control+C');

    // Check the clipboard and compare it to both the concatenated string and previous contents.
    const clipboardText2 = await window.evaluate(() => navigator.clipboard.readText())
    expect(clipboardText2).toBe(rand + rand2);
    expect(clipboardText2).not.toBe(clipboardText);
  });

  // Copy Portion - Check that the cut function only saves selected text.
  test('Copy Portion', async () =>
  {
    // Generate three random strings and type all of them in order.
    const rand1 = makeString(testStringSize);
    const rand2 = makeString(testStringSize);
    const rand3 = makeString(testStringSize);
    await window.type('#ide div.view-line', rand1 + " " + rand2 + " " + rand3);

    // Select the second string.
    // Move the cursor to the word and press Ctrl + D.
    await window.keyboard.press('ArrowRight');
    for(var i = 0; i < testStringSize + 1; i++)
      await window.keyboard.press('ArrowLeft');
    await window.keyboard.press('Control+D');

    // Press Ctrl + C.
    await window.keyboard.press('Control+C');

    // Check the clipboard and compare it to the second string.
    const clipboardText = await window.evaluate(() => navigator.clipboard.readText())
    expect(clipboardText).toBe(rand2);
  });

  // Paste - Check that the paste keyboard shortcut writes the clipboard content.
  test('Paste', async () =>
  {
    // Generate a random string and copy it to the clipboard.
    const rand = makeString(testStringSize);
    await window.evaluate(str => navigator.clipboard.writeText(str), rand);

    // Press Ctrl + V.
    await window.keyboard.press('Control+V');

    // Check the text and compare it to the random string.
    const file_contents = await window.locator('#ide div.view-line');
    expect(file_contents).toContainText(rand);
    
    // Check that the string is still in the clipboard.
    const clipboardText = await window.evaluate(() => navigator.clipboard.readText());
    expect(clipboardText).toBe(rand);
  });

  // Paste at Cursor - Check that the paste function occurs at the cursor.
  test('Paste at Cursor', async () =>
  {
    // Generate three random strings.
    // Type the first two and save the third to the clipboard.
    const rand1 = makeString(testStringSize);
    const rand2 = makeString(testStringSize);
    const rand3 = makeString(testStringSize);
    await window.type('#ide div.view-line', rand1 + rand2);
    await window.evaluate(str => navigator.clipboard.writeText(str), rand3);
    
    // Move the cursor between the first and second string.
    await window.keyboard.press('ArrowRight');
    for(var i = 0; i < testStringSize; i++)
      await window.keyboard.press('ArrowLeft');

    // Press Ctrl + V.
    await window.keyboard.press('Control+V');

    // Check that the text area contains the strings in the correct order (1, 3, 2).
    const file_contents = await window.locator('#ide div.view-line');
    expect(file_contents).toContainText(rand1 + rand3 + rand2);

    // Move to the front and paste again.
    await window.keyboard.press('Control+A');
    await window.keyboard.press('ArrowLeft');
    await window.keyboard.press('Control+V');

    // Check that the text area contains the strings in the correct order (3, 1, 3, 2).
    const file_contents2 = await window.locator('#ide div.view-line');
    expect(file_contents2).not.toBe(file_contents);
    expect(file_contents).toContainText(rand3 + rand1 + rand3 + rand2);
  });

  // Paste Overwrite - Check that pasted text overwrites selected text.
  test('Paste Overwrite', async () =>
  {
    // Generate three random strings.
    // Type the first two and save the third to the clipboard.
    const rand1 = makeString(testStringSize);
    const rand2 = makeString(testStringSize);
    const rand3 = makeString(testStringSize);
    await window.type('#ide div.view-line', rand1 + " " + rand2);
    await window.evaluate(str => navigator.clipboard.writeText(str), rand3);

    // Select the first string.
    await window.keyboard.press('Control+A');
    await window.keyboard.press('ArrowLeft');
    await window.keyboard.press('Control+D');

    // Press Ctrl + V.
    await window.keyboard.press('Control+V');

    // Check that the text area contains only the third and second string.
    const file_contents = await window.locator('#ide div.view-line');
    expect(file_contents).toContainText(rand3 + " " + rand2);
  });

  // After Each - Clear the text area and clipboard.
  test.afterEach(async () =>
  {
    // Clear the text area.
    await window.locator('#ide div.view-line').click();
    await window.keyboard.press('Control+A');
    await window.keyboard.press('Delete');

    // Clear the clipboard.
    await window.evaluate(() => navigator.clipboard.writeText(''));
  });
  
  // After All - Exit app.
  test.afterAll(async () =>
  {
    window = null;
    await electronApp.close();
  });
});