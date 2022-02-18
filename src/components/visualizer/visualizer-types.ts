interface ThreadInfo {
  name: string;
  state: string;
}

interface VariableInfo {
  name: string;
  type: string;
  value: string;
}

interface VisualizerState {
  threads: ThreadInfo[];
  globals: VariableInfo[];
}
