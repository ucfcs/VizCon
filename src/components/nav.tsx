import React, { useEffect, useState } from 'react';
import { filePathToShortName } from '../util/utils';
import WindowControls from './nav/windowcontrols';
import Menu from './nav/menuitem';
import '../styles/nav.scss';

interface NavProps {
  current: OpenFileData;
  dirty: boolean;
  visualizerActive: boolean;
  landingPath: string;
  openFile: () => void;
  openExampleFile: () => void;
  openUserGuide: () => void;
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
  landingPath,
  openFile,
  openExampleFile,
  openUserGuide,
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
  const [title, setTitle] = useState('');

  useEffect(() => {
    let reactTitle = '';
    if (current.path !== landingPath) {
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

  function isFileOpen() {
    return current.path !== landingPath;
  }

  function vizOrNoFile() {
    return inVisualizer() || (!isFileOpen() && false);
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
            { name: 'Open File', action: openFile, disable: inVisualizer, keybind: 'Ctrl+!Shift+O' },
            { name: 'Open Example File', action: openExampleFile, disable: inVisualizer, keybind: 'Ctrl+Shift+O' },
            { name: "Open User's Guide", action: openUserGuide },
            {
              name: 'b',
              action: () => {
                return;
              },
              seperator: true,
            },
            { name: 'Save File', action: saveFile, disable: vizOrNoFile, keybind: 'Ctrl+!Shift+!Alt+S' },
            { name: 'Save As', action: saveAs, disable: vizOrNoFile, keybind: 'Ctrl+Shift+!Alt+S' },
            { name: 'Save All', action: saveAll, disable: vizOrNoFile, keybind: 'Ctrl+!Shift+Alt+S' },
            {
              name: 'b',
              action: () => {
                return;
              },
              seperator: true,
            },
            { name: 'Close File', action: closeFile, disable: vizOrNoFile, keybind: 'Ctrl+!Shift+W' },
            { name: 'Close Window', action: closeWindow, keybind: 'Ctrl+Shift+W' },
          ]}
        />
        <Menu
          title="Compile"
          options={[
            { name: 'Compile File', action: compile, disable: vizOrNoFile, keybind: 'F5' },
            { name: 'Compile And Run File', action: compileAndRun, disable: vizOrNoFile, keybind: 'F7' },
          ]}
        />
        <Menu
          title="View"
          options={[
            { name: 'Show Compile Output', action: showCompileOutput, disable: inVisualizer },
            { name: 'Show Editor', action: showEditor, disable: inEditor },
            { name: 'Show Visualizer', action: showVisualizer, disable: inVisualizer },
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
