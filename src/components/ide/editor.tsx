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
      // TODO: file reading
      // TODO: allow unique monaco instances (this doesnt work right now)
      const editor = monaco.editor.create(div.current, {
        value: ['int main(void) {', '    return 0;', '}'].join('\n'),
        language: 'c',
        automaticLayout: true
      });
      setMonacoInstance(editor);
    }

    return () => {
      monacoInstance.dispose();
    }
  }, []);

  return <div className='editor' ref={div}></div>;
}
