import React, { useEffect, useState } from 'react';
import { filePathToShortName } from '../util/utils';
import WindowControls from './nav/windowcontrols';
import MenuItem from './nav/menuitem';
import '../styles/nav.scss';

interface NavProps {
  openFile: () => void;
  openBlankFile: () => void;
  saveFile: () => void;
  saveAll: () => void;
  saveAs: () => void;
  current: OpenFileData;
  visualizerActive: boolean;
  compile: () => void;
  showCompileOutput: () => void;
  showVisualizer: () => void;
  showEditor: () => void;
}

export default function Nav({
  openFile,
  openBlankFile,
  saveFile,
  saveAll,
  saveAs,
  current,
  visualizerActive,
  compile,
  showCompileOutput,
  showVisualizer,
  showEditor,
}: NavProps): React.ReactElement {
  const showMenu = window.platform.getPlatform() !== 'darwin';
  // const showMenu = true;

  const [title, setTitle] = useState('');

  useEffect(() => {
    let reactTitle = '';
    if (current.path !== 'tracking://Landing') {
      const shortName = filePathToShortName(current.path);
      const titlePrefix = current.dirty ? '• ' : '';
      reactTitle = titlePrefix + shortName + ' - ';
    }
    const titleSuffix = visualizerActive ? 'Visualizer - ' : 'Editor - ';
    reactTitle += titleSuffix;

    setTitle(reactTitle);
    document.title = reactTitle + 'VizCon';
  }, [current, current.dirty, visualizerActive]);

  // TODO: Update this menu to match the options found in main/macMenu.ts
  return (
    <div className="titlebar">
      <div className="titlebar-drag-region"></div>
      <a className="appicon"></a>
      <div className="menubar" role="menubar">
        <MenuItem
          title="File"
          options={[
            { name: 'New File', action: openBlankFile },
            { name: 'Open File', action: openFile },
            { name: 'Save File', action: saveFile },
            { name: 'Save As', action: saveAs },
            { name: 'Save All', action: saveAll },
          ]}
        />
        <MenuItem
          title="Compile"
          options={[
            {
              name: 'Compile File',
              action: compile,
            },
          ]}
        />
        <MenuItem
          title="View"
          options={[
            {
              name: 'Zoom In',
              action: () => {
                console.log('zoom in');
              },
            },
            {
              name: 'Zoom Out',
              action: () => {
                console.log('zoom out');
              },
            },
            {
              name: 'Show Compile Output',
              action: showCompileOutput,
            },
            {
              name: 'Show Visualizer',
              action: showVisualizer,
            },
            {
              name: 'Show Editor',
              action: showEditor,
            },
          ]}
        />
      </div>
      <div className="window-title">{title}VizCon</div>
      {showMenu && (
        <>
          <WindowControls />
        </>
      )}
    </div>
  );
}
