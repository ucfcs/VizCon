import React, { useEffect, useState } from 'react';
import '../styles/ide.scss';
import Editor from './ide/editor';
import Landing from './ide/landing';
import Tab from './ide/tab';

interface IDEProps {
  files: OpenFileData[];
  current: OpenFileData;
  setCurrent: (_: OpenFileData) => void;
  closeFile: (_: OpenFileData) => void;
}

export default function IDE({ files, current, setCurrent, closeFile }: IDEProps): React.ReactElement {
  const [tabination, setTabination] = useState(<div className="tabination"></div>);

  function regenTabination() {
    // Create the tabs for the files
    setTabination(
      <div className="tabination">
        {files.map(file => {
          // for each file in the file map, return a new tab with the key being the file name
          const setActive = () => {
            setCurrent(file);
          };
          const close = () => {
            closeFile(file);
          };
          return (
            <Tab
              setActive={setActive}
              close={close}
              name={file.path}
              dirty={file.dirty}
              current={current}
              key={'tab' + file.path}
            />
          );
        })}
      </div>
    );
  }

  useEffect(() => {
    regenTabination();
  }, [current, current.dirty, files]);

  return (
    <div id="ide">
      {tabination}
      <div className="active-editor">
        {files.length == 0 && <Landing />}
        {files.length >= 1 && <Editor current={current} regenTabination={regenTabination} />}
      </div>
    </div>
  );
}
