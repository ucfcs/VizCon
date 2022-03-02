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
