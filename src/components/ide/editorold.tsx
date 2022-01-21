// this file is currently just a save of the old way of dong the editor component, for if it is ever needed again
// DONT USE IT
import React, { useEffect, useRef, useState } from 'react';
import * as monaco from 'monaco-editor';

interface EditorProps {
  file: string;
}

export default function Editor({ file }: EditorProps): React.ReactElement {
  const div = useRef<HTMLDivElement>(null);
  const [monacoInstance, setMonacoInstance] = useState<monaco.editor.IStandaloneCodeEditor>(null);

  useEffect(() => {
    if (div.current) {
      div.current.id="file"+file;
      // TODO: file reading
      // TODO: allow unique monaco instances (this doesnt work right now)
      console.log(file)
      const editor = monaco.editor.create(div.current, {
        value: ['int main(void) {', '    return 0;', '}', '// ' + file].join('\n'),
        language: 'c',
        automaticLayout: true
      });
      setMonacoInstance(editor);
    }

    return () => {
      monacoInstance.dispose();
    }
  }, []);

  return <div id={file} className='editor-container'>
    <div ref={div}></div>
  </div>;
}
