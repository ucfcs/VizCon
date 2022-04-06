import React, { useRef, useState, useEffect } from 'react';

interface MenuProps {
  title: string;
  options?: MenuItemOpts[];
}

interface MenuItemProps extends MenuItemOpts {
  parentTitle: string;
}

interface MenuItemOpts {
  name: string;
  action: () => void;
  disable?: () => boolean;
  keybind?: string;
  seperator?: boolean;
}

interface KeyboardMasks {
  ctrl: boolean;
  shift: boolean;
  alt: boolean;
}

function getMasksForBind(keybind: string): [KeyboardMasks, string] {
  let ctrl = false;
  let shift = false;
  let alt = false;

  if (keybind.includes('Ctrl+')) {
    keybind = keybind.replace('Ctrl+', '');
    ctrl = true;
  }

  if (keybind.includes('Shift+')) {
    keybind = keybind.replace('Shift+', '');
    shift = true;
  }

  if (keybind.includes('Alt+')) {
    keybind = keybind.replace('Alt+', '');
    alt = true;
  }

  return [
    {
      ctrl: ctrl,
      shift: shift,
      alt: alt,
    },
    keybind,
  ];
}

function getDisallowedMasksForBind(keybind: string): [KeyboardMasks, string] {
  let ctrl = false;
  let shift = false;
  let alt = false;

  if (keybind.includes('!Ctrl+')) {
    keybind = keybind.replace('!Ctrl+', '');
    ctrl = true;
  }

  if (keybind.includes('!Shift+')) {
    keybind = keybind.replace('!Shift+', '');
    shift = true;
  }

  if (keybind.includes('!Alt+')) {
    keybind = keybind.replace('!Alt+', '');
    alt = true;
  }

  return [
    {
      ctrl: ctrl,
      shift: shift,
      alt: alt,
    },
    keybind,
  ];
}

const callbackCache: { [key: string]: () => void } = {};
const keyboardCache: { [key: string]: (ke: KeyboardEvent) => void } = {};

function MenuItem({ parentTitle, name, action, disable, keybind, seperator }: MenuItemProps): React.ReactElement {
  const [disabled, setDisabled] = useState(false);

  useEffect(() => {
    // check if disable func exists and its value
    if (disable && disable()) {
      setDisabled(true);
      window.platform.disableMenu(parentTitle, name, true);
      return;
    }
    setDisabled(false);
    window.platform.disableMenu(parentTitle, name, false);
  });

  if (seperator) {
    return (
      <li className="menu-action disabled">
        <span className="action-label seperator disabled"></span>
      </li>
    );
  }

  const onClick = () => {
    if (disabled) {
      return;
    }
    action();
  };

  // add the event for macOS
  // TODO: remove this for non macOS platforms??
  window.removeEventListener(`Nav-${parentTitle}-${name}`, callbackCache[name]);
  callbackCache[name] = () => {
    onClick();
  };
  window.addEventListener(`Nav-${parentTitle}-${name}`, callbackCache[name]);

  let displayKeybind = keybind;

  // handle keybinds
  if (keybind) {
    window.removeEventListener('keydown', keyboardCache[name]);

    // get the disallowed masks, then the allowed masks, in that order
    const [disallowedMasks, partialKeybind] = getDisallowedMasksForBind(keybind);
    displayKeybind = partialKeybind;
    const [masks, key] = getMasksForBind(partialKeybind);

    keyboardCache[name] = (ke: KeyboardEvent) => {
      // if ctrl key required but not pressed, return
      if (masks.ctrl && !ke.ctrlKey) {
        return;
      }

      if (masks.shift && !ke.shiftKey) {
        return;
      }

      if (masks.alt && !ke.altKey) {
        return;
      }

      // check if any of the disallowed masks are present and pressed
      if (disallowedMasks.ctrl && ke.ctrlKey) {
        return;
      }
      
      if (disallowedMasks.shift && ke.shiftKey) {
        return;
      }

      if (disallowedMasks.alt && ke.altKey) {
        return;
      }

      // check that key matches
      // additional check to see if the keybind is +, allowing the = to take the place of the + so the user does not need to hold shift
      if (ke.key.toUpperCase() === key || (key === '+' && ke.key.toUpperCase() === '=')) {
        ke.preventDefault();
        onClick();
      }
    };

    window.addEventListener('keydown', keyboardCache[name]);
  }

  const className = 'menu-action' + (disabled ? ' disabled' : '');

  return (
    <li className={className} onClick={onClick}>
      <span className="action-label">{name}</span>
      {displayKeybind && <span className="keybind">{displayKeybind}</span>}
    </li>
  );
}

// TODO: mouse out instead of blur
// TODO: this gets rerendered every time a new file is opened, a solution must be found
export default function Menu({ title, options }: MenuProps): React.ReactElement {
  const actionRef = useRef<HTMLDivElement>();
  const [expanded, setExpanded] = useState(false);
  const [className, setClassName] = useState('menu-item');
  const [optionElements, setOptionElements] = useState(<></>);

  useEffect(() => {
    const els = options.map((opt, i) => {
      return (
        <MenuItem
          key={title + '-' + i}
          parentTitle={title}
          name={opt.name}
          action={opt.action}
          disable={opt.disable}
          keybind={opt.keybind}
          seperator={opt.seperator}
        />
      );
    });
    setOptionElements(<ul>{els}</ul>);
  }, [options]);

  function reset() {
    setClassName('menu-item');
    actionRef.current.classList.remove('visible');
    setExpanded(false);
  }

  function globalClickReset(e: MouseEvent) {
    const element = e.target as HTMLElement;

    document.body.removeEventListener('click', globalClickReset);

    // dont run this code if it inside a menu
    // TODO: this doesnt always work properly, when clicking on a disabled part of the menu it still closes
    if (
      (element.classList.contains('menu-item') || element.classList.contains('disabled') || actionRef.current.contains(element)) &&
      element.dataset.title === title
    ) {
      return;
    }

    reset();
  }

  function onItemClick(e: React.MouseEvent) {
    if (e.button !== 0) {
      // ignore non left click
      return;
    }

    // if already expanded
    if (expanded) {
      reset();
      return;
    }

    setExpanded(true);
    setClassName('menu-item open');

    const element = e.target as HTMLDivElement;
    const box = element.getBoundingClientRect();
    actionRef.current.style.left = box.left.toString() + 'px';
    actionRef.current.classList.add('visible');
    actionRef.current.focus();

    document.getElementById('vizcon').addEventListener('click', globalClickReset);
  }

  return (
    <div className={className} onClick={onItemClick} data-title={title}>
      {title}
      <div className="menu-actions" ref={actionRef} onBlur={reset}>
        <div className="menu-container">{optionElements}</div>
      </div>
    </div>
  );
}
