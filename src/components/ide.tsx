import React, { useEffect, useState } from 'react';
import '../styles/ide.scss';
import Editor from './ide/editor';
import Landing from './ide/landing';
import Output from './ide/output';
import Tab from './ide/tab';
import ScrollableElement from './scrollableElement';

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
  newFile: () => void;
  openFile: () => void;
  openExampleFile: () => void;
  compileAndRunFile: () => void;
  openUserGuide: () => void;
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
  openExampleFile,
  compileAndRunFile,
  openUserGuide,
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
      <div className="tabination-and-play">
        <ScrollableElement content={tabination} />
        <div className="editor-actions">
          <ul className="editor-actions-container">
            <li className="action-item">
              {files.length == 0 && (
                <a className="action-label codicon codicon-play disabled" role="button" title="Simulate File" onClick={compileAndRunFile} />
              )}
              {files.length >= 1 && (
                <a className="action-label codicon codicon-play" role="button" title="Simulate File" onClick={compileAndRunFile} />
              )}
            </li>
          </ul>
        </div>
      </div>
      <div className="active-editor">
        {files.length == 0 && (
          <Landing newFile={newFile} openFile={openFile} openExampleFile={openExampleFile} openUserGuide={openUserGuide} />
        )}
        {files.length >= 1 && <Editor current={current} regenTabination={regenTabination} setDirty={setDirty} />}
      </div>
      {showOutput && <Output current={current} text={compileResults} closePanel={closeOutput} />}
    </div>
  );
}
