interface OpenFileData {
  path: string;
  fileContent: string;
  currentContent: string;
}

interface EditorProps {
  current: OpenFileData;
}
