import React from 'react';
import { filePathToShortName } from '../../util/utils';

interface ControlsProps {
  fileName: string;
  simulationActive: boolean;
  start: () => void;
  restart: () => void;
  stop: () => void;
  goBack: () => void;
}

interface ControlProps {
  label: string;
  action?: {
    title: string;
    codiconClass: string;
    action: () => void;
  };
  className?: string;
}

// TODO: restructure so entire element is clickable
function Control({ label, action, className = '' }: ControlProps): React.ReactElement {
  const labelElem = <div className="label">{label}</div>;

  if (action) {
    className = (className + ' has-action').trim();
  }

  return (
    <div className={`control ${className}`.trim()}>
      {action && (
        <a className="action-button padding-container" role="button" title={action.title} onClick={action.action}>
          {/* TODO: tab index tracking */}
          {labelElem}
          <div className="actions">
            <ul className="actions-container">
              <li className="action-item" key={`Control: ${label} ${action.title}`}>
                <span className={`action-label codicon ${action.codiconClass}`} />
              </li>
            </ul>
          </div>
        </a>
      )}
      {!action && <div className="padding-container">{labelElem}</div>}
    </div>
  );
}

export default function Controls({
  fileName,
  simulationActive,
  start,
  restart,
  stop,
  goBack,
}: ControlsProps): React.ReactElement {
  return (
    <div className="controls">
      <Control label={'Simulating File: ' + filePathToShortName(fileName)} className="pad-r" />
      {simulationActive && (
        <Control
          label="Restart Simulation"
          action={{ title: 'Restart Simulation', codiconClass: 'codicon-play', action: restart }}
        />
      )}
      {!simulationActive && (
        <Control
          label="Start Simulation"
          action={{ title: 'Start Simulation', codiconClass: 'codicon-play', action: start }}
        />
      )}
      <Control
        label="Stop Simulation"
        action={{ title: 'Stop Simulation', codiconClass: 'codicon-debug-stop', action: stop }}
      />
      <Control
        label="Return to Editor"
        action={{ title: 'Return to Editor', codiconClass: 'codicon-discard', action: goBack }}
        className="f-end"
      />
    </div>
  );
}
