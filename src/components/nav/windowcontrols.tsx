import React, { useState, useEffect } from 'react';

export default function WindowControls(): React.ReactElement {
  const [maximized, setMaximized] = useState<boolean>(false);

  window.addEventListener('resize', checkMaximized);

  async function checkMaximized() {
    setMaximized(await window.platform.isMaximized());
  }

  useEffect(() => {
    checkMaximized();
  });

  return (
    <div className="window-controls">
      <div className="window-icon codicon codicon-chrome-minimize" onClick={window.platform.minimize}></div>
      {maximized && (
        <>
          <div
            className="window-icon codicon codicon-chrome-restore"
            onClick={() => {
              window.platform.restore();
              setMaximized(false);
            }}
          ></div>
        </>
      )}
      {!maximized && (
        <>
          <div
            className="window-icon codicon codicon-chrome-maximize"
            onClick={() => {
              window.platform.maximize();
              setMaximized(true);
            }}
          ></div>
        </>
      )}
      <div className="window-icon codicon codicon-chrome-close" onClick={window.platform.close}></div>
    </div>
  );
}
