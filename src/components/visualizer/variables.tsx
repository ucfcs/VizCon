import React, { useEffect, useState } from 'react';

interface VariablesProps {
  globals: VariableData[];
  threads: ThreadData[];
}

interface VarTableProps {
  title: string;
  vars: VariableData[];
}

// TODO: folding
function VarTable({ vars, title }: VarTableProps): React.ReactElement {
  const [tableValues, setTableValues] = useState([]);

  useEffect(() => {
    const trs = vars.map(variable => {
      const className = 'variable-row';
      return (
        // TODO: give more detailed information in the title?
        <tr className={className} key={`Variable: ${variable.name}`} title={`Variable: ${variable.name}`}>
          <td className="variable-name">{variable.name}</td>
          <td className="variable-type">{variable.type}</td>
          <td className="variable-value">{variable.value}</td>
        </tr>
      );
    });

    setTableValues(trs);
  }, [vars]);

  return (
    <div className="accordion-parent">
      <div className="title-container sm">
        <div className="title">
          <div className="title-border">{title}</div>
        </div>
      </div>
      <table className="variable-table">
        <thead className="variable-head">
          <tr>
            <th className="variable-name">Name</th>
            <th className="variable-type">Type</th>
            <th className="variable-value">Value</th>
          </tr>
        </thead>
        <tbody className="variable-body">{tableValues}</tbody>
      </table>
    </div>
  );
}

export default function Variables({ globals, threads }: VariablesProps): React.ReactElement {
  const [localTabs, setLocalTabs] = useState(<></>);

  useEffect(() => {
    const varTableList: React.ReactElement[] = threads.map(thread => {
      return <VarTable vars={thread.locals || []} title={thread.name} />;
    });
    setLocalTabs(<>{varTableList}</>);
  }, [threads]);

  return (
    <div className="variables-container">
      <div className="title-container">
        <div className="title">
          <div className="title-border">Variables</div>
        </div>
      </div>
      <VarTable vars={globals} title="Globals" key="GlobalVariables" />
      {localTabs}
    </div>
  );
}
