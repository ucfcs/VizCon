import React, { useEffect, useState } from 'react';
import '../styles/ide.scss';
import Editor from './ide/editor';
import Landing from './ide/landing';

interface IDEProps {
  files: string[];
}

interface TabProps {
  setActive: () => void;
  name: string;
  current: string;
}

function Tab({ setActive, name, current }: TabProps): React.ReactElement {
  const [cls, setCls] = useState<string>('tab');
  // TODO: fancy name reduction so it is only the file name and not the whole path
  // TODO: the start simulation button

  // TODO RESUME: fix the tab problem
  useEffect(() => {
    setCls('tab' + (current === name ? ' active' : ''));
  }, [current]);

  return (
    <div className={cls} onClick={setActive} draggable>
      {/* TODO: Icon goes here */}
      <div className="tab-label">{name}</div>
      {/* TODO: the actions area */}
      <div className="tab-actions"></div>
    </div>
  );
}

export default function IDE({ files }: IDEProps): React.ReactElement {
  const [landing] = useState(<Landing />);
  const [current, setCurrent] = useState(files.length > 0 ? files[0] : '');
  const [editors, setEditors] = useState<{ [key: string]: React.ReactElement }>({});
  const [tabination, setTabination] = useState(<div className="tabination"></div>);
  const [currentEditor, setCurrentEditor] = useState(landing);

  useEffect(() => {
    // Updates the editors
    const newEditors: { [key: string]: React.ReactElement } = {};
    files.forEach(file => {
      // If an editor exists, keep that one
      if (editors[file]) {
        newEditors[file] = editors[file];
        return;
      }

      // else make a new editor
      newEditors[file] = <Editor file={file}/>;
    });
    setEditors(newEditors);
  }, [files]);

  useEffect(() => {
    // Create the tabs for the files
    setTabination(
      <div className="tabination">
        {files.map(file => {
          // for each file in the file map, return a new tab with the key being the file name
          const setActive = () => {
            setCurrent(file);
          };
          return <Tab setActive={setActive} name={file} current={current} key={file} />;
        })}
      </div>
    );
  }, [current, files]);

  useEffect(() => {
    console.log('Setting editor to:', current, editors[current], editors);
    if (current && current !== '' && editors[current]) {
      setCurrentEditor(editors[current]);
    } else {
      setCurrentEditor(landing)
    }
  }, [current, editors]);

  return (
    <div id="ide">
      {tabination}
      <div className="active-editor">{currentEditor}</div>
    </div>
  );
}
