import React from 'react';

interface LandingProps {
  openFile: () => void;
  newFile: () => void;
}

export default function Landing({ newFile, openFile }: LandingProps): React.ReactElement {
  return (
    <div className="landing">
      <h2>Welcome to VizCon!</h2>
      <h3>
        <a onClick={openFile}>Open a file</a> to keep working.
      </h3>
      <h3>
        Or <a onClick={newFile}>create a new file</a> to get started.
      </h3>
    </div>
  );
}
