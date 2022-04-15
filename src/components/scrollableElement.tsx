import React, {  } from 'react';
import '../styles/component/scroll.scss';

interface ScrollableElementProps {
  content: React.ReactElement;
}

export default function ScrollableElement({ content }: ScrollableElementProps): React.ReactElement {
  return <div className="scroll-container">{content}</div>;
}
