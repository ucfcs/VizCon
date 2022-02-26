import React, { useEffect, useRef } from 'react';
import { default as MonacoEditor } from '@monaco-editor/react';
import * as monaco from 'monaco-editor';

interface ConsoleProps {
  text: string;
  current: OpenFileData;
  closePanel: () => void;
}

export default function Output({ text, current, closePanel }: ConsoleProps): React.ReactElement {
  const editorRef = useRef<monaco.editor.IStandaloneCodeEditor>(null);

  function onEditorMount(editor: monaco.editor.IStandaloneCodeEditor): void {
    editorRef.current = editor;
  }

  useEffect(() => {
    editorRef?.current?.setValue(text);
  }, [text]);

  return (
    <div className="editor-console">
      <div className="title-container">
        <div className="title">
          <div className="title-border">Compilation Output</div>
        </div>
        <div className="tab-actions">
          <ul className="tab-actions-container">
            <li className="action-item">
              <a className="action-label codicon codicon-close" role="button" title="Close Panel" onClick={closePanel} />
            </li>
          </ul>
        </div>
      </div>
      <div className="output-container">
        <MonacoEditor
          theme="vs-dark"
          defaultLanguage="txt"
          defaultValue={text}
          path={current.path + ' Output'}
          onMount={onEditorMount}
          loading={<div></div>}
          // TODO: add more content to that loading
          options={{
            minimap: { enabled: false },
            readOnly: true,
            lineNumbers: 'off',
            renderLineHighlight: 'none',
            scrollBeyondLastLine: false,
            folding: false,
            guides: {
              indentation: false,
              bracketPairs: false,
              bracketPairsHorizontal: false,
              highlightActiveBracketPair: false,
              highlightActiveIndentation: false,
            },
          }}
        />
      </div>
    </div>
  );
}
