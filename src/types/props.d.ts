interface OpenFileData {
  path: string;
  fileContent: string;
  currentContent: string;
  dirty: boolean;
}

interface EditorProps {
  current: OpenFileData;
  regenTabination: () => void;
  setDirty: (_: boolean) => void;
}

interface ThreadData {
  name: string;
  state: 'running' | 'ready' | 'waiting (semaphore)' | 'waiting (mutex)' | 'waiting (vcJoin)' | 'sleeping' | 'completed';
  locals: VariableData[];
}

interface VariableData {
  name: string;
  type: string;
  value: string;
}

interface VisualizerState {
  threads: ThreadData[];
  globals: VariableData[];
}

interface DebuggerHandle {
  doStep: () => Promise<DebuggerResponse>;
  stop: () => Promise<any>;
}

type DebuggerResponse =
  | { type: 'controller_error'; error: string }
  | { type: 'vc_error'; errCode: VC_ErrorCode }
  | ({ type: 'res' } & VisualizerState)
  | { type: 'process_killed' }
  | { type: 'process_end'; code: number };

type VC_ErrorMap = {
  VC_ERROR_DOUBLELOCK: -12;
  VC_ERROR_CROSSTHREADUNLOCK: -13;
  VC_ERROR_SEMVALUELIMIT: -14;
  VC_ERROR_DEADLOCK: 15;
};
type VC_ErrorCode = VC_ErrorMap[keyof VC_ErrorMap];

interface DebuggerError {
  err: string;
}

type VisualizerRunState =
  | 'not_started'
  | 'starting'
  | 'error'
  | 'pausing'
  | 'paused'
  | 'terminating'
  | 'terminated'
  | 'running'
  | 'finished';

type UnsavedChangesResponse = 'save' | 'dontsave' | 'cancel';
