// this file is currently just a save of the old way of dong the editor component, for if it is ever needed again
// DONT USE IT
import React, { useEffect, useRef, useState } from 'react';
import * as monaco from 'monaco-editor';

interface EditorProps {
  file: string;
}

export default function EditorOld({ file }: EditorProps): React.ReactElement {
  const div = useRef<HTMLDivElement>(null);
  const [monacoInstance, setMonacoInstance] = useState<monaco.editor.IStandaloneCodeEditor>(null);

  useEffect(() => {
    async function run() {
      console.log(file);
      if (div.current) {
        let fileContent = await window.platform.openFileSync(file);
        console.log(fileContent);
        if (fileContent.length == 0 || fileContent.startsWith('ERROR:')) {
          fileContent = ['int main(void) {', '    return 0;', '}', '// ' + file].join('\n');
        }

        // TODO: allow unique monaco instances (this doesnt work right now)
        const editor = monaco.editor.create(div.current, {
          value: fileContent,
          language: 'c',
          automaticLayout: true,
          theme: 'vs-dark',
        });
        setMonacoInstance(editor);
      }
    }
    run();

    return () => {
      monacoInstance.dispose();
    };
  }, []);

  return <div id={file} className="editor-container" ref={div}></div>;
}
