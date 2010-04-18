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

  echo 
  bitmap=$d/grub-choose-default.png
  inkscape -z -f scalable/grub-choose-default.svg -e $bitmap -w $w -h $h

  bitmaps="$bitmaps $bitmap"
done

echo
echo "Creating Windows icon"
convert $bitmaps ../win32/grub-choose-default.ico

echo
echo "Creating installer logo"
logo=grub-choose-default-logo
inkscape -z -f scalable/grub-choose-default.svg -e $logo.png -w 64 -h 64
convert $logo.png ../win32/$logo.bmp && rm $logo.png
