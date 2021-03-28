@echo off

REM Solution environment variables can be specified in this file

REM ***** Visual Leak Detector *****
set VLD_PATH=C:\Program Files (x86)\Visual Leak Detector
set VLD_INCLUDEPATH=%VLD_PATH%\include
set VLD_LIBPATH=%VLD_PATH%\lib\Win32

REM ***** GnuPlot *****
set GNUPLOT_PATH=C:\Program Files\gnuplot

REM Update path to make sure that correct version of GnuPlot will be used
set PATH=%GNUPLOT_PATH%;%PATH%

start splat.sln
