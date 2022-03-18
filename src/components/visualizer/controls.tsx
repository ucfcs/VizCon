import React from 'react';
import { filePathToShortName } from '../../util/utils';

interface ControlsProps {
  fileName: string;
  start: () => void;
  restart: () => void;
  stop: () => void;
  goBack: () => void;
  status: VisualizerRunState;
  simulationSpeed: number;
  setSimulationSpeed: (speed: number) => void;
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
  start,
  restart,
  stop,
  goBack,
  status,
  setSimulationSpeed,
  simulationSpeed,
}: ControlsProps): React.ReactElement {
  return (
    <div className="controls">
      <Control label={'Simulating File: ' + filePathToShortName(fileName)} className="pad-r" />
      {status === 'finished' && (
        <Control label="Restart Simulation" action={{ title: 'Restart Simulation', codiconClass: 'codicon-play', action: restart }} />
      )}
      {(status === 'not_started' || status === 'finished' || status === 'stopped') && (
        <Control label="Start Simulation" action={{ title: 'Start Simulation', codiconClass: 'codicon-play', action: start }} />
      )}
      {status === 'running' && (
        <Control label="Stop Simulation" action={{ title: 'Stop Simulation', codiconClass: 'codicon-debug-stop', action: stop }} />
      )}
      {/*Temporary style hack. The status is not a control and not a label.*/}
      <div className="control">
        <div className="padding-container label">Status: {getStatusDisplayName(status)}</div>
      </div>
      {/*This input is for testing only and should probably be removed*/}
      <input
        type="range"
        min="0"
        max="1000"
        value={simulationSpeed}
        step="20"
        onChange={e => {
          setSimulationSpeed(e.target.valueAsNumber);
        }}
      />
      <Control
        label="Return to Editor"
        action={{ title: 'Return to Editor', codiconClass: 'codicon-discard', action: goBack }}
        className="f-end"
      />
    </div>
  );
}

function getStatusDisplayName(state: VisualizerRunState): string {
  const mapping: { [k in VisualizerRunState]: string } = {
    not_started: 'Not started',
    starting: 'Starting...',
    stopping: 'Stopping...',
    stopped: 'Stopped',
    running: 'Running',
    finished: 'Finished',
  };
  return mapping[state];
}
