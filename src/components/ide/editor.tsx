import React, { useEffect, useRef, useState } from 'react';
import { default as MonacoEditor, Monaco } from '@monaco-editor/react';
import * as monaco from 'monaco-editor';

interface EditorProps {
  file: string;
}

export default function Editor({ file }: EditorProps): React.ReactElement {
  const editorRef = useRef<monaco.editor.IStandaloneCodeEditor>(null);
  const [defaultValue, setDefaultValue] = useState(['int main(void) {', '    return 0;', '}', '// ' + file].join('\n'));

  function onEditorMount(editor: monaco.editor.IStandaloneCodeEditor, monaco: Monaco) {
    editorRef.current = editor;
  }

  useEffect(() => {
    async function run() {
      const fileContent = await window.platform.readFileSync(file);
      if (fileContent?.length == 0 || fileContent.startsWith('ERROR:')) {
        // TODO: open modal here and prevent loading of the entire component
        // This might be better acomplished by changing the call to just read file,
        // and instead making the open modal happen somewhere higher up the FC call stack
        console.error('Bad file or path', file, fileContent);
        return;
      }

      setDefaultValue(fileContent);
    }
    run();
  })

  // TODO: This solution still does not generate a unique monaco instance per file, more investigation needed
  return (
    <div id={file} className="editor-container">
      <MonacoEditor
        defaultLanguage="c"
        defaultValue={defaultValue}
        onMount={onEditorMount}
      />
    </div>
  );
}
