#!/bin/bash

# /**************************************************************************
#
#  This file is part of the Blackfriars Trading Terminal
#
#  Copyright (C) 2021 Alexander Mishurov
#
#  GNU General Public License Usage
#  This file may be used under the terms of the GNU
#  General Public License version 3. The licenses are as published by
#  the Free Software Foundation and appearing in the file LICENSE.GPL3
#  included in the packaging of this file. Please review the following
#  information to ensure the GNU General Public License requirements will
#  be met: https://www.gnu.org/licenses/gpl-3.0.html.
#
# **************************************************************************/

set -e

if [[ "$OSTYPE" != "msys"* ]] ; then
  echo 'Run the script in Windows Git BASH'
  exit 1
fi

inkscape=inkscape.com
magick=magick.exe

if ! $inkscape --version > /dev/null ; then
  echo 'depends on inkscape'
  echo 'https://inkscape.org'
  echo 'select "add to path" during installation'
  exit 1
fi

if ! $magick -version > /dev/null ; then
  echo 'depends on magick'
  echo 'https://imagemagick.org'
  echo 'select "add to path" during installation'
  exit 1
fi

SIZES='
16
20
24
32
40
48
64
128
256
'

SCRIPT_DIR=$(cd "$(dirname "$0")"; pwd)
SVG=$SCRIPT_DIR/../appicon/blackfriars.svg
#SVG="$1"
SVG_DIR=$(cd "$(dirname "$SVG")"; pwd)
BASE=$(basename "$SVG" | sed 's/\.[^\.]*$//')

ICONSET="${BASE}_iconset"
echo "processing: $SVG"
mkdir -p "$ICONSET"

MAGICK_PARAMS=''
for SIZE in $SIZES; do
  OUT_PNG="$ICONSET"/blackfriars-${SIZE}.png
  $inkscape "$SVG" -w $SIZE -h $SIZE -o "$OUT_PNG"
  MAGICK_PARAMS="$MAGICK_PARAMS $OUT_PNG"
done

$magick convert $MAGICK_PARAMS $BASE.ico
rm -r "$ICONSET"
mv "$BASE.ico" $SVG_DIR/
