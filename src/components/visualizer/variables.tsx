import React, { useEffect, useState } from 'react';

interface VariablesProps {
  globals: VariableData[];
  locals: {
    [key: string]: VariableData[];
  };
}

interface VarTableProps {
  title?: string;
  vars: VariableData[];
}

// TODO: folding
function VarTable({ vars, title }: VarTableProps): React.ReactElement {
  const [tableValues, setTableValues] = useState([]);

  useEffect(() => {
    const trs = vars.map(variable => {
      const className = 'variable-row';
      return (
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
    <table className="variable-table">
      <thead className='variable-head'>
        <tr>
          <th className='variable-name'>Name</th>
          <th className='variable-type'>Type</th>
          <th className='variable-value'>Value</th>
        </tr>
      </thead>
      <tbody className='variable-body'>{tableValues}</tbody>
    </table>
  );
}

// TODO: display locals, and titles
export default function Variables({ globals, locals }: VariablesProps): React.ReactElement {
  return (
    <div className="variables-container">
      <div className="title-container">
        <div className="title">
          <div className="title-border">Variables</div>
        </div>
      </div>
      <VarTable vars={globals} />
    </div>
  );
}
