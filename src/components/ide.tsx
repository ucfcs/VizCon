import React, { useEffect, useState } from 'react';
import '../styles/ide.scss';
import Editor from './ide/editor';
import Landing from './ide/landing';
import Tab from './ide/tab';

interface IDEProps {
  files: OpenFileData[];
  current: OpenFileData;
  setCurrent: (_: OpenFileData) => void;
}

export default function IDE({ files, current, setCurrent }: IDEProps): React.ReactElement {
  const [tabination, setTabination] = useState(<div className="tabination"></div>);

  useEffect(() => {
    // Create the tabs for the files
    setTabination(
      <div className="tabination">
        {files.map(file => {
          // for each file in the file map, return a new tab with the key being the file name
          const setActive = () => {
            setCurrent(file);
          };
          return <Tab setActive={setActive} name={file.path} current={current} key={'tab' + file.path} />;
        })}
      </div>
    );
  }, [current, files]);

  return (
    <div id="ide">
      {tabination}
      <div className="active-editor">
        {files.length == 0 && <Landing />}
        {files.length >= 1 && <Editor current={current} />}
      </div>
    </div>
  );
}
