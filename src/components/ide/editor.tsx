import React, { useRef } from 'react';
import { default as MonacoEditor } from '@monaco-editor/react';
import * as monaco from 'monaco-editor';

export default function Editor({ current, regenTabination, setDirty }: EditorProps): React.ReactElement {
  const editorRef = useRef<monaco.editor.IStandaloneCodeEditor>(null);

  function onEditorMount(editor: monaco.editor.IStandaloneCodeEditor): void {
    editorRef.current = editor;
  }

  function onContentChange(content: string): void {
    current.currentContent = content;
    current.dirty = current.currentContent !== current.fileContent;
    setDirty(current.dirty);
    regenTabination();
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
        loading={<div></div>}
        // TODO: add more content to that loading
      />
    </div>
  );
}
