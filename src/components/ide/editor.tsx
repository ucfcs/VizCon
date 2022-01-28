import React, { useRef } from 'react';
import { default as MonacoEditor, Monaco } from '@monaco-editor/react';
import * as monaco from 'monaco-editor';

export default function Editor({ current }: EditorProps): React.ReactElement {
  const editorRef = useRef<monaco.editor.IStandaloneCodeEditor>(null);

  function onEditorMount(editor: monaco.editor.IStandaloneCodeEditor, monaco: Monaco) {
    editorRef.current = editor;
  }

  return (
    <div className="editor-container">
      <MonacoEditor
        theme="vs-dark"
        defaultLanguage="c"
        defaultValue={current.content}
        path={current.path}
        onMount={onEditorMount}
      />
    </div>
  );
}
