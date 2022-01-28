import React, { StrictMode, useState } from 'react';
import * as ReactDOM from 'react-dom';
import Nav from './components/nav';
import IDE from './components/ide';

const root = document.getElementById('vizcon');
document.body.classList.add(window.platform.getPlatform());

ReactDOM.render(<App />, root);

function App(): React.ReactElement {
  // TODO: system to account for a new blank file
  const [files, setFiles] = useState<Array<string>>([]);
  const [current, setCurrent] = useState(files.length > 0 ? files[0] : '');

  function openFile(): void {
    window.platform.openFileDialog().then(newFiles => {
      setFiles([...files, ...newFiles]);
      setCurrent(newFiles[newFiles.length - 1]);
    });
  }

  return (
    <StrictMode>
      <Nav openFile={openFile} />
      <IDE files={files} current={current} setCurrent={setCurrent} />
    </StrictMode>
  );
}
