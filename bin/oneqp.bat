if not EXIST rec    md rec
if not EXIST stream md stream

set InputFile=h:\seqs\avs2\%1_%3x%4_%5.yuv

set RecFile=rec\%2_%1_QP%8.yuv

set StrmFile=stream\%2_%1_QP%8.avs

utest_x64.exe -f encoder_ra.cfg -p InputFile=%InputFile% -p OutputFile=%StrmFile% -p ReconFile="" -p SourceWidth=%3 -p SourceHeight=%4 -p FrameRate=%5 -p FramesToBeEncoded=%6 -p IntraPeriod=%7 -p QP=%8