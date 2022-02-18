import React from 'react';

interface ThreadData {
  name: string;
  active: boolean;
  // TODO, paramater function name
}

interface ThreadsProps {
  data: ThreadData[];
}

export default function Threads({ data }: ThreadsProps): React.ReactElement {
  return (
    <div className="threads-container">
      <div className="title-container">
        <div className="title">
          <div className="title-border">Threads</div>
        </div>
      </div>
      <table className="thread-table">
        <tbody></tbody>
      </table>
    </div>
  );
}
