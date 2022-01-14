import React from 'react';
import WindowControls from './nav/windowcontrols';
import MenuItem from './nav/menuitem';
import '../styles/nav.scss';

export default function Nav(): React.ReactElement {
  const showMenu = window.platform.getPlatform() !== 'darwin';

  return (
    <div className="titlebar" style={{height: '30px'}}>
      {showMenu && (
        <>
          <div className='titlebar-drag-region'></div>
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
