import React from 'react';
import MonacoEditor from "react-monaco-editor";

interface EditorProps {
  file: string;
}

export default function Editor({ file }: EditorProps): React.ReactElement {
  return <div id={file} className='editor-container'>
    <MonacoEditor language='c' value={['int main(void) {', '    return 0;', '}', '// ' + file].join('\n')}/>
  </div>;
}
