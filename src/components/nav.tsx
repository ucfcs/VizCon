import React, { useEffect, useState } from 'react';
import { filePathToShortName } from '../util/utils';
import WindowControls from './nav/windowcontrols';
import MenuItem from './nav/menuitem';
import '../styles/nav.scss';

interface NavProps {
  current: OpenFileData;
  dirty: boolean;
  visualizerActive: boolean;
  openFile: () => void;
  openBlankFile: () => void;
  saveFile: () => void;
  saveAll: () => void;
  saveAs: () => void;
  compile: () => void;
  compileAndRun: () => void;
  showCompileOutput: () => void;
  showVisualizer: () => void;
  showEditor: () => void;
  closeFile: () => void;
  closeWindow: () => void;
}

export default function Nav({
  current,
  dirty,
  visualizerActive,
  openFile,
  openBlankFile,
  saveFile,
  saveAll,
  saveAs,
  compile,
  compileAndRun,
  showCompileOutput,
  showVisualizer,
  showEditor,
  closeFile,
  closeWindow,
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
    const titleSuffix = visualizerActive ? 'Visualier - ' : 'Editor - ';
    reactTitle += titleSuffix;

    setTitle(reactTitle);
    document.title = reactTitle + 'VizCon';
  }, [current, dirty, visualizerActive]);

  // TODO: Update this menu to match the options found in main/macMenu.ts
  return (
    <div className="titlebar">
      <div className="titlebar-drag-region"></div>
      <a className="appicon"></a>
      <div className="menubar" role="menubar">
        <MenuItem
          title="File"
          options={[
            { name: 'New File', action: openBlankFile, keybind: 'Ctrl+N' },
            {
              name: 'b',
              action: () => {
                return;
              },
              seperator: true,
            },
            { name: 'Open File', action: openFile, keybind: 'Ctrl+O' },
            {
              name: 'b',
              action: () => {
                return;
              },
              seperator: true,
            },
            { name: 'Save File', action: saveFile, keybind: 'Ctrl+S' },
            { name: 'Save As', action: saveAs, keybind: 'Ctrl+Shift+S' },
            { name: 'Save All', action: saveAll, keybind: 'Ctrl+Alt+S' },
            {
              name: 'b',
              action: () => {
                return;
              },
              seperator: true,
            },
            { name: 'Close File', action: closeFile, keybind: 'Ctrl+W' },
            { name: 'Close Window', action: closeWindow, keybind: 'Ctrl+Shift+W' },
          ]}
        />
        <MenuItem
          title="Compile"
          options={[
            {
              name: 'Compile File',
              action: compile,
            },
            {
              name: 'Compile And Run File',
              action: compileAndRun,
            },
          ]}
        />
        <MenuItem
          title="View"
          options={[
            {
              name: 'Show Compile Output',
              action: showCompileOutput,
            },
            {
              // TODO: disabling
              name: 'Show Editor',
              action: showEditor,
            },
            {
              name: 'Show Visualizer',
              action: showVisualizer,
            },
            {
              name: 'b',
              action: () => {
                return;
              },
              seperator: true,
            },
            {
              name: 'Zoom In',
              action: () => {
                window.platform.zoomIn();
              },
              keybind: 'Ctrl++',
            },
            {
              name: 'Zoom Out',
              action: () => {
                window.platform.zoomOut();
              },
              keybind: 'Ctrl+-',
            },
            {
              name: 'Reset Zoom',
              action: () => {
                window.platform.resetZoom();
              },
              keybind: 'Ctrl+0',
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
