import React from 'react';

interface VariablesProps {
  globals: VariableInfo[];
}

export default function Variables({ globals }: VariablesProps): React.ReactElement {
  return (
    <div className="variables-container">
      <div className="title-container">
        <div className="title">
          <div className="title-border">Variables</div>
        </div>
      </div>
      <table className="variables-globals">
        <caption>Globals:</caption>
        <thead>
          <tr>
            <th>Type</th>
            <th>Name</th>
            <th>Value</th>
          </tr>
        </thead>
        <tbody>
          {globals.map((global) => {
            return (
              <tr key={global.name}>
                <td>{global.type}</td>
                <td>{global.name}</td>
                <td>{global.value}</td>
              </tr>
            );
          })}
        </tbody>
      </table>
    </div>
  );
}
