
PCX2BMP

---------------------------

Graphic converter for PCEngine developers.

PCX(256 palette) ---> BMP(256 palette)
BMP(256 palette) ---> PCX(256 palette)

Input / output PCX and BMP are 256-color palette images.

---------------------------
1.compilation

(Installded c: MSYS2/MinGW)
set PATH=C:\msys64\mingw32\bin\;C:\msys64\usr\bin\;%PATH%
gcc.exe pcx2bmp.c -opcx2bmp.exe

---------------------------
2.command line

pcxtool InputFileName OutputFileName

The combination of input / output files is fixed.
  (ok)pcxtool input.PCX output.BMP
  (ok)pcxtool input.BMP output.PCX
  (no)pcxtool input.PCX output.PCX
  (no)pcxtool input.BMP output.BMP

Input / output PCX and BMP are 256-color palette images.

---------------------------
3.License
This program is a modification of pcxtool that comes with the HuC compiler.

The license follows HuC.


HuC
https://github.com/dshadoff/huc
https://github.com/uli/huc

DevKit HomePage
http://www.magicengine.com/mkit/
Zeograd's lair(->HuC)
https://www.zeograd.com/index.php

----------------------------------------------------------------
thanks all.

