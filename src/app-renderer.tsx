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
  // TODO: system to account for a new blank file
  const [files, setFiles] = useState<Array<OpenFileData>>([]);
  // TODO: remove the files length check, that is for when we hard code the initial files
  // Unless we allow the initial content of the files array to be the files they previously had open
  const [current, setCurrent] = useState(files.length > 0 ? files[0] : { path: 'Landing', content: '' });

  function openFile(): void {
    window.platform.openFileDialog().then(async newFiles => {
      const newFileContents = await window.platform.readFilesSync(newFiles);
      const newFileData = newFileContents.map((fileContent, i): OpenFileData => {
        return {
          path: newFiles[i],
          content: fileContent,
        };
      });
      setFiles([...files, ...newFileData]);
      setCurrent(newFileData[newFileData.length - 1]);
    });
  }

  function openBlankFile(): void {
    const blank: OpenFileData = {
      path: 'Untitled ' + untitledCount,
      content: '', // TODO: do we enable a default template?
    };
    untitledCount++;
    setFiles([...files, blank]);
    setCurrent(blank);
  }

  return (
    <StrictMode>
      <Nav openFile={openFile} openBlankFile={openBlankFile} />
      <IDE files={files} current={current} setCurrent={setCurrent} />
    </StrictMode>
  );
}
