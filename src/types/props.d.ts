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
