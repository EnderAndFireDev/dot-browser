import React from 'react';
import { StyledApp, Style } from './style';
import { createGlobalStyle } from 'styled-components';

import { SuggestionBox } from '../SuggestionBox';

import { observer } from 'mobx-react';

import { Menu } from '../Menu';
import dot from '../../store';

const GlobalStyle = createGlobalStyle`${Style}`;

export const App = observer(() => (
  <StyledApp>
    <GlobalStyle />
    <SuggestionBox />
    <Menu store={dot}/>
  </StyledApp>
))