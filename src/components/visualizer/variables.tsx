import React, { useEffect, useState } from 'react';

interface VariableData {
  name: string;
  type: string;
  value: string;
}

interface VariablesProps {
  globals: VariableData[];
  locals: {
    [key: string]: VariableData[];
  };
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
        <tr className={className} key={`Variable: ${variable.name}`} title={`Variable: ${variable.name}`}>
          <td className="variable-name">{variable.name}</td>
          <td className="variable-type">{variable.type}</td>
          <td className="variable-value">{variable.value}</td>
        </tr>
      );
    });

    setTableValues(trs);
  }, vars);


  return (
    <div className='accordion-parent'>
      <div className="title-container sm">
        <div className="title">
          <div className="title-border">{title}</div>
        </div>
      </div>
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
    </div>
  );
}

// TODO: display locals, and titles
export default function Variables({ globals, locals }: VariablesProps): React.ReactElement {
  const [localTabs, setLocalTabs] = useState(<></>);

  useEffect(() => {
    const varTableList: React.ReactElement[] = [];
    // possible overly complex loop to get an array of the elements and give them their title
    for (const key in locals) {
      const varArray = locals[key];
      varTableList.push(<VarTable vars={varArray} title={key}/>)
    }

    setLocalTabs(<>
      {varTableList}
    </>);
  }, [locals]);
  
  return (
    <div className="variables-container">
      <div className="title-container">
        <div className="title">
          <div className="title-border">Variables</div>
        </div>
      </div>
      <VarTable vars={globals} title='Globals' key='GlobalVariables' />
      {localTabs}
    </div>
  );
}
