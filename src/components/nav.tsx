import React from 'react';
import WindowControls from './nav/windowcontrols';
import MenuItem from './nav/menuitem';
import '../styles/nav.scss';

interface NavProps {
  openFile: () => void;
  openBlankFile: () => void;
}

export default function Nav({ openFile, openBlankFile }: NavProps): React.ReactElement {
  const showMenu = window.platform.getPlatform() !== 'darwin';
  // const showMenu = true;

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
      {/* TODO: have this title be dynamic, and when it updates, update window.title */}
      <div className="window-title">Temporary Title</div>
      {showMenu && (
        <>
          <WindowControls />
        </>
      )}
    </div>
  );
}
