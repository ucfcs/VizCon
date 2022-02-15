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
  actions?: {
    title: string;
    codiconClass: string;
    action: () => void;
  }[];
  className?: string;
}

// TODO: restructure so entire element is clickable
function Control({ label, actions = [], className = '' }: ControlProps): React.ReactElement {
  const actionLIs = actions.map(action => {
    return (
      <li className="action-item" key={`Control: ${label} ${action.title}`}>
        {/* TODO: tab index tracking */}
        <a
          className={`action-label codicon ${action.codiconClass}`}
          role="button"
          title={action.title}
          onClick={action.action}
        />
      </li>
    );
  });

  return (
    <div className={`control ${className}`}>
      <div className="label">{label}</div>
      {actions.length > 0 && (
        <div className="actions">
          <ul className="actions-container">{actionLIs}</ul>
        </div>
      )}
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
      <Control label={'Simulating File: ' + filePathToShortName(fileName)} className="pad-r"/>
      {simulationActive && (
        <Control
          label="Restart Simulation"
          actions={[{ title: 'Restart Simulation', codiconClass: 'codicon-play', action: restart }]}
        />
      )}
      {!simulationActive && (
        <Control
          label="Start Simulation"
          actions={[{ title: 'Start Simulation', codiconClass: 'codicon-play', action: start }]}
        />
      )}
      <Control
        label="Stop Simulation"
        actions={[{ title: 'Stop Simulation', codiconClass: 'codicon-debug-stop', action: stop }]}
      />
      <Control
        label="Return to Editor"
        actions={[{ title: 'Return to Editor', codiconClass: 'codicon-discard', action: goBack }]}
        className="f-end"
      />
    </div>
  );
}
