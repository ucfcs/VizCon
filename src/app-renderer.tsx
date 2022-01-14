import React, { StrictMode, useState } from 'react';
import * as ReactDOM from 'react-dom';
import { MemoryRouter, Route, Routes } from 'react-router-dom';
import Nav from './components/nav';
import IDE from './components/ide';

const root = document.getElementById('vizcon');
document.body.classList.add(window.platform.getPlatform());

ReactDOM.render(<App />, root);

function App(): React.ReactElement {
  const [files, setFiles] = useState<Array<string>>(['hw1.c', 'bruh.c']);

  function openFile(file: string): boolean {
    setFiles([...files, file]);
    return true;
  }

  return (
    <StrictMode>
      <Nav openFile={openFile}/>
      <IDE files={files}/>
    </StrictMode>
  );
}
