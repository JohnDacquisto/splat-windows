echo off

REM Solution environment variables can be specified in this file

REM ***** Visual Leak Detector *****
REM set VLD_PATH=C:\Program Files (x86)\Visual Leak Detector
REM set VLD_INCLUDEPATH=%VLD_PATH%\include
REM set VLD_LIBPATH=%VLD_PATH%\lib\Win32

REM Update path to make sure that correct version of Oracle Client DLLs will be used
REM set PATH=%ORACLE_BINPATH%;%PATH%

start splat.sln