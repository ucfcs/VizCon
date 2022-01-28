import React, { useEffect, useState } from 'react';
import '../styles/ide.scss';
import EditorOld from './ide/editorold';
import Landing from './ide/landing';
import Tab from './ide/tab';

interface IDEProps {
  files: string[];
  current: string;
  setCurrent: (_: string) => void;
}

export default function IDE({ files, current, setCurrent }: IDEProps): React.ReactElement {
  const [landing] = useState(<Landing />);
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
      newEditors[file] = <EditorOld file={file} key={file}/>;
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
          return <Tab setActive={setActive} name={file} current={current} key={'tab' + file} />;
        })}
      </div>
    );
  }, [current, files]);

  useEffect(() => {
    if (current && current !== '' && editors[current]) {
      setCurrentEditor(editors[current]);
    } else {
      setCurrentEditor(landing);
    }
  }, [current, editors]);

  return (
    <div id="ide">
      {tabination}
      <div className="active-editor">{currentEditor}</div>
    </div>
  );
}
