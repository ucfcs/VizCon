import React, { StrictMode } from 'react';
import * as ReactDOM from 'react-dom';
import { MemoryRouter } from 'react-router-dom';
import Nav from './components/nav';

const root = document.getElementById('vizcon');
document.body.classList.add(window.platform.getPlatform());

ReactDOM.render(
  <StrictMode>
    <MemoryRouter>
      <Nav />
    </MemoryRouter>
  </StrictMode>,
  root
);
