import React, { useEffect, useRef } from 'react';
import { default as MonacoEditor } from '@monaco-editor/react';
import * as monaco from 'monaco-editor';

interface ConsoleOutputProps {
  text: string;
  current: OpenFileData;
}

export default function ConsoleOutput({ text, current }: ConsoleOutputProps): React.ReactElement {
  const editorRef = useRef<monaco.editor.IStandaloneCodeEditor>(null);

  function onEditorMount(editor: monaco.editor.IStandaloneCodeEditor): void {
    editorRef.current = editor;
  }

  useEffect(() => {
    editorRef?.current?.setValue(text);
  }, [text]);

  return (
    <div className="visualizer-console">
      <div className="title-container">
        <div className="title">
          <div className="title-border">Compilation Output</div>
        </div>
      </div>
      <div className="output-container">
        <MonacoEditor
          theme="vs-dark"
          defaultLanguage="txt"
          defaultValue={text}
          path={current.path + ' Stdout'}
          onMount={onEditorMount}
          loading={<div></div>}
          // TODO: add more content to that loading
          options={{
            minimap: { enabled: false },
            readOnly: true,
            lineNumbers: 'off',
            renderLineHighlight: 'none',
            scrollBeyondLastLine: false,
            detectIndentation: false,
            tabSize: 100000,
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
