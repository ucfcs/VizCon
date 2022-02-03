import React, { StrictMode, useState } from 'react';
import * as ReactDOM from 'react-dom';
import Nav from './components/nav';
import IDE from './components/ide';

const root = document.getElementById('vizcon');
document.body.classList.add(window.platform.getPlatform());
const defaultCurrent = { path: 'Landing', fileContent: '', currentContent: '', dirty: false };

// TODO: better management of untitled filess, when the 1st is deleted, it should be the next to be openeded
let untitledCount = 1;


function App(): React.ReactElement {
  const [files, setFiles] = useState<Array<OpenFileData>>([]);
  // TODO: remove the files length check, that is for when we hard code the initial files
  // Unless we allow the initial content of the files array to be the files they previously had open
  const [current, setCurrent] = useState(defaultCurrent);

  function openFile(): void {
    window.platform.openFileDialog().then(async newFiles => {
      // if no files were selected, dont attempt to read anything
      if (newFiles[0].includes('EMPTY:')) {
        return;
      }

      // filter out already open files
      const filteredFiles = newFiles.filter(file => {
        for (let i = 0; i < files.length; i++) {
          if (files[i].path === file) {
            return false;
          }
        }
        return true;
      });

      const newFileContents = await window.platform.readFilesSync(filteredFiles);
      const newFileData = newFileContents.map((diskContent, i): OpenFileData => {
        return {
          path: filteredFiles[i],
          fileContent: diskContent,
          currentContent: diskContent,
          dirty: false,
        };
      });
      setFiles([...files, ...newFileData]);
      setCurrent(newFileData[newFileData.length - 1]);
    });
  }

  function openBlankFile(): void {
    const blank: OpenFileData = {
      path: 'tracking://Untitled-' + untitledCount,
      fileContent: '', // TODO: do we enable a default template?
      currentContent: '',
      dirty: false,
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
    files.forEach(async file => {
      saveFileImpl(file);
    });
  }

  function closeFile(file: OpenFileData): void {
    const filesNew = files.filter(f => {
      return f !== file;
    });

    setFiles(filesNew);

    // dont worry about checking the current, there is no current to set
    if (filesNew.length === 0) {
      setCurrent(defaultCurrent);
      return;
    }

    console.log(current, file);
    if (current.path === file.path) {
      let nextSibling = files.indexOf(file);
      if (nextSibling >= filesNew.length) {
        nextSibling = filesNew.length - 1;
      }

      const newCurrent = filesNew[nextSibling];
      console.log('current is file', nextSibling, newCurrent);

      // lets be clear, this is a hack. For what ever reason though, react is not updating which file is current when i call this
      setTimeout(() => {
        setCurrent(newCurrent);
      }, 5);
    }
  }

  return (
    <>
      <Nav openFile={openFile} openBlankFile={openBlankFile} saveFile={saveFile} saveAll={saveAll} saveAs={saveAs} />
      <IDE files={files} current={current} setCurrent={setCurrent} closeFile={closeFile} />
    </>
  );
}

ReactDOM.render(
  <StrictMode>
    <App />
  </StrictMode>,
  root
);
