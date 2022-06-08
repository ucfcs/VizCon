import { delay } from '../../util/utils';

interface VisualizerControllerOptions {
  exeFile: string;
  speed: number;
  onConsoleOutput: (out: string[]) => void;
  onStateChange: (newState: VisualizerState) => void;
  onRunStateChange: (newRunState: VisualizerRunState) => void;
}

const vcErrorMap: VC_ErrorMap = {
  VC_ERROR_DOUBLELOCK: -12,
  VC_ERROR_CROSSTHREADUNLOCK: -13,
  VC_ERROR_SEMVALUELIMIT: -14,
  VC_ERROR_DEADLOCK: 15,
};

function assertUnreachable(e: never): never {
  throw new Error('Unhandled case detected (assertUnreachable)');
}
function getErrorMsg(state: VC_ErrorCode): string {
  if (state === vcErrorMap.VC_ERROR_DEADLOCK) return 'All threads are waiting. Deadlock?';
  if (state === vcErrorMap.VC_ERROR_CROSSTHREADUNLOCK) return 'A thread attempted to unlock an mutex that was locked by another thread.';
  if (state === vcErrorMap.VC_ERROR_SEMVALUELIMIT) return 'A thread attempted to signal a semaphore that was already at its maximum value.';
  if (state === vcErrorMap.VC_ERROR_DOUBLELOCK) return 'A thread attempted to lock a mutex that it already locked.';
  assertUnreachable(state);
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
        this.onConsoleOutput([`Program finished. Exit code: ${msg.code}\n`]);
        return;
      }
      if (msg.type === 'process_killed') {
        console.log('Process was killed.');
        return;
      }
      if (msg.type === 'vc_error') {
        this.status = 'error';
        this.onRunStateChange('error');
        this.onConsoleOutput([`Program crashed. Error: ${getErrorMsg(msg.errCode)}\n`]);
        //TODO: this.onStateChange(msg);
        return;
      }
      if (msg.type === 'controller_error') {
        this.status = 'error';
        this.onRunStateChange('error');
        this.onConsoleOutput([`${msg.error}\n`]);
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
