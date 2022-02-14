import React, { useEffect, useState } from 'react';
import '../styles/visualizer.scss';

interface VisualizerProps {
  inVisualizer: boolean;
  current: OpenFileData;
}

export default function Visualizer({ inVisualizer, current }: VisualizerProps): React.ReactElement {
  const [className, setClassName] = useState('');

  useEffect(() => {
    console.log('some event happened on the visualizer', inVisualizer, current);
  }, [inVisualizer, current]);

  useEffect(() => {
    setClassName(!inVisualizer ? 'hidden' : '');
  }, [inVisualizer]);

  return (
    <div id="visualizer" className={className}>
      <div>Temporary visualizer content</div>
    </div>
  );
}
