import React, { useEffect, useRef, useState } from 'react';
import '../styles/visualizer.scss';
import Controls from './visualizer/controls';
import ConsoleOutput from './visualizer/output';
import Threads from './visualizer/threads';
import Variables from './visualizer/variables';
import { ThreadData, VisualizerController, VisualizerState } from './visualizer/visualizerController';

interface VisualizerProps {
  inVisualizer: boolean;
  current: OpenFileData;
  goBack: () => void;
}

const testingThreadData: ThreadData[] = [
  {
    state: 'running',
    name: '1 main',
  },
  {
    state: 'ready',
    name: '2 coroutine',
  },
];

const testingGlobalVars = [
  {
    name: 'i',
    type: 'int',
    value: '0',
  },
  {
    name: 'counter',
    type: 'int',
    value: '42',
  },
  {
    name: 'ptr',
    type: 'int *',
    value: '0x76b949c587f3942a',
  },
];

const testingLocalVars = {
  '1 main': [{ name: 'sem', type: 'VCSem*', value: '0x76b949c587f3942a' }],
  '2 corutine': [
    { name: 'sem', type: 'VCSem*', value: '0x76b949c587f3942a' },
    { name: 'counter', type: 'long', value: '3506' },
  ],
};

export default function Visualizer({ inVisualizer, current, goBack }: VisualizerProps): React.ReactElement {
  const [className, setClassName] = useState('');
  const [visualizerState, setVisualizerState] = useState<VisualizerState | null>(null);
  const [runState, setRunState] = useState('not started');
  const visualizerController = useRef(null);
  const [consoleOutput, setConsoleOutput] = useState('');
  const [simulationSpeed, setSimulationSpeed] = useState(100);

  // This is a testing useeffect
  useEffect(() => {
    console.log('some event happened on the visualizer', inVisualizer, current);
  }, [inVisualizer, current]);

  useEffect(() => {
    setClassName(!inVisualizer ? 'hidden' : '');
  }, [inVisualizer]);

  // TODO: actual functions
  function start() {
    console.log('test start');

    setRunState('Starting...');
    visualizerController.current = new VisualizerController({
      // TODO: pass the path to the executable file
      executableFile: 'executable',
      speed: simulationSpeed,
      onVisualizerStateChange: newState => {
        setVisualizerState(newState);
      },
      onVisualizerRunStateChange: newRunState => {
        setRunState(newRunState);
      },
      onConsoleOutput: data => {
        setConsoleOutput(console_out => {
          let new_out = console_out;

          for (const entry of data) {
            new_out += entry;
          }
          return new_out;
        });
      },
    });
    const task = visualizerController.current.start();
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
      <Controls
        fileName={current.path}
        simulationActive={false}
        start={start}
        restart={restart}
        stop={stop}
        goBack={goBack}
        status={runState}
        setSimulationSpeed={simulationSpeed => {
          if (visualizerController.current !== null) {
            visualizerController.current.setSpeed(simulationSpeed);
          }
          setSimulationSpeed(simulationSpeed);
        }}
        simulationSpeed={simulationSpeed}
      />
      <div className="visualizer-main">
        <Threads data={visualizerState?.threads || testingThreadData} />
        <ConsoleOutput current={current} text={consoleOutput} />
        <Variables
          globals={visualizerState?.globals || testingGlobalVars}
          locals={
            {
              /*TODO: locals*/
            } || testingLocalVars
          }
        />
      </div>
    </div>
  );
}
