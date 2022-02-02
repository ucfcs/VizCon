import React, { useRef } from 'react';
import { default as MonacoEditor } from '@monaco-editor/react';
import * as monaco from 'monaco-editor';

export default function Editor({ current }: EditorProps): React.ReactElement {
  const editorRef = useRef<monaco.editor.IStandaloneCodeEditor>(null);

  function onEditorMount(editor: monaco.editor.IStandaloneCodeEditor): void {
    editorRef.current = editor;
  }

  function onContentChange(content: string, event: monaco.editor.IModelContentChangedEvent): void {
    current.currentContent = content;
    // for debugging, remove this later
    console.log(event)
  }

  return (
    <div className="editor-container">
      <MonacoEditor
        theme="vs-dark"
        defaultLanguage="c"
        defaultValue={current.fileContent}
        path={current.path}
        onMount={onEditorMount}
        onChange={onContentChange}
      />
    </div>
  );
}
