import React, { useEffect, useState } from 'react';
import { filePathToShortName } from '../util/utils';
import WindowControls from './nav/windowcontrols';
import Menu from './nav/menuitem';
import '../styles/nav.scss';

interface NavProps {
  current: OpenFileData;
  dirty: boolean;
  visualizerActive: boolean;
  openFile: () => void;
  openExampleFile: () => void;
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
  openExampleFile,
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
    const titleSuffix = visualizerActive ? 'Visualizer - ' : 'Editor - ';
    reactTitle += titleSuffix;

    setTitle(reactTitle);
    document.title = reactTitle + 'VizCon';
  }, [current, dirty, visualizerActive]);

  function inVisualizer() {
    return visualizerActive;
  }

  function inEditor() {
    return !visualizerActive;
  }

  // TODO: Update this menu to match the options found in main/macMenu.ts
  return (
    <div className="titlebar">
      <div className="titlebar-drag-region"></div>
      <a className="appicon"></a>
      <div className="menubar" role="menubar">
        <Menu
          title="File"
          options={[
            { name: 'New File', action: openBlankFile, disable: inVisualizer, keybind: 'Ctrl+N' },
            {
              name: 'b',
              action: () => {
                return;
              },
              seperator: true,
            },
            { name: 'Open File', action: openFile, disable: inVisualizer, keybind: 'Ctrl+O' },
            { name: 'Open Example File', action: openExampleFile, disable: inVisualizer, keybind: 'Ctrl+Shift+O' },
            {
              name: 'b',
              action: () => {
                return;
              },
              seperator: true,
            },
            { name: 'Save File', action: saveFile, disable: inVisualizer, keybind: 'Ctrl+S' },
            { name: 'Save As', action: saveAs, disable: inVisualizer, keybind: 'Ctrl+Shift+S' },
            { name: 'Save All', action: saveAll, disable: inVisualizer, keybind: 'Ctrl+Alt+S' },
            {
              name: 'b',
              action: () => {
                return;
              },
              seperator: true,
            },
            { name: 'Close File', action: closeFile, disable: inVisualizer, keybind: 'Ctrl+W' },
            { name: 'Close Window', action: closeWindow, keybind: 'Ctrl+Shift+W' },
          ]}
        />
        <Menu
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
        <Menu
          title="View"
          options={[
            {
              name: 'Show Compile Output',
              disable: inVisualizer,
              action: showCompileOutput,
            },
            {
              // TODO: disabling
              name: 'Show Editor',
              disable: inEditor,
              action: showEditor,
            },
            {
              name: 'Show Visualizer',
              disable: inVisualizer,
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
