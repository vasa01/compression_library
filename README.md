Compression library

Author: Vaclav Bradac

This is a compression library that served to compare the coding algorithms EZW, SPIHT, EBCOT. It is also possible to change the type of transformation between wavelet and contour.

Installation:
Make - in source folder with Makefile
Make run - to example

Example:

./main -i ./images/lena.bmp -o "decode.bmp" -t 0 -e 2 -s 20 -q 1 -l 4


Requirements:
OpenCV 3.1
C ++ 11
Compiler g ++


###############################################################################
# Help  - compression library                                                 #
###############################################################################
# -i  source - set input source image                                         #
# -o  source - set output source image                                        #
# -t  number - set transform type                                             #
#         - 0 is DWT_CDF9/7, 1 is DWT_CDF5/3, 2 is contoulet                  #
# -c  number - set color space type                                           #
#         - 0 is YCBCR, 1 is RGB, 2 is  GRAY                                  #
# -e  number - set encode coef type                                           #
#         - 0 is EZW, 1 is SPIHT, 2 is EBCOT                                  #
# -l  number - set level number in transform                                  #
# -s  number - set number maxsize compresss file in kB                        #
# -p  number - set number maxplanes in encoding coef                          #
# -b  number - set number block size in encoding coef EBCOT (defaul 64)       #
# -q  number - if number == 0 quantization turn off else turn on              #
###############################################################################

