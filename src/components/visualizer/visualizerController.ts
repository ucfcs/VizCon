import { delay } from '../../util/utils';

interface VisualizerControllerOptions {
  exeFile: string;
  speed: number;
  onConsoleOutput: (out: string[]) => void;
  onStateChange: (newState: VisualizerState) => void;
  onRunStateChange: (newRunState: string) => void;
}

export default class VisualizerController {
  private running = false;
  private readonly exeFile: string;
  private delayMilliseconds: number;
  private readonly onConsoleOutput: (out: string[]) => void;
  private readonly onStateChange: (newState: VisualizerState) => void;
  private readonly onRunStateChange: (newRunState: string) => void;

  constructor({ exeFile, speed, onConsoleOutput, onStateChange, onRunStateChange }: VisualizerControllerOptions) {
    this.exeFile = exeFile;
    this.delayMilliseconds = speed;
    this.onConsoleOutput = onConsoleOutput;
    this.onStateChange = onStateChange;
    this.onRunStateChange = onRunStateChange;
  }

  start(): void {
    this.startAsync();
  }

  stop(): void {
    // TODO: improve cancellation
    this.running = false;
  }

  setSpeed(delayMilliseconds: number): void {
    this.delayMilliseconds = delayMilliseconds;
  }

  private async startAsync() {
    const debuggerHandle = await window.platform.launchProgram(this.exeFile, data => {
      //console.log("Console stdout output", data);
      this.onConsoleOutput([data]);
    });
    this.onRunStateChange('Running');
    this.running = true;
    while (this.running) {
      const msg = await debuggerHandle.doStep();
      //console.log('received visualizer state', msg);
      if (msg.type === 'process_end') {
        this.onRunStateChange('Finished');
        return;
      }
      this.onStateChange(msg);
      await delay(this.delayMilliseconds);
    }
    console.log('Stopping visualizer...');
    this.onRunStateChange('Manually stopping...');
    const res = await debuggerHandle.stop();
    this.onRunStateChange('Stopped.');
    console.log('Visualizer stopped', res);
  }
}
