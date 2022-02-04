import React, { useEffect, useState } from 'react';
import { filePathToShortName } from './ide/tab';
import WindowControls from './nav/windowcontrols';
import MenuItem from './nav/menuitem';
import '../styles/nav.scss';

interface NavProps {
  openFile: () => void;
  openBlankFile: () => void;
  saveFile: () => void;
  saveAll: () => void;
  saveAs: () => void;
  current: OpenFileData;
}

export default function Nav({ openFile, openBlankFile, saveFile, saveAll, saveAs, current }: NavProps): React.ReactElement {
  const showMenu = window.platform.getPlatform() !== 'darwin';
  // const showMenu = true;
  
  const [title, setTitle] = useState('');

  useEffect(() => {
    let reactTitle = '';
    if (current.path !== 'tracking://Landing') {
      const shortName = filePathToShortName(current.path);
      const titlePrefix = current.dirty ? '• ' : '';
      reactTitle = titlePrefix + shortName + ' - ';
    }
    
    setTitle(reactTitle);
    document.title = reactTitle + 'VizCon';
  }, [current, current.dirty]);

  return (
    <div className="titlebar">
      <div className="titlebar-drag-region"></div>
      {showMenu && (
        <>
          <a className="appicon"></a>
          <div className="menubar" role="menubar">
            <MenuItem
              title="File"
              options={[
                { name: 'Open File', action: openFile },
                { name: 'New File', action: openBlankFile },
                { name: 'Save File', action: saveFile },
                { name: 'Save All', action: saveAll },
                { name: 'Save As', action: saveAs },
              ]}
            />
            <MenuItem
              title="View"
              options={[
                {
                  name: 'Zoom In',
                  action: () => {
                    console.log('zoom in');
                  },
                },
                {
                  name: 'Zoom Out',
                  action: () => {
                    console.log('zoom out');
                  },
                },
              ]}
            />
          </div>
        </>
      )}
      {/* TODO: have this track if the Editor or Visualizer is open, append it to the end ot title */}
      <div className="window-title">{title}VizCon</div>
      {showMenu && (
        <>
          <WindowControls />
        </>
      )}
    </div>
  );
}
