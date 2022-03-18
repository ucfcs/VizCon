import { delay } from '../../util/utils';

interface VisualizerControllerOptions {
  exeFile: string;
  speed: number;
  onConsoleOutput: (out: string[]) => void;
  onStateChange: (newState: VisualizerState) => void;
  onRunStateChange: (newRunState: VisualizerRunState) => void;
}

export default class VisualizerController {
  private running = false;
  private readonly exeFile: string;
  private delayMilliseconds: number;
  private readonly onConsoleOutput: (out: string[]) => void;
  private readonly onStateChange: (newState: VisualizerState) => void;
  private readonly onRunStateChange: (newRunState: VisualizerRunState) => void;
  private debuggerHandle: DebuggerHandle;

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
    console.log('Stopping visualizer...');
    this.onRunStateChange('stopping');
    this.debuggerHandle.stop().then(res => {
      this.onRunStateChange('stopped');
      console.log('Visualizer stopped', res);
    });
  }

  setSpeed(delayMilliseconds: number): void {
    this.delayMilliseconds = delayMilliseconds;
  }

  private async startAsync() {
    this.debuggerHandle = await window.platform.launchProgram(this.exeFile, data => {
      //console.log("Console stdout output", data);
      this.onConsoleOutput([data]);
    });
    this.onRunStateChange('running');
    this.running = true;
    while (this.running) {
      const msg = await this.debuggerHandle.doStep();
      //console.log('received visualizer state', msg);
      if (msg.type === 'process_end') {
        this.running = false;
        this.onRunStateChange('finished');
        return;
      }
      if (msg.type === 'process_killed') {
        this.running = false;
        console.log('Process was killed.');
        return;
      }
      this.onStateChange(msg);
      await delay(this.delayMilliseconds);
    }
  }
}
