import React, { useEffect, useRef, useState } from 'react';
import '../styles/visualizer.scss';
import Controls from './visualizer/controls';
import ConsoleOutput from './visualizer/output';
import Threads from './visualizer/threads';
import Variables from './visualizer/variables';
import VisualizerController from './visualizer/visualizerController';

interface VisualizerProps {
  inVisualizer: boolean;
  current: OpenFileData;
  goBack: () => void;
}

export default function Visualizer({ inVisualizer, current, goBack }: VisualizerProps): React.ReactElement {
  const [className, setClassName] = useState('');
  const [visualizerState, setVisualizerState] = useState<VisualizerState | null>(null);
  const [runState, setRunState] = useState<VisualizerRunState>('not_started');
  const visualizerController = useRef<VisualizerController>(null);
  const [consoleOutput, setConsoleOutput] = useState('');
  const [simulationSpeed, setSimulationSpeed] = useState(10);

  // This is a testing useeffect
  useEffect(() => {
    console.log('some event happened on the visualizer', inVisualizer, current);
  }, [inVisualizer, current]);

  useEffect(() => {
    setClassName(!inVisualizer ? 'hidden' : '');
  }, [inVisualizer]);

  function start() {
    setConsoleOutput('');
    setRunState('starting');
    visualizerController.current = new VisualizerController({
      exeFile: current.path,
      speed: simulationSpeed,
      onStateChange: newState => {
        setVisualizerState(newState);
      },
      onRunStateChange: newRunState => {
        setRunState(newRunState);
      },
      onConsoleOutput: data => {
        setConsoleOutput(consoleOut => {
          let newOut = consoleOut;

          for (const entry of data) {
            newOut += entry;
          }
          return newOut;
        });
      },
    });
    visualizerController.current.start();
  }

  function terminate() {
    visualizerController.current.terminate();
  }

  function pause() {
    visualizerController.current.pause();
  }

  function resume() {
    visualizerController.current.resume();
  }

  return (
    <div id="visualizer" className={className}>
      <Controls
        fileName={current.path}
        start={start}
        terminate={terminate}
        pause={pause}
        resume={resume}
        goBack={goBack}
        status={runState}
      />
      <div className="visualizer-main">
        <Threads data={visualizerState?.threads || []} />
        <ConsoleOutput current={current} text={consoleOutput} />
        <Variables globals={visualizerState?.globals || []} threads={visualizerState?.threads || []} />
      </div>
    </div>
  );
}
