import React, { useEffect, useRef, useState } from 'react';
import '../styles/visualizer.scss';
import Controls from './visualizer/controls';
import ConsoleOutput from './visualizer/output';
import Threads from './visualizer/threads';
import Variables from './visualizer/variables';

interface VisualizerProps {
  inVisualizer: boolean;
  current: OpenFileData;
  goBack: () => void;
}


export default function Visualizer({ inVisualizer, current, goBack }: VisualizerProps): React.ReactElement {
  const [className, setClassName] = useState('');
  const [visualizerState, setVisualizerState] = useState<VisualizerState | null>(null);
  const [runState, setRunState] = useState('not started');
  const visualizerController = useRef(null)
  const [consoleOutput, setConsoleOutput] = useState('');
  const [simulationSpeed, setSimulationSpeed] = useState(100);

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
    let delayMilliseconds = simulationSpeed;
    setRunState("Starting...");
    const task = async function() {
        await window.platform._temp_launchProgram("path");
        setRunState("Started");
        // TODO: improve cancellation
        // possibly refactor this into some sort of controller class
        while (running) {
            const msg = await window.platform._temp_doStep();
            console.log("received visualizer state", msg);
            if (msg.type === 'process_end') {
              setRunState("Finished");
              return;
            }
            setConsoleOutput((console_out) => {
              let new_out = console_out;
              // Not actually lines
              for (const printed_line of msg.printed_lines) {
                new_out += printed_line;
              }
              return new_out;
            });
            
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
      <Controls fileName={current.path} simulationActive={false} start={start} restart={restart} stop={stop} goBack={goBack}
        status={runState}
        setSimulationSpeed={(simulationSpeed) => {
          if (visualizerController.current !== null) {
            visualizerController.current.setSpeed(simulationSpeed);
          }
          setSimulationSpeed(simulationSpeed);
        }}
        simulationSpeed={simulationSpeed}
      />
      <div className='visualizer-main'>
        <Threads data={visualizerState?.threads || []}/>
        <ConsoleOutput current={current} text={consoleOutput} />
        <Variables globals={visualizerState?.globals || []} locals={{'1 main': [
          {name: 'sem',
          type: 'VCSem*',
          value: '0x76b949c587f3942a'}
        ]}}/>
      </div>
    </div>
  );
}

function delay(millis: number): Promise<void> {
  return new Promise((resolve, reject) => {
    setTimeout(
      () => { resolve(); },
      millis
    );
  });
}
