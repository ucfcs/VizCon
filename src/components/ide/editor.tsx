import React from 'react';
import {default as MonacoEditor} from "@monaco-editor/react";

interface EditorProps {
  file: string;
}

export default function Editor({ file }: EditorProps): React.ReactElement {
  return <div id={file} className='editor-container'>
    <MonacoEditor language='c' value={['int main(void) {', '    return 0;', '}', '// ' + file].join('\n')} />
  </div>;
}
