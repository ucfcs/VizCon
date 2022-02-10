import React, { useEffect, useState } from 'react';
import Icon from '../../imgs/seti-ui/c.svg';
import { filePathToShortName } from '../../util/utils';

interface TabProps {
  setActive: () => void;
  close: () => void;
  name: string;
  dirty: boolean;
  current: OpenFileData;
}

export default function Tab({ setActive, close, name, dirty, current }: TabProps): React.ReactElement {
  const [cls, setCls] = useState<string>('tab');
  const [title, setTitle] = useState(name);
  // TODO: the start simulation button

  const shortName = filePathToShortName(name);

  useEffect(() => {
    setCls('tab' + (current.path === name ? ' active' : '') + (dirty ? ' dirty' : ''));
  }, [current, dirty]);

  useEffect(() => {
    if (dirty) {
      setTitle(name + ' • Modified');
    }
  }, [dirty]);

  return (
    <div className={cls} onClick={setActive} title={title}>
      <div className="tab-label">
        <Icon className="icon" />
        {shortName}
      </div>
      <div className="tab-actions">
        <ul className="tab-actions-container">
          <li className="action-item">
            {/* TODO: tab index tracking */}
            <a className="action-label codicon codicon-close" role="button" title="Close" onClick={close} />
          </li>
        </ul>
      </div>
    </div>
  );
}
