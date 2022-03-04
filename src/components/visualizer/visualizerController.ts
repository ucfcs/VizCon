import { delay } from '../../util/utils';

export default class VisualizerController {
  private running = false;
  private readonly executableFile: string;
  private delayMilliseconds: number;
  private readonly onConsoleOutput: (out: string[]) => void;
  private readonly onVisualizerStateChange: (newState: VisualizerState) => void;
  private readonly onVisualizerRunStateChange: (newRunState: string) => void;
  constructor({
    executableFile,
    speed,
    onConsoleOutput,
    onVisualizerStateChange,
    onVisualizerRunStateChange,
  }: {
    executableFile: string;
    speed: number;
    onConsoleOutput: (out: string[]) => void;
    onVisualizerStateChange: (newState: VisualizerState) => void;
    onVisualizerRunStateChange: (newRunState: string) => void;
  }) {
    this.executableFile = executableFile;
    this.delayMilliseconds = speed;
    this.onConsoleOutput = onConsoleOutput;
    this.onVisualizerStateChange = onVisualizerStateChange;
    this.onVisualizerRunStateChange = onVisualizerRunStateChange;
  }
  start() {
    this._startLoop();
  }
  stop() {
    // TODO: improve cancellation
    this.running = false;
  }
  setSpeed(delayMilliseconds: number) {
    this.delayMilliseconds = delayMilliseconds;
  }
  private async _startLoop() {
    const debuggerHandle = await window.platform._temp_launchProgram(this.executableFile, data => {
      //console.log("Console stdout output", data);
      this.onConsoleOutput([data]);
    });
    this.onVisualizerRunStateChange('Running');
    this.running = true;
    while (this.running) {
      const msg = await debuggerHandle.doStep();
      //console.log('received visualizer state', msg);
      if (msg.type === 'process_end') {
        this.onVisualizerRunStateChange('Finished');
        return;
      }
      this.onVisualizerStateChange(msg);
      await delay(this.delayMilliseconds);
    }
  }
}
