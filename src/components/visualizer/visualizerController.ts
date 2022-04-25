import { delay } from '../../util/utils';

interface VisualizerControllerOptions {
  exeFile: string;
  speed: number;
  onConsoleOutput: (out: string[]) => void;
  onStateChange: (newState: VisualizerState) => void;
  onRunStateChange: (newRunState: VisualizerRunState) => void;
}

export default class VisualizerController {
  private status: VisualizerRunState = 'not_started';
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

  terminate(): void {
    // TODO: improve cancellation
    this.status = 'terminating';
    console.log('Terminating visualizer...');
    this.onRunStateChange('terminating');
    this.debuggerHandle.stop().then(res => {
      this.status = 'terminated';
      this.onRunStateChange('terminated');
      console.log('Visualizer terminated', res);
    });
  }

  pause(): void {
    this.status = 'pausing';
    this.onRunStateChange('pausing');
  }
  resume(): void {
    if (this.status !== 'paused') {
      console.error('The visualizer can only be resumed of it is in the paused state.');
      return;
    }
    this.startLoop();
  }

  setSpeed(delayMilliseconds: number): void {
    this.delayMilliseconds = delayMilliseconds;
  }

  private async startAsync() {
    const handle = await window.platform.launchProgram(this.exeFile, data => {
      //console.log("Console stdout output", data);
      this.onConsoleOutput([data]);
    });
    if ('err' in handle) {
      this.status = 'error';
      this.onRunStateChange('error');
      this.onConsoleOutput(['Error starting application. Perhaps you forgot to compile it?\n']);
      return;
    }
    this.debuggerHandle = handle;
    await this.startLoop();
  }
  private async startLoop() {
    this.status = 'running';
    this.onRunStateChange('running');
    while (this.status === 'running') {
      const msg = await this.debuggerHandle.doStep();
      //console.log('received visualizer state', msg);
      if (msg.type === 'process_end') {
        this.status = 'finished';
        this.onRunStateChange('finished');
        return;
      }
      if (msg.type === 'process_killed') {
        console.log('Process was killed.');
        return;
      }
      if (msg.type === 'error') {
        if (msg.error === 'deadlock') {
          this.status = 'deadlock';
          this.onRunStateChange('deadlock');
          this.onConsoleOutput(['All threads are waiting. Deadlock?\n']);
        } else {
          this.status = 'error';
          this.onRunStateChange('error');
          this.onConsoleOutput([`Error: ${msg.error}`]);
        }
        //TODO: this.onStateChange(msg);
        return;
      }
      this.onStateChange(msg);
      await delay(this.delayMilliseconds);
    }
    if (this.status === 'pausing') {
      this.status = 'paused';
      this.onRunStateChange('paused');
    }
  }
}
