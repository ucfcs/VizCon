import React, { useRef, useState, useEffect } from 'react';

interface MenuItemProps {
  title: string;
  options?: {
    name: string;
    action: () => void;
    break?: boolean;
  }[];
}

// TODO: mouse out instead of blur
export default function MenuItem({ title, options }: MenuItemProps): React.ReactElement {
  const actionRef = useRef<HTMLDivElement>();
  const [expanded, setExpanded] = useState(false);
  const [className, setClassName] = useState('menu-item');
  const [optionElements, setOptionElements] = useState(<></>);

  useEffect(() => {
    const els = options.map((opt, i) => {
      if (opt.break) {
        return <li key={title + '-' + i} className="menu-action disabled"></li>;
      }
      return (
        <li key={title + '-' + i} className="menu-action" onClick={opt.action}>
          <span className="action-label">{opt.name}</span>
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
    console.log("click reset")
    const element = e.target as HTMLElement;

    document.body.removeEventListener('click', globalClickReset);

    // dont run this code if it inside a menu
    if ((element.classList.contains("menu-item") || actionRef.current.contains(element)) && element.dataset.title === title) {
      return;
    }

    reset();
  }

  function onItemClick(e: React.MouseEvent) {
    console.log("ItemClick")
    if (e.button !== 0) {
      // ignore non left click
      return;
    }

    // if already expanded
    if (expanded) {
      reset();
      return;
    }

    console.log("setting expanded")

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
