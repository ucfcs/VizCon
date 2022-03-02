function filePathToShortName(name: string): string {
  let shortName = name.replace(/\\/g, '/');
  if (shortName.lastIndexOf('/')) {
    shortName = shortName.substring(shortName.lastIndexOf('/') + 1);
  }
  return shortName;
}

function filePathToFileName(name: string): string {
  const shortName = filePathToShortName(name);
  const idx = shortName.lastIndexOf('.');
  return shortName.substring(0, idx);
}

function getDisplayValueForBool(bool: boolean): string {
  return bool ? 'true' : 'false';
}

function delay(millis: number): Promise<void> {
  return new Promise((resolve, reject) => {
    setTimeout(
      () => { resolve(); },
      millis
    );
  });
}

export { filePathToShortName, filePathToFileName, getDisplayValueForBool, delay };
