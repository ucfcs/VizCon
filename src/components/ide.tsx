import React, { useEffect, useState } from 'react';
import '../styles/ide.scss';
import Editor from './ide/editor';
import Landing from './ide/landing';
import Output from './ide/output';
import Tab from './ide/tab';

interface IDEProps {
  files: OpenFileData[];
  current: OpenFileData;
  setCurrent: (_: OpenFileData) => void;
  setDirty: (_: boolean) => void;
  closeFile: (_: OpenFileData) => void;
  showOutput: boolean;
  closeOutput: () => void;
  compileResults: string;
  inVisualizer: boolean;
  openFile: () => void;
  newFile: () => void;
}

export default function IDE({
  files,
  current,
  setCurrent,
  setDirty,
  closeFile,
  compileResults,
  showOutput,
  closeOutput,
  inVisualizer,
  newFile,
  openFile,
}: IDEProps): React.ReactElement {
  const [tabination, setTabination] = useState(<div className="tabination"></div>);
  const [className, setClassName] = useState('');

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
          return <Tab setActive={setActive} close={close} name={file.path} dirty={file.dirty} current={current} key={'tab' + file.path} />;
        })}
      </div>
    );
  }

  useEffect(() => {
    regenTabination();
    setDirty(current.dirty);
  }, [current, files]);

  useEffect(() => {
    setClassName((showOutput ? 'output-visible' : '') + ' ' + (inVisualizer ? 'hidden' : ''));
  }, [showOutput, inVisualizer]);

  return (
    <div id="ide" className={className}>
      {tabination}
      <div className="active-editor">
        {files.length == 0 && <Landing newFile={newFile} openFile={openFile} />}
        {files.length >= 1 && <Editor current={current} regenTabination={regenTabination} setDirty={setDirty} />}
      </div>
      {showOutput && <Output current={current} text={compileResults} closePanel={closeOutput} />}
    </div>
  );
}
