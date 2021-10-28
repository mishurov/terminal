/****************************************************************************
**
** This file is part of the Blackfriars Trading Terminal
**
** Copyright (C) 2021 Alexander Mishurov
**
** GNU General Public License Usage
** This file may be used under the terms of the GNU
** General Public License version 3. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/


const handleHeight = 5;
const animationDuration = 300;
const handlePressed = '#656565';
const handleOver = '#555';
const handleOut = '#514f5c';
const gridColor = '#514f5c';
const gridLineColor = '#3a3a3a';
const gridLabelsColor = '#999';
const crossHairSize = 6;

const axisCopyProps = [
    'color',
    'gridLineColor',
    'labelsColor',
];

const intervalToString = interval => {
  let intervalName;

  if (interval == 1)
    intervalName = `${interval} minute`;
  else if (interval < 60)
    intervalName = `${interval} minutes`;
  else if (interval < 1440)
    intervalName = 'hour';
  else if (interval < 10080)
    intervalName = 'day';
  else if (interval < 99999)
    intervalName = 'week';
  else
    intervalName = 'month';

  return intervalName;
}

const toFakeUTC = d => new Date(d.toUTCString().slice(0, -4));

const maxNumCandles = 10000;
