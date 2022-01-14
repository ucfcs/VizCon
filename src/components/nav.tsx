import React from 'react';
import WindowControls from './nav/windowcontrols';
import MenuItem from './nav/menuitem';
import '../styles/nav.scss';

interface NavProps {
  openFile: (file: string) => boolean; // TODO: this will be used later
}

export default function Nav({openFile}: NavProps): React.ReactElement {
  const showMenu = window.platform.getPlatform() !== 'darwin';

  return (
    <div className="titlebar" style={{ height: '30px' }}>
      {showMenu && (
        <>
          <div className="titlebar-drag-region"></div>
          <a className="appicon"></a>
          <div className="menubar" role="menubar">
            <MenuItem />
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
