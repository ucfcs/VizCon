import React from 'react';

interface EditorProps {
  file: string;
}

export default function Editor({ file }: EditorProps): React.ReactElement {
  return <div>{file}</div>;
}
