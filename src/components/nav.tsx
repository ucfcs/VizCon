import React from 'react';
import WindowControls from './nav/windowcontrols';
import MenuItem from './nav/menuitem';
import '../styles/nav.scss';

interface NavProps {
  openFile: () => void; // TODO: this will be used later
}

export default function Nav({ openFile }: NavProps): React.ReactElement {
  const showMenu = window.platform.getPlatform() !== 'darwin';

  return (
    <div className="titlebar" style={{ height: '30px' }}>
      {showMenu && (
        <>
          <div className="titlebar-drag-region"></div>
          <a className="appicon"></a>
          <div className="menubar" role="menubar">
            <MenuItem title="File" options={[{ name: 'Open File', action: openFile }]} />
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
      {/* Also TODO: the visibility of this on macOS needs to be reevaluated */}
      <div className="window-title">Temporary Title</div>
      {showMenu && (
        <>
          <WindowControls />
        </>
      )}
    </div>
  );
}
