import React, { StrictMode, useState } from 'react';
import * as ReactDOM from 'react-dom';
import Nav from './components/nav';
import IDE from './components/ide';
import Visualizer from './components/visualizer';
import { filePathToShortName } from './util/utils';

const root = document.getElementById('vizcon');
document.body.classList.add(window.platform.getPlatform());
const defaultCurrent = { path: 'tracking://Landing', fileContent: '', currentContent: '', dirty: false };

// TODO: better management of untitled files, when the 1st is deleted, it should be the next to be opened
let untitledCount = 1;

function App(): React.ReactElement {
  const [files, setFiles] = useState<Array<OpenFileData>>([]);
  const [dirty, setDirty] = useState(false);
  const [current, setCurrent] = useState(defaultCurrent);
  const [outputVisible, setOutputVisible] = useState(false);
  const [compileResult, setCompileResult] = useState('');
  const [inVisualizer, setInVisualizer] = useState(false);

  async function handleNewFiles(newFiles: string[]) {
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
  }

  function openFile(): void {
    window.platform.openFileDialog().then(handleNewFiles);
  }

  function openExampleFile(): void {
    window.platform.openExampleFileDialog().then(handleNewFiles);
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

  async function saveFileImpl(file: OpenFileData, forceDialog?: boolean): Promise<OpenFileData> {
    const writtenPath = await window.platform.saveFileToDisk(file.path, file.currentContent, forceDialog);
    console.log(`saving file ${file.path}, status ${writtenPath}`);

    const newFiles = [...files];
    for (let i = 0; i < newFiles.length; i++) {
      if (newFiles[i].path === file.path) {
        newFiles[i].path = writtenPath;
        newFiles[i].dirty = false;
        newFiles[i].fileContent = newFiles[i].currentContent;
        setFiles(newFiles);
        setCurrent(newFiles[i]);
        return newFiles[i];
      }
    }
  }

  async function saveFile(): Promise<void> {
    saveFileImpl(current);
  }

  async function saveAs(): Promise<void> {
    saveFileImpl(current, true);
  }

  async function saveAll(): Promise<void> {
    const newFiles = [...files];
    let oldCurrentPath = current.path;
    let newCurrent: OpenFileData;

    for (let i = 0; i < newFiles.length; i++) {
      const file = newFiles[i];
      if (!file.dirty) {
        continue;
      }

      const writtenPath = await window.platform.saveFileToDisk(file.path, file.currentContent, false);
      console.log(`saving file ${file.path}, status ${writtenPath}`);

      if (oldCurrentPath === file.path) {
        oldCurrentPath = writtenPath;
      }

      for (let j = 0; j < newFiles.length; j++) {
        if (newFiles[j].path === file.path) {
          newFiles[j].path = writtenPath;
          newFiles[j].dirty = false;
          newFiles[j].fileContent = newFiles[j].currentContent;
          break;
        }
      }
    }

    files.forEach(file => {
      if (file.path === oldCurrentPath) {
        newCurrent = file;
      }
    });

    setFiles(newFiles);
    setCurrent(newCurrent || current);
  }

  async function closeFile(file: OpenFileData): Promise<void> {
    if (file.dirty) {
      const response = await window.platform.showUnsavedSaveDialog(filePathToShortName(file.path));

      if (response === 'cancel') {
        return;
      }

      if (response === 'save') {
        file = (await saveFileImpl(file)) || file;
      }

      // if dontsave or save, fall through to the rest of the close code
    }

    const filesNew = files.filter(f => {
      return f !== file;
    });

    setFiles(filesNew);

    // dont worry about checking the current, there is no current to set
    if (filesNew.length === 0) {
      setTimeout(() => {
        setCurrent(defaultCurrent);
      }, 5);
      return;
    }

    if (current.path === file.path) {
      let nextSibling = files.indexOf(file);
      if (nextSibling >= filesNew.length) {
        nextSibling = filesNew.length - 1;
      }

      const newCurrent = filesNew[nextSibling];

      // lets be clear, this is a hack. For what ever reason though, react is not updating which file is current without this
      setTimeout(() => {
        setCurrent(newCurrent);
      }, 5);
    } else {
      for (let i = 0; i < filesNew.length; i++) {
        if (filesNew[i].path === current.path) {
          setTimeout(() => {
            setCurrent(filesNew[i]);
          }, 5);
          break;
        }
      }
    }
  }

  async function compile(run: boolean): Promise<void> {
    if (current.path === 'tracking://Landing') {
      return;
    }

    document.body.classList.add('compiling');
    let file = current;

    // handle atttempting to compile an unsaved file
    if (current.dirty) {
      const response = await window.platform.showUnsavedCompileDialog(filePathToShortName(file.path));

      if (response === 'cancel') {
        return;
      }

      if (response === 'save') {
        file = (await saveFileImpl(file)) || file;
      }
    }

    const results = await window.platform.compileFile(file.path);
    console.log(results);

    document.body.classList.remove('compiling');

    if (results.err !== '' && results.err.includes('error: ')) {
      setOutputVisible(true);
      setCompileResult(results.err);
      return;
    }

    if (run) {
      // TODO: hook up the run command
      setInVisualizer(true);
    } else {
      setOutputVisible(true);
      if (results?.err.includes('warning: ')) {
        setCompileResult('Compilation succeeded with warnings.\n' + results.err);
      } else {
        setCompileResult('Compilation succeeded with no warnings or errors.');
      }
    }
  }

  return (
    <>
      <Nav
        current={current}
        dirty={dirty}
        visualizerActive={inVisualizer}
        openFile={openFile}
        openExampleFile={openExampleFile}
        openBlankFile={openBlankFile}
        saveFile={saveFile}
        saveAll={saveAll}
        saveAs={saveAs}
        compile={() => compile(false)}
        compileAndRun={() => compile(true)}
        showCompileOutput={() => setOutputVisible(true)}
        showVisualizer={() => setInVisualizer(true)}
        showEditor={() => setInVisualizer(false)}
        closeFile={() => closeFile(current)}
        // TODO: additional window close actions
        closeWindow={() => window.close()}
      />
      <IDE
        files={files}
        current={current}
        setCurrent={setCurrent}
        setDirty={setDirty}
        closeFile={closeFile}
        compileResults={compileResult}
        showOutput={outputVisible}
        closeOutput={() => setOutputVisible(false)}
        inVisualizer={inVisualizer}
        newFile={openBlankFile}
        openFile={openFile}
        openExampleFile={openExampleFile}
      />
      <Visualizer inVisualizer={inVisualizer} current={current} goBack={() => setInVisualizer(false)} />
    </>
  );
}

ReactDOM.render(
  <StrictMode>
    <App />
  </StrictMode>,
  root
);
