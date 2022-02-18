import React, { useEffect, useState } from 'react';
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
  const [consoleOutput, setConsoleOutput] = useState('');

  useEffect(() => {
    console.log('some event happened on the visualizer', inVisualizer, current);
  }, [inVisualizer, current]);

  useEffect(() => {
    setClassName(!inVisualizer ? 'hidden' : '');
  }, [inVisualizer]);

  // TODO: actual functions
  function start() {
    console.log('test start');
  }
  function restart() {
    console.log('test restart');
  }
  function stop() {
    console.log('test stop');
  }

  return (
    <div id="visualizer" className={className}>
      <Controls fileName={current.path} simulationActive={false} start={start} restart={restart} stop={stop} goBack={goBack}/>
      <div className='visualizer-main'>
        <Threads data={[]}/>
        <ConsoleOutput current={current} text={consoleOutput} />
        <Variables data={[]}/>
      </div>
    </div>
  );
}
