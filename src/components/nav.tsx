import React from 'react';
import WindowControls from './nav/windowcontrols';
import '../styles/nav.scss';

export default function Nav(): React.ReactElement {
  const showMenu = window.platform.getPlatform() !== 'darwin';

  return (
    <div className="titlebar">
      {showMenu && (
        <>
          <a className="appicon"></a>
          <div className="menubar" role="menubar"></div>
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
