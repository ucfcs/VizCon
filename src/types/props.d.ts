interface OpenFileData {
  path: string;
  fileContent: string;
  currentContent: string;
  dirty: boolean;
}

interface EditorProps {
  current: OpenFileData;
  regenTabination: () => void;
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
