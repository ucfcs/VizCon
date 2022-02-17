import React, { useEffect, useRef, useState } from 'react';
import '../styles/visualizer.scss';
import Controls from './visualizer/controls';

interface VisualizerProps {
  inVisualizer: boolean;
  current: OpenFileData;
  goBack: () => void;
}
interface ThreadInfo {
  name: string;
  active: boolean;
}
interface VariableInfo {
  name: string;
  type: string;
  value: string;
}
interface VisualizerState {
  threads: ThreadInfo[];
  globals: VariableInfo[];
}

export default function Visualizer({ inVisualizer, current, goBack }: VisualizerProps): React.ReactElement {
  const [className, setClassName] = useState('');
  const [visualizerState, setVisualizerState] = useState<VisualizerState | null>(null);
  const [runState, setRunState] = useState('not started');
  const visualizerController = useRef(null)
  useEffect(() => {
    console.log('some event happened on the visualizer', inVisualizer, current);
  }, [inVisualizer, current]);

  useEffect(() => {
    setClassName(!inVisualizer ? 'hidden' : '');
  }, [inVisualizer]);

  // TODO: actual functions
  function start() {
    console.log('test start');
    
    let running = true;
    let delayMilliseconds = 100;
    setRunState("starting...");
    const task = async function() {
        await window.platform._temp_launchProgram("path");
        setRunState("started");
        // TODO: improve cancellation
        // possibly refactor this into some sort of controller class
        while (running) {
            const msg = await window.platform._temp_doStep();
            console.log("received visualizer state", msg);
            if (msg.type === 'process_end') {
              setRunState("finished");
              return;
            }
            setVisualizerState(msg);
            await delay(delayMilliseconds);
        }
    }();
    visualizerController.current = {
      stop: () => {running = false;},
      setSpeed: (delay: number) => {delayMilliseconds = delay;}
    };
  }
  function restart() {
    console.log('test restart');
  }
  function stop() {
    console.log('test stop');
    visualizerController.current.stop();
  }

  return (
    <div id="visualizer" className={className}>
      <Controls fileName={current.path} simulationActive={false} start={start} restart={restart} stop={stop} goBack={goBack}/>
      Status: {runState}
      {/*This input is for testing only and should probably be removed*/}
      <input type="range" min="0" max="1000" defaultValue="100" step="20"
        onChange={ (e) => {
          visualizerController.current.setSpeed(e.target.value)
        }}
      />
      <Displays visualizerState={visualizerState}/>
    </div>
  );
}

function Displays({visualizerState} : {visualizerState: VisualizerState} ) {
  if (visualizerState === null)
    return (<span>Loading...</span>);
  return (
    <div>
      Threads:
      <ul>
        {visualizerState.threads.map((thread: any) => {
            return <li key={thread.name}>{thread.active ? "*"+thread.name : thread.name}</li>
        })}
      </ul>
      <table className="globals">
        <caption>Globals</caption>
        <thead>
            <tr>
                <th>Type</th>
                <th>Name</th>
                <th>Value</th>
            </tr>
        </thead>
        <tbody>
        {visualizerState.globals.map((global) => {
            return <tr key={global.name}><td>{global.type}</td><td>{global.name}</td><td>{global.value}</td></tr>
        })}
        </tbody>
      </table>
    </div>
  )
}

function delay(millis: number): Promise<void> {
  return new Promise((resolve, reject) => {
    setTimeout(
      () => { resolve(); },
      millis
    );
  });
}
