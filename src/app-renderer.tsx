import React, { StrictMode, useState } from 'react';
import * as ReactDOM from 'react-dom';
import Nav from './components/nav';
import IDE from './components/ide';

const root = document.getElementById('vizcon');
document.body.classList.add(window.platform.getPlatform());

ReactDOM.render(<App />, root);

// TODO: better management of untitled filess, when the 1st is deleted, it should be the next to be openeded
let untitledCount = 1;

function App(): React.ReactElement {
  const [files, setFiles] = useState<Array<OpenFileData>>([]);
  // TODO: remove the files length check, that is for when we hard code the initial files
  // Unless we allow the initial content of the files array to be the files they previously had open
  const [current, setCurrent] = useState(files.length > 0 ? files[0] : { path: 'Landing', fileContent: '', currentContent: '' });

  function openFile(): void {
    window.platform.openFileDialog().then(async newFiles => {
      const newFileContents = await window.platform.readFilesSync(newFiles);
      // if no files were selected, dont attempt to read anything
      if (newFileContents[0].includes('EMPTY:')) {
        return;
      }
      const newFileData = newFileContents.map((diskContent, i): OpenFileData => {
        return {
          path: newFiles[i],
          fileContent: diskContent,
          currentContent: diskContent
        };
      });
      setFiles([...files, ...newFileData]);
      setCurrent(newFileData[newFileData.length - 1]);
    });
  }

  function openBlankFile(): void {
    const blank: OpenFileData = {
      path: 'tacking://Untitled-' + untitledCount,
      fileContent: '', // TODO: do we enable a default template?
      currentContent: ''
    };
    untitledCount++;
    setFiles([...files, blank]);
    setCurrent(blank);
  }

  async function saveFileImpl(file: OpenFileData, forceDialog?: boolean): Promise<void> {
    const writtenPath = await window.platform.saveFileToDisk(file.path, file.currentContent, forceDialog);
    console.log(`saving file ${file.path}, satatus ${writtenPath}`);
    current.path = writtenPath;
  }

  async function saveFile(): Promise<void> {
    saveFileImpl(current);
  }

  async function saveAs(): Promise<void> {
    saveFileImpl(current, true);
  }

  function saveAll(): void {
    files.forEach(async file =>  {
      saveFileImpl(file);
    });
  }

  return (
    <StrictMode>
      <Nav openFile={openFile} openBlankFile={openBlankFile} saveFile={saveFile} saveAll={saveAll} saveAs={saveAs} />
      <IDE files={files} current={current} setCurrent={setCurrent} />
    </StrictMode>
  );
}
