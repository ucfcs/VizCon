import React from 'react';
import { filePathToShortName } from '../../util/utils';

interface ControlsProps {
  fileName: string;
  start: () => void;
  pause: () => void;
  resume: () => void;
  terminate: () => void;
  goBack: () => void;
  status: VisualizerRunState;
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
  pause,
  resume,
  terminate,
  goBack,
  status,
}: ControlsProps): React.ReactElement {
  return (
    <div className="controls">
      <Control label={'Simulating File: ' + filePathToShortName(fileName)} className="pad-r" />
      {/*Temporary style hack. The status is not a control and not a label.*/}
      <div className="control">
        <div className="padding-container label">Status: {getStatusDisplayName(status)}</div>
      </div>
      {/*(status === 'running' || status === 'paused') && (
        <Control label="Restart Simulation" action={{ title: 'Restart Simulation', codiconClass: 'codicon-play', action: restart }} />
      )*/}
      {(status === 'not_started' || status === 'finished' || status === 'terminated' || status === 'error' || status === 'deadlock') && (
        <Control label="Start Simulation" action={{ title: 'Start Simulation', codiconClass: 'codicon-play', action: start }} />
      )}
      {status === 'running' && (
        <Control label="Pause Simulation" action={{ title: 'Pause Simulation', codiconClass: 'codicon-debug-pause', action: pause }} />
      )}
      {status === 'paused' && (
        <Control
          label="Resume Simulation"
          action={{ title: 'Resume Simulation', codiconClass: 'codicon-debug-continue', action: resume }}
        />
      )}
      {(status === 'running' || status === 'pausing' || status === 'paused') && (
        <Control
          label="Force Quit Simulation"
          action={{ title: 'Force Quit Simulation', codiconClass: 'codicon-debug-stop', action: terminate }}
        />
      )}
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
    terminating: 'Terminating...',
    terminated: 'Terminated',
    running: 'Running',
    error: 'Error',
    deadlock: 'Deadlock',
    pausing: 'Pausing...',
    paused: 'Paused',
    finished: 'Finished',
  };
  return mapping[state];
}
