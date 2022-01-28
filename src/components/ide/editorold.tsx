// this file is currently just a save of the old way of dong the editor component, for if it is ever needed again
// DONT USE IT
import React, { useEffect, useRef, useState } from 'react';
import * as monaco from 'monaco-editor';

interface EditorProps {
  file: string;
}

export default function EditorOld({ file }: EditorProps): React.ReactElement {
  const div = useRef<HTMLDivElement>(null);
  // TODO: better default content
  const [fileContent, setFileContent] = useState<string>(
    ['int main(void) {', '    return 0;', '}', '// ' + file].join('\n')
  );
  const [monacoInstance, setMonacoInstance] = useState<monaco.editor.IStandaloneCodeEditor>(null);

  // this use effect is pureley here for the initial mount of the component
  // this *will* need to be done better, because there is no way to dispose the monaco editor
  // to do that, we *might* need to do some hacky call backs and what not to propigate the dispose call
  // to the tab closing
  useEffect(() => {
    console.log(monacoInstance);
    if (div.current && !monacoInstance) {
      const editor = monaco.editor.create(div.current, {
        value: fileContent,
        language: 'c',
        automaticLayout: true,
        theme: 'vs-dark',
      });
      setMonacoInstance(editor);
    }
  }, []);

  useEffect(() => {
    run();
    async function run() {
      // only run if monaco has been loaded
      if (!monacoInstance) return;
      const fileContent = await window.platform.readFilesSync([file]);
      console.log(fileContent);
      if (fileContent?.length == 0 || fileContent[0].startsWith('ERROR:')) {
        // TODO: open modal here and prevent loading of the entire component
        // This might be better acomplished by changing the call to just read file,
        // and instead making the open modal happen somewhere higher up the FC call stack
        // Did that, it is just a read call now
        console.error('Bad file or path', file, fileContent);
        return;
      }
      setFileContent(fileContent[0]);
      monacoInstance.setValue(fileContent[0]);
    }
  }, [file, monacoInstance]);

  return <div id={file} className="editor-container" ref={div}></div>;
}
