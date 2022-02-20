import React from 'react';

interface VariableData {
  name: string;
  active: boolean;
  // TODO, paramater function name
}

interface VariablesProps {
  data: VariableData[];
}

export default function Variables({ data }: VariablesProps): React.ReactElement {
  return (
    <div className="variables-container">
      <div className="title-container">
        <div className="title">
          <div className="title-border">Variables</div>
        </div>
      </div>
      <table className="variables-table">
        <tbody></tbody>
      </table>
    </div>
  );
}
