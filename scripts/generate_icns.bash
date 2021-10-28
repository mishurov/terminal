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

if [[ "$OSTYPE" != "darwin"* ]] ; then
  echo 'Run the script in MacOS'
  exit 1
fi

if [ ! -e /usr/bin/iconutil ] ; then
  echo '/usr/bin/iconutil required'
  exit 1
fi
if [ ! -e /usr/bin/sips ] ; then
  echo '/usr/bin/sips required'
  exit 1
fi

inkscape=/usr/local/bin/inkscape

SIZES='
16,16x16
32,16x16@2x
64,32x32@2x
128,128x128
256,128x128@2x
512,256x256@2x
1024,512x512@2x
'

#SVG="$1"
SCRIPT_DIR=$(cd "$(dirname "$0")"; pwd)
SVG=$SCRIPT_DIR/../appicon/blackfriars.svg
SVG_DIR=$(cd "$(dirname "$SVG")"; pwd)

BASE=$(basename "$SVG" | sed 's/\.[^\.]*$//')
ICONSET="$BASE.iconset"
echo "processing: $SVG"
mkdir -p "$ICONSET"
if [ -e "$inkscape" ]; then
  $inkscape "$SVG" -w 1024 -h 1024 -o "$ICONSET"/icon_512x512@2x.png
else
  echo 'depends on inkscape'
  echo 'brew install inkscape'
  exit 1
fi
for PARAMS in $SIZES; do
  SIZE=$(echo $PARAMS | cut -d, -f1)
  LABEL=$(echo $PARAMS | cut -d, -f2)
  sips -z $SIZE $SIZE "$ICONSET"/icon_512x512@2x.png --out "$ICONSET"/icon_$LABEL.png
done
cp "$ICONSET/icon_16x16@2x.png" "$ICONSET/icon_32x32.png"
cp "$ICONSET/icon_128x128@2x.png" "$ICONSET/icon_256x256.png"
cp "$ICONSET/icon_256x256@2x.png" "$ICONSET/icon_512x512.png"
iconutil -c icns "$ICONSET"
rm -rf "$ICONSET"
mv "$BASE.icns" $SVG_DIR/
