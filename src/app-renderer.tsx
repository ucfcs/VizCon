import React, { StrictMode, useState } from 'react';
import * as ReactDOM from 'react-dom';
import Nav from './components/nav';
import IDE from './components/ide';

const root = document.getElementById('vizcon');
document.body.classList.add(window.platform.getPlatform());

ReactDOM.render(<App />, root);

function App(): React.ReactElement {
  const [files, setFiles] = useState<Array<string>>([]);
  const [current, setCurrent] = useState(files.length > 0 ? files[0] : '');

  function openFile(file: string): boolean {
    setFiles([...files, file]);
    setCurrent(file);
    return true;
  }

  return (
    <StrictMode>
      <Nav openFile={openFile}/>
      <IDE files={files} current={current} setCurrent={setCurrent}/>
    </StrictMode>
  );
}
