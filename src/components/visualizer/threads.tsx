import React from 'react';

interface ThreadsProps {
  threads: ThreadInfo[];
}

export default function Threads({ threads }: ThreadsProps): React.ReactElement {
  return (
    <div className="threads-container">
      <div className="title-container">
        <div className="title">
          <div className="title-border">Threads</div>
        </div>
      </div>
      <table className="threads-table">
        <caption>Threads:</caption>
        <thead>
          <tr>
            <th>Thread ID</th>
            <th>State</th>
          </tr>
        </thead>
        <tbody>
          {threads.map((thread) => {
            return (
              <tr key={thread.name}>
                <td>{thread.name}</td>
                <td>{thread.state}</td>
              </tr>
            );
          })}
        </tbody>
      </table>
    </div>
  );
}
