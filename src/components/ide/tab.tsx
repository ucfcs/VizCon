import React, { useEffect, useState } from 'react';

interface TabProps {
  setActive: () => void;
  name: string;
  current: OpenFileData;
}

export default function Tab({ setActive, name, current }: TabProps): React.ReactElement {
  const [cls, setCls] = useState<string>('tab');
  // TODO: the start simulation button
  // TODO: the close button

  let shortName = name.replace(/\\/g, '/');
  if (shortName.lastIndexOf('/')) {
    shortName = shortName.substring(shortName.lastIndexOf('/') + 1);
  }

  useEffect(() => {
    setCls('tab' + (current?.path === name ? ' active' : ''));
  }, [current]);

  return (
    <div className={cls} onClick={setActive} draggable>
      {/* TODO: Icon goes here */}
      <div className="tab-label">{shortName}</div>
      {/* TODO: the actions area */}
      <div className="tab-actions"></div>
    </div>
  );
}
