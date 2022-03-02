import React, { useEffect, useState } from 'react';
import { getDisplayValueForBool } from '../../util/utils';

interface ThreadData {
  name: string;
  active: boolean;
  complete: boolean;
  inProcessor: boolean;
  // TODO, paramater function name
}

interface ThreadsProps {
  data: ThreadData[];
}

export default function Threads({ data }: ThreadsProps): React.ReactElement {
  const [tableValues, setTableValues] = useState([]);

  useEffect(() => {
    const trs = data.map(thread => {
      const className = 'thread-row' + (thread.inProcessor ? ' current' : '');
      return (
        <tr className={className} key={`Thread: ${thread.name}`} title={`Thread: ${thread.name}`}>
          <td className="thread-name">{thread.name}</td>
          <td className="thread-active">{getDisplayValueForBool(thread.active)}</td>
          <td className="thread-complete">{getDisplayValueForBool(thread.complete)}</td>
        </tr>
      );
    });

    setTableValues(trs);
  }, data);

  return (
    <div className="threads-container">
      <div className="title-container">
        <div className="title">
          <div className="title-border">Threads</div>
        </div>
      </div>
      <table className="thread-table">
        <thead className="thread-head">
          <tr>
            <th className="thread-name">Thread</th>
            <th className="thread-active">Active</th>
            <th className="thread-complete">Complete</th>
          </tr>
        </thead>
        <tbody className="thread-body">{tableValues}</tbody>
      </table>
    </div>
  );
}
