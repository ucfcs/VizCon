import React, { useEffect, useState } from 'react';

interface TabProps {
  setActive: () => void;
  name: string;
  current: OpenFileData;
}

export default function Tab({ setActive, name, current }: TabProps): React.ReactElement {
  const [cls, setCls] = useState<string>('tab');
  // TODO: fancy name reduction so it is only the file name and not the whole path
  // TODO: the start simulation button

  // TODO RESUME: fix the tab problem
  useEffect(() => {
    setCls('tab' + (current?.path === name ? ' active' : ''));
  }, [current]);

  return (
    <div className={cls} onClick={setActive} draggable>
      {/* TODO: Icon goes here */}
      <div className="tab-label">{name}</div>
      {/* TODO: the actions area */}
      <div className="tab-actions"></div>
    </div>
  );
}
