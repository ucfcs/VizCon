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
  state: 'running' | 'ready' | 'waiting' | 'completed';
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
  doStep: () => Promise<any>;
  stop: () => Promise<any>;
}

type VisualizerRunState = 'not_started' | 'starting' | 'pausing' | 'paused' | 'terminating' | 'terminated' | 'running' | 'finished';

type UnsavedChangesResponse = 'save' | 'dontsave' | 'cancel';
