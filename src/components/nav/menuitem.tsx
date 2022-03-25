import React, { useRef, useState, useEffect } from 'react';

interface MenuItemProps {
  title: string;
  options?: {
    name: string;
    action: () => void;
    keybind?: string;
    seperator?: boolean;
  }[];
}

interface KeyboardMasks {
  ctrl: boolean;
  shift: boolean;
  alt: boolean;
}

function getMasksForBind(keybind: string): [KeyboardMasks, string] {
  let ctrl: boolean;
  let shift: boolean;
  let alt: boolean;

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

  return [{
    ctrl: ctrl,
    shift: shift,
    alt: alt
  }, keybind];
}

const callbackCache: { [key: string]: () => void } = {};
const keyboardCache: { [key: string]: (ke: KeyboardEvent) => void } = {};

// TODO: mouse out instead of blur
// TODO: this gets rerendered every time a new file is opened, a solution must be found
export default function MenuItem({ title, options }: MenuItemProps): React.ReactElement {
  const actionRef = useRef<HTMLDivElement>();
  const [expanded, setExpanded] = useState(false);
  const [className, setClassName] = useState('menu-item');
  const [optionElements, setOptionElements] = useState(<></>);

  useEffect(() => {
    const els = options.map((opt, i) => {
      if (opt.seperator) {
        return (
          <li key={title + '-' + i} className="menu-action disabled">
            <span className="action-label seperator disabled"></span>
          </li>
        );
      }

      window.removeEventListener(`Nav-${title}-${opt.name}`, callbackCache[opt.name]);

      callbackCache[opt.name] = () => {
        opt.action();
      };

      window.addEventListener(`Nav-${title}-${opt.name}`, callbackCache[opt.name]);

      if (opt.keybind) {
        window.removeEventListener('keydown', keyboardCache[opt.name]);

        const [masks, key] = getMasksForBind(opt.keybind);

        keyboardCache[opt.name] = (ke: KeyboardEvent) => {
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

          // check that key matches
          // additional check to see if the keybind is +, allowing the = to take the place of the + so the user does not need to hold shift
          if (ke.key.toUpperCase() === key || (key === '+' && ke.key.toUpperCase() === '=')) {
            ke.preventDefault();
            opt.action();
          }
        };

        window.addEventListener('keydown', keyboardCache[opt.name]);
      }

      return (
        <li key={title + '-' + i} className="menu-action" onClick={opt.action}>
          <span className="action-label">{opt.name}</span>
          <span className='keybind'>{opt.keybind}</span>
        </li>
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
