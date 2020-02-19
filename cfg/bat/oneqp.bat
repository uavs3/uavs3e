if not EXIST rec    md rec
if not EXIST stream md stream

set InputFile=z:\avs2\%1_%3x%4_%5.yuv

set RecFile=rec\%1_%3x%4_QP%8.yuv

set StrmFile=stream\%1_%3x%4_QP%8.avs3

set logfile=log\%1_%3x%4_QP%8.log

.\uavs3e.exe --config encode_RA.cfg -i %InputFile% -o %StrmFile% -r %RecFile% -w %3 -h %4 --fps_num %5 --fps_den 1 -f %6 -q %8 -p %7 --wpp_threads 8 >%logfile%