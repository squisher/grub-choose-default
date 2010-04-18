#!/bin/sh

if [ ! -x "`which inkscape`" ]; then
  echo "Could not find inkscape"
  exit 1
fi

if [ ! -x "`which convert`" ]; then
  echo "Could not find convert"
  exit 1
fi

bitmaps=""

for d in *x*; do
  w=`echo $d | cut -dx -f1`
  h=`echo $d | cut -dx -f2`

  echo "Exporting $d..."
  bitmap=$d/grub-choose-default.png
  inkscape -z -f scalable/grub-choose-default.svg -e $bitmap -w $w -h $h

  bitmaps="$bitmaps $bitmap"
done

echo "Creating Windows icon"
convert $bitmaps grub-choose-default.ico
