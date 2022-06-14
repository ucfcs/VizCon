import React from 'react';

interface LandingProps {
  newFile: () => void;
  openFile: () => void;
  openExampleFile: () => void;
  openUserGuide: () => void;
}

export default function Landing({ newFile, openFile, openExampleFile, openUserGuide }: LandingProps): React.ReactElement {
  return (
    <div className="landing">
      <h2>Welcome to VizCon!</h2>
      <h3>
        <a onClick={openFile}>Open a file</a> to keep working.
      </h3>
      <h3>
        Or <a onClick={newFile}>create a new file</a> to get started.
      </h3>
      <br />
      <h4>
        To see an example, <a onClick={openExampleFile}>open an example file</a>.
      </h4>
      <h4>
        Or <a onClick={openUserGuide}>view the user's guide</a>
      </h4>

      {/* TODO: dispay recent files */}
    </div>
  );
}
