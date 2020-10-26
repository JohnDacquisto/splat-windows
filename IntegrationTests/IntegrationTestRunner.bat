@echo off
REM - IntegrationTestRunner
REM - To run integration tests, build SPLAT x64 Release, and unzip all regression files in IntegrationTests/TestXX/Results/ folders.
REM - Also, for test 12, unzip test_12_input in the IntegrationTests/Test12 folder.
REM - After running, check TestResults file, to see if any tests resulted in different files to investigate further.


REM - Testing setup.

cd %IntegrationTests%..\x64\Release\

REM - Set to a different number than 1 to skip that specific test.
set /a RunTest01=1
set /a RunTest02=1
set /a RunTest03=1
set /a RunTest04=1
set /a RunTest05=1
set /a RunTest06=1
set /a RunTest07=1
set /a RunTest08=1
set /a RunTest09=1
set /a RunTest10=1
set /a RunTest11=1
set /a RunTest12=1
set /a RunTest13=1
set /a RunTest14=1
set /a RunTest15=1
set /a RunTest16=1

set IntegrationTests=%~dp0

set TestResults=IntegrationTestResults.txt

if exist %IntegrationTests%%TestResults% del %IntegrationTests%%TestResults%

echo IntegrationTestRunner.
echo.
echo To run the integration tests, click any button. To not run the tests, exit out of the command prompt.
echo.
pause


REM - Test 01.

if /i "%RunTest01%" EQU "1" (

	if exist %IntegrationTests%Test01\map.ppm del %IntegrationTests%Test01\map.ppm
	if exist %IntegrationTests%Test01\test_1_trace.txt del %IntegrationTests%Test01\test_1_trace.txt
	if exist %IntegrationTests%Test01\WNJU-DT-site_report.txt del %IntegrationTests%Test01\WNJU-DT-site_report.txt
	if exist %IntegrationTests%Test01\test_1_time.txt del %IntegrationTests%Test01\test_1_time.txt

	splat.exe -t "%IntegrationTests%Test01\wnju-dt.qth" -L 20 -o "%IntegrationTests%Test01\map.ppm" -d "%IntegrationTests%Test01\DEM\\" -rsp "%IntegrationTests%Test01\\" -ano "%IntegrationTests%Test01\test_1_trace.txt" -time "%IntegrationTests%Test01\test_1_time.txt" -metric -hd 0 -mxpg 4

	echo *** Test 01 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 01.1 - map.ppm >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test01\map.ppm" "%IntegrationTests%Test01\Results\map.ppm" >> %IntegrationTests%%TestResults%

	echo 01.2 - test_1_trace.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test01\test_1_trace.txt" "%IntegrationTests%Test01\Results\test_1_trace.txt" >> %IntegrationTests%%TestResults%

	echo 01.3 - WNJU-DT-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test01\WNJU-DT-site_report.txt" "%IntegrationTests%Test01\Results\WNJU-DT-site_report.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 02.

if /i "%RunTest02%" EQU "1" (

	if exist %IntegrationTests%Test02\map.geo del %IntegrationTests%Test02\map.geo
	if exist %IntegrationTests%Test02\map.ppm del %IntegrationTests%Test02\map.ppm
	if exist %IntegrationTests%Test02\test_2_trace.txt del %IntegrationTests%Test02\test_2_trace.txt
	if exist %IntegrationTests%Test02\WNJU-DT_-site_report.txt del %IntegrationTests%Test02\WNJU-DT_-site_report.txt
	if exist %IntegrationTests%Test02\test_2_time.txt del %IntegrationTests%Test02\test_2_time.txt

	splat.exe -t "%IntegrationTests%Test02\wnju-dt.qth" -L 40 -gc 10 -R 50 -o "%IntegrationTests%Test02\map.ppm" -d "%IntegrationTests%Test02\DEM\\" -rsp "%IntegrationTests%Test02\\" -ano "%IntegrationTests%Test02\test_2_trace.txt" -time "%IntegrationTests%Test02\test_2_time.txt" -geo -erp -dbm -ngs -hd 0 -mxpg 9

	echo *** Test 02 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 02.1 - map.geo >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test02\map.geo" "%IntegrationTests%Test02\Results\map.geo" >> %IntegrationTests%%TestResults%

	echo 02.2 - map.ppm >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test02\map.ppm" "%IntegrationTests%Test02\Results\map.ppm" >> %IntegrationTests%%TestResults%

	echo 02.3 - test_2_trace.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test02\test_2_trace.txt" "%IntegrationTests%Test02\Results\test_2_trace.txt" >> %IntegrationTests%%TestResults%

	echo 02.4 - WNJU-DT-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test02\WNJU-DT_-site_report.txt" "%IntegrationTests%Test02\Results\WNJU-DT_-site_report.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 03.

if /i "%RunTest03%" EQU "1" (

	if exist %IntegrationTests%Test03\map.kml del %IntegrationTests%Test03\map.kml
	if exist %IntegrationTests%Test03\map.ppm del %IntegrationTests%Test03\map.ppm
	if exist %IntegrationTests%Test03\map-ck.ppm del %IntegrationTests%Test03\map-ck.ppm
	if exist %IntegrationTests%Test03\test_3_trace.txt del %IntegrationTests%Test03\test_3_trace.txt
	if exist %IntegrationTests%Test03\WNDC-DT_-site_report.txt del %IntegrationTests%Test03\WNDC-DT_-site_report.txt
	if exist %IntegrationTests%Test03\test_3_time.txt del %IntegrationTests%Test03\test_3_time.txt

	splat.exe -t "%IntegrationTests%Test03\wndc-dt.qth" -L 10 -gc 3 -o "%IntegrationTests%Test03\map.ppm" -d "%IntegrationTests%Test03\DEM\\" -rsp "%IntegrationTests%Test03\\" -s "%IntegrationTests%Test03\dc_sites.dat" "%IntegrationTests%Test03\md_sites.dat" "%IntegrationTests%Test03\va_sites.dat" -ano "%IntegrationTests%Test03\test_3_trace.txt" -time "%IntegrationTests%Test03\test_3_time.txt" -n -metric -kml -hd 0 -mxpg 4

	echo *** Test 03 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 03.1 - map.kml >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test03\map.kml" "%IntegrationTests%Test03\Results\map.kml" >> %IntegrationTests%%TestResults%

	echo 03.2 - map.ppm >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test03\map.ppm" "%IntegrationTests%Test03\Results\map.ppm" >> %IntegrationTests%%TestResults%

	echo 03.3 - map-ck.ppm >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test03\map-ck.ppm" "%IntegrationTests%Test03\Results\map-ck.ppm" >> %IntegrationTests%%TestResults%

	echo 03.4 - test_3_trace.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test03\test_3_trace.txt" "%IntegrationTests%Test03\Results\test_3_trace.txt" >> %IntegrationTests%%TestResults%

	echo 03.5 - WNDC-DT-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test03\WNDC-DT_-site_report.txt" "%IntegrationTests%Test03\Results\WNDC-DT_-site_report.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 04.

if /i "%RunTest04%" EQU "1" (

	if exist %IntegrationTests%Test04\map.ppm del %IntegrationTests%Test04\map.ppm
	if exist %IntegrationTests%Test04\test_4_trace.txt del %IntegrationTests%Test04\test_4_trace.txt
	if exist %IntegrationTests%Test04\WNDC-DT-site_report.txt del %IntegrationTests%Test04\WNDC-DT-site_report.txt
	if exist %IntegrationTests%Test04\test_4_time.txt del %IntegrationTests%Test04\test_4_time.txt

	splat.exe -t "%IntegrationTests%Test04\wndc-dt.qth" -L 25 -gc 8 -o "%IntegrationTests%Test04\map.ppm" -d "%IntegrationTests%Test04\DEM\\" -rsp "%IntegrationTests%Test04\\" -s "%IntegrationTests%Test04\dc_sites.dat" "%IntegrationTests%Test04\md_sites.dat" "%IntegrationTests%Test04\va_sites.dat" -ano "%IntegrationTests%Test04\test_4_trace.txt" -b "%IntegrationTests%Test04\dc_counties.dat" "%IntegrationTests%Test04\md_counties.dat" "%IntegrationTests%Test04\va_counties.dat" -udt "%IntegrationTests%Test04\1000_ft_wall.txt" -db 150 -time "%IntegrationTests%Test04\test_4_time.txt" -olditm -hd 0 -mxpg 4

	echo *** Test 04 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 04.1 - map.ppm >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test04\map.ppm" "%IntegrationTests%Test04\Results\map.ppm" >> %IntegrationTests%%TestResults%

	echo 04.2 - test_4_trace.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test04\test_4_trace.txt" "%IntegrationTests%Test04\Results\test_4_trace.txt" >> %IntegrationTests%%TestResults%

	echo 04.3 - WNDC-DT-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test04\WNDC-DT-site_report.txt" "%IntegrationTests%Test04\Results\WNDC-DT-site_report.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 05.

if /i "%RunTest05%" EQU "1" (

	if exist %IntegrationTests%Test05\map.ppm del %IntegrationTests%Test05\map.ppm
	if exist %IntegrationTests%Test05\test_5_log.txt del %IntegrationTests%Test05\test_5_log.txt
	if exist %IntegrationTests%Test05\WNJU-DT-site_report.txt del %IntegrationTests%Test05\WNJU-DT-site_report.txt
	if exist %IntegrationTests%Test05\test_5_time.txt del %IntegrationTests%Test05\test_5_time.txt

	splat.exe -t "%IntegrationTests%Test05\wnju-dt.qth" -c 40 -R 100 -m 1.333 -o "%IntegrationTests%Test05\map.ppm" -d "%IntegrationTests%Test05\DEM\\" -rsp "%IntegrationTests%Test05\\" -ano "%IntegrationTests%Test05\test_5_trace.txt" -b "%IntegrationTests%Test05\nj_counties.dat" "%IntegrationTests%Test05\ny_counties.dat" "%IntegrationTests%Test05\pa_counties.dat" -log "%IntegrationTests%Test05\test_5_log.txt" -time "%IntegrationTests%Test05\test_5_time.txt" -metric -hd 0 -mxpg 9

	echo *** Test 05 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 05.1 - map.ppm >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test05\map.ppm" "%IntegrationTests%Test05\Results\map.ppm" >> %IntegrationTests%%TestResults%

	echo 05.2 - test_5_log.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test05\test_5_log.txt" "%IntegrationTests%Test05\Results\test_5_log.txt" >> %IntegrationTests%%TestResults%

	echo 05.3 - WNJU-DT-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test05\WNJU-DT-site_report.txt" "%IntegrationTests%Test05\Results\WNJU-DT-site_report.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 06.

if /i "%RunTest06%" EQU "1" (

	if exist %IntegrationTests%Test06\map.ppm del %IntegrationTests%Test06\map.ppm
	if exist %IntegrationTests%Test06\test_6_trace.txt del %IntegrationTests%Test06\test_6_trace.txt
	if exist %IntegrationTests%Test06\WNJU-DT-site_report.txt del %IntegrationTests%Test06\WNJU-DT-site_report.txt
	if exist %IntegrationTests%Test06\test_6_time.txt del %IntegrationTests%Test06\test_6_time.txt

	splat.exe -t "%IntegrationTests%Test06\wnju-dt.qth" -L 15 -gc 4 -R 60 -o "%IntegrationTests%Test06\map.ppm" -d "%IntegrationTests%Test06\DEM\\" -rsp "%IntegrationTests%Test06\\" -ano "%IntegrationTests%Test06\test_6_trace.txt" -b "%IntegrationTests%Test06\nj_counties.dat" "%IntegrationTests%Test06\ny_counties.dat" "%IntegrationTests%Test06\pa_counties.dat" -time "%IntegrationTests%Test06\test_6_time.txt" -metric -erp -dbm -N -sc -hd 0 -mxpg 9

	echo *** Test 06 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 06.1 - map.ppm >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test06\map.ppm" "%IntegrationTests%Test06\Results\map.ppm" >> %IntegrationTests%%TestResults%

	echo 06.2 - test_6_trace.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test06\test_6_trace.txt" "%IntegrationTests%Test06\Results\test_6_trace.txt" >> %IntegrationTests%%TestResults%

	echo 06.3 - WNJU-DT-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test06\WNJU-DT-site_report.txt" "%IntegrationTests%Test06\Results\WNJU-DT-site_report.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 07.

if /i "%RunTest07%" EQU "1" (

	if exist %IntegrationTests%Test07\path_loss_profile.png del %IntegrationTests%Test07\path_loss_profile.png
	if exist %IntegrationTests%Test07\profile.gp del %IntegrationTests%Test07\profile.gp
	if exist %IntegrationTests%Test07\splat.gp del %IntegrationTests%Test07\splat.gp
	if exist %IntegrationTests%Test07\TYSONS-TX-site_report.txt del %IntegrationTests%Test07\TYSONS-TX-site_report.txt
	if exist %IntegrationTests%Test07\TYSONS-TX-to-OSAGE-RX.txt del %IntegrationTests%Test07\TYSONS-TX-to-OSAGE-RX.txt
	if exist %IntegrationTests%Test07\test_7_time.txt del %IntegrationTests%Test07\test_7_time.txt

	splat.exe -t "%IntegrationTests%Test07\tysons-tx.qth" -r "%IntegrationTests%Test07\osage-rx.qth" -d "%IntegrationTests%Test07\DEM\\" -rsp "%IntegrationTests%Test07\\" -l "%IntegrationTests%Test07\path_loss_profile.png" -time "%IntegrationTests%Test07\test_7_time.txt" -metric -gpsav -hd 0 -mxpg 4

	echo *** Test 07 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 07.1 - path_loss_profile.png >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test07\path_loss_profile.png" "%IntegrationTests%Test07\Results\path_loss_profile.png" >> %IntegrationTests%%TestResults%

	echo 07.2 - profile.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test07\profile.gp" "%IntegrationTests%Test07\Results\profile.gp" >> %IntegrationTests%%TestResults%

	echo 07.3 - splat.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test07\splat.gp" "%IntegrationTests%Test07\Results\splat.gp" >> %IntegrationTests%%TestResults%

	echo 07.4 - TYSONS-TX-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test07\TYSONS-TX-site_report.txt" "%IntegrationTests%Test07\Results\TYSONS-TX-site_report.txt" >> %IntegrationTests%%TestResults%

	echo 07.5 - TYSONS-TX-to-OSAGE-RX.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test07\TYSONS-TX-to-OSAGE-RX.txt" "%IntegrationTests%Test07\Results\TYSONS-TX-to-OSAGE-RX.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 08.

if /i "%RunTest08%" EQU "1" (

	if exist %IntegrationTests%Test08\curvature.gp del %IntegrationTests%Test08\curvature.gp
	if exist %IntegrationTests%Test08\fresnel.gp del %IntegrationTests%Test08\fresnel.gp
	if exist %IntegrationTests%Test08\fresnel_pt_6.gp del %IntegrationTests%Test08\fresnel_pt_6.gp
	if exist %IntegrationTests%Test08\profile.gp del %IntegrationTests%Test08\profile.gp
	if exist %IntegrationTests%Test08\reference.gp del %IntegrationTests%Test08\reference.gp
	if exist %IntegrationTests%Test08\splat.gp del %IntegrationTests%Test08\splat.gp
	if exist %IntegrationTests%Test08\terrain_height_profile.png del %IntegrationTests%Test08\terrain_height_profile.png
	if exist %IntegrationTests%Test08\TYSONS-TX-site_report.txt del %IntegrationTests%Test08\TYSONS-TX-site_report.txt
	if exist %IntegrationTests%Test08\TYSONS-TX-to-OSAGE-RX.txt del %IntegrationTests%Test08\TYSONS-TX-to-OSAGE-RX.txt
	if exist %IntegrationTests%Test08\test_8_time.txt del %IntegrationTests%Test08\test_8_time.txt

	splat.exe -t "%IntegrationTests%Test08\tysons-tx.qth" -r "%IntegrationTests%Test08\osage-rx.qth" -d "%IntegrationTests%Test08\DEM\\" -rsp "%IntegrationTests%Test08\\" -h "%IntegrationTests%Test08\terrain_height_profile.png" -f 725 -fz 70 -time "%IntegrationTests%Test08\test_8_time.txt" -metric -gpsav -hd 0 -mxpg 4

	echo *** Test 08 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 08.1 - curvature.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test08\curvature.gp" "%IntegrationTests%Test08\Results\curvature.gp" >> %IntegrationTests%%TestResults%

	echo 08.2 - fresnel.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test08\fresnel.gp" "%IntegrationTests%Test08\Results\fresnel.gp" >> %IntegrationTests%%TestResults%

	echo 08.3 - fresnel_pt_6.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test08\fresnel_pt_6.gp" "%IntegrationTests%Test08\Results\fresnel_pt_6.gp" >> %IntegrationTests%%TestResults%

	echo 08.4 - profile.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test08\profile.gp" "%IntegrationTests%Test08\Results\profile.gp" >> %IntegrationTests%%TestResults%

	echo 08.5 - reference.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test08\reference.gp" "%IntegrationTests%Test08\Results\reference.gp" >> %IntegrationTests%%TestResults%

	echo 08.6 - splat.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test08\splat.gp" "%IntegrationTests%Test08\Results\splat.gp" >> %IntegrationTests%%TestResults%

	echo 08.7 - terrain_height_profile.png >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test08\terrain_height_profile.png" "%IntegrationTests%Test08\Results\terrain_height_profile.png" >> %IntegrationTests%%TestResults%

	echo 08.8 - TYSONS-TX-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test08\TYSONS-TX-site_report.txt" "%IntegrationTests%Test08\Results\TYSONS-TX-site_report.txt" >> %IntegrationTests%%TestResults%

	echo 08.9 - TYSONS-TX-to-OSAGE-RX.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test08\TYSONS-TX-to-OSAGE-RX.txt" "%IntegrationTests%Test08\Results\TYSONS-TX-to-OSAGE-RX.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 09.

if /i "%RunTest09%" EQU "1" (

	if exist %IntegrationTests%Test09\curvature.gp del %IntegrationTests%Test09\curvature.gp
	if exist %IntegrationTests%Test09\profile.gp del %IntegrationTests%Test09\profile.gp
	if exist %IntegrationTests%Test09\reference.gp del %IntegrationTests%Test09\reference.gp
	if exist %IntegrationTests%Test09\splat.gp del %IntegrationTests%Test09\splat.gp
	if exist %IntegrationTests%Test09\terrain_height_profile_normalized.png del %IntegrationTests%Test09\terrain_height_profile_normalized.png
	if exist %IntegrationTests%Test09\TYSONS-TX-site_report.txt del %IntegrationTests%Test09\TYSONS-TX-site_report.txt
	if exist %IntegrationTests%Test09\TYSONS-TX-to-OSAGE-RX.txt del %IntegrationTests%Test09\TYSONS-TX-to-OSAGE-RX.txt
	if exist %IntegrationTests%Test09\test_9_time.txt del %IntegrationTests%Test09\test_9_time.txt

	splat.exe -t "%IntegrationTests%Test09\tysons-tx.qth" -r "%IntegrationTests%Test09\osage-rx.qth" -d "%IntegrationTests%Test09\DEM\\" -rsp "%IntegrationTests%Test09\\" -H "%IntegrationTests%Test09\terrain_height_profile_normalized.png" -time "%IntegrationTests%Test09\test_9_time.txt" -nf -metric -gpsav -hd 0 -mxpg 4

	echo *** Test 09 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 09.1 - curvature.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test09\curvature.gp" "%IntegrationTests%Test09\Results\curvature.gp" >> %IntegrationTests%%TestResults%

	echo 09.2 - profile.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test09\profile.gp" "%IntegrationTests%Test09\Results\profile.gp" >> %IntegrationTests%%TestResults%

	echo 09.3 - reference.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test09\reference.gp" "%IntegrationTests%Test09\Results\reference.gp" >> %IntegrationTests%%TestResults%

	echo 09.4 - splat.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test09\splat.gp" "%IntegrationTests%Test09\Results\splat.gp" >> %IntegrationTests%%TestResults%

	echo 09.5 - terrain_height_profile_normalized.png >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test09\terrain_height_profile_normalized.png" "%IntegrationTests%Test09\Results\terrain_height_profile_normalized.png" >> %IntegrationTests%%TestResults%

	echo 09.6 - TYSONS-TX-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test09\TYSONS-TX-site_report.txt" "%IntegrationTests%Test09\Results\TYSONS-TX-site_report.txt" >> %IntegrationTests%%TestResults%

	echo 09.7 - TYSONS-TX-to-OSAGE-RX.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test09\TYSONS-TX-to-OSAGE-RX.txt" "%IntegrationTests%Test09\Results\TYSONS-TX-to-OSAGE-RX.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 10.

if /i "%RunTest10%" EQU "1" (

	if exist %IntegrationTests%Test10\profile.gp del %IntegrationTests%Test10\profile.gp
	if exist %IntegrationTests%Test10\splat.gp del %IntegrationTests%Test10\splat.gp
	if exist %IntegrationTests%Test10\terrain_profile.png del %IntegrationTests%Test10\terrain_profile.png
	if exist %IntegrationTests%Test10\TYSONS-TX-site_report.txt del %IntegrationTests%Test10\TYSONS-TX-site_report.txt
	if exist %IntegrationTests%Test10\TYSONS-TX-to-OSAGE-RX.txt del %IntegrationTests%Test10\TYSONS-TX-to-OSAGE-RX.txt
	if exist %IntegrationTests%Test10\test_10_time.txt del %IntegrationTests%Test10\test_10_time.txt

	splat.exe -t "%IntegrationTests%Test10\tysons-tx.qth" -r "%IntegrationTests%Test10\osage-rx.qth" -d "%IntegrationTests%Test10\DEM\\" -rsp "%IntegrationTests%Test10\\" -p "%IntegrationTests%Test10\terrain_profile.png" -time "%IntegrationTests%Test10\test_10_time.txt" -metric -gpsav -hd 0 -mxpg 4

	echo *** Test 10 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 10.1 - profile.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test10\profile.gp" "%IntegrationTests%Test10\Results\profile.gp" >> %IntegrationTests%%TestResults%

	echo 10.2 - splat.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test10\splat.gp" "%IntegrationTests%Test10\Results\splat.gp" >> %IntegrationTests%%TestResults%

	echo 10.3 - terrain_profile.png >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test10\terrain_profile.png" "%IntegrationTests%Test10\Results\terrain_profile.png" >> %IntegrationTests%%TestResults%

	echo 10.4 - TYSONS-TX-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test10\TYSONS-TX-site_report.txt" "%IntegrationTests%Test10\Results\TYSONS-TX-site_report.txt" >> %IntegrationTests%%TestResults%

	echo 10.5 - TYSONS-TX-to-OSAGE-RX.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test10\TYSONS-TX-to-OSAGE-RX.txt" "%IntegrationTests%Test10\Results\TYSONS-TX-to-OSAGE-RX.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 11.

if /i "%RunTest11%" EQU "1" (

	if exist %IntegrationTests%Test11\profile.gp del %IntegrationTests%Test11\profile.gp
	if exist %IntegrationTests%Test11\reference.gp del %IntegrationTests%Test11\reference.gp
	if exist %IntegrationTests%Test11\splat.gp del %IntegrationTests%Test11\splat.gp
	if exist %IntegrationTests%Test11\terrain_elevation_profile.png del %IntegrationTests%Test11\terrain_elevation_profile.png
	if exist %IntegrationTests%Test11\test_11_log.txt del %IntegrationTests%Test11\test_11_log.txt
	if exist %IntegrationTests%Test11\TYSONS-TX-site_report.txt del %IntegrationTests%Test11\TYSONS-TX-site_report.txt
	if exist %IntegrationTests%Test11\TYSONS-TX-to-OSAGE-RX.txt del %IntegrationTests%Test11\TYSONS-TX-to-OSAGE-RX.txt
	if exist %IntegrationTests%Test11\test_11_time.txt del %IntegrationTests%Test11\test_11_time.txt

	splat.exe -t "%IntegrationTests%Test11\tysons-tx.qth" -r "%IntegrationTests%Test11\osage-rx.qth" -d "%IntegrationTests%Test11\DEM\\" -rsp "%IntegrationTests%Test11\\" -e "%IntegrationTests%Test11\terrain_elevation_profile.png" -log "%IntegrationTests%Test11\test_11_log.txt" -time "%IntegrationTests%Test11\test_11_time.txt" -metric -gpsav -hd 0 -mxpg 4

	echo *** Test 11 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 11.1 - profile.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test11\profile.gp" "%IntegrationTests%Test11\Results\profile.gp" >> %IntegrationTests%%TestResults%

	echo 11.2 - reference.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test11\reference.gp" "%IntegrationTests%Test11\Results\reference.gp" >> %IntegrationTests%%TestResults%

	echo 11.3 - splat.gp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test11\splat.gp" "%IntegrationTests%Test11\Results\splat.gp" >> %IntegrationTests%%TestResults%

	echo 11.4 - terrain_elevation_profile.png >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test11\terrain_elevation_profile.png" "%IntegrationTests%Test11\Results\terrain_elevation_profile.png" >> %IntegrationTests%%TestResults%

	echo 11.5 - test_11_log.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test11\test_11_log.txt" "%IntegrationTests%Test11\Results\test_11_log.txt" >> %IntegrationTests%%TestResults%

	echo 11.6 - TYSONS-TX-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test11\TYSONS-TX-site_report.txt" "%IntegrationTests%Test11\Results\TYSONS-TX-site_report.txt" >> %IntegrationTests%%TestResults%

	echo 11.7 - TYSONS-TX-to-OSAGE-RX.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test11\TYSONS-TX-to-OSAGE-RX.txt" "%IntegrationTests%Test11\Results\TYSONS-TX-to-OSAGE-RX.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 12.

if /i "%RunTest12%" EQU "1" (

	if exist %IntegrationTests%Test12\test_12_input.txt (

		if exist %IntegrationTests%Test12\wnju-dt.ppm del %IntegrationTests%Test12\wnju-dt.ppm
		if exist %IntegrationTests%Test12\test_12_time.txt del %IntegrationTests%Test12\test_12_time.txt

		splat.exe -t "%IntegrationTests%Test12\wnju-dt.qth" -d "%IntegrationTests%Test12\DEM\\" -ani "%IntegrationTests%Test12\test_12_input.txt" -s "%IntegrationTests%Test12\sites.dat" -b "%IntegrationTests%Test12\nj_counties.dat" "%IntegrationTests%Test12\ny_counties.dat" "%IntegrationTests%Test12\pa_counties.dat" -time "%IntegrationTests%Test12\test_12_time.txt" -metric -hd 0 -mxpg 4

		echo *** Test 12 Results *** >> %IntegrationTests%%TestResults%

		echo.>> %IntegrationTests%%TestResults%

		echo 12.1 - wnju-dt.ppm >> %IntegrationTests%%TestResults%
		fc.exe /b "%IntegrationTests%Test12\wnju-dt.ppm" "%IntegrationTests%Test12\Results\wnju-dt.ppm" >> %IntegrationTests%%TestResults%

		echo.>> %IntegrationTests%%TestResults%

	) else (

		echo !!! Test 12 - Skipped - Need to unzip the test_12_input.txt file !!! >> %IntegrationTests%%TestResults%

		echo.>> %IntegrationTests%%TestResults%

	)

)


REM - Test 13.

if /i "%RunTest13%" EQU "1" (

	if exist %IntegrationTests%Test13\map.ppm del %IntegrationTests%Test13\map.ppm
	if exist %IntegrationTests%Test13\ship-1.lcf del %IntegrationTests%Test13\ship-1.lcf
	if exist %IntegrationTests%Test13\SHIP-1-site_report.txt del %IntegrationTests%Test13\SHIP-1-site_report.txt
	if exist %IntegrationTests%Test13\SHIP-2-site_report.txt del %IntegrationTests%Test13\SHIP-2-site_report.txt
	if exist %IntegrationTests%Test13\splat.lrp del %IntegrationTests%Test13\splat.lrp
	if exist %IntegrationTests%Test13\test_13_trace.txt del %IntegrationTests%Test13\test_13_trace.txt
	if exist %IntegrationTests%Test13\test_13_time.txt del %IntegrationTests%Test13\test_13_time.txt

	splat.exe -t "%IntegrationTests%Test13\ship-1.qth" "%IntegrationTests%Test13\ship-2.qth" -L 10.0 -o "%IntegrationTests%Test13\map.ppm" -rsp "%IntegrationTests%Test13\\" -ano "%IntegrationTests%Test13\test_13_trace.txt" -time "%IntegrationTests%Test13\test_13_time.txt" -metric -hd 1 -mxpg 4

	echo *** Test 13 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 13.1 - map.ppm >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test13\map.ppm" "%IntegrationTests%Test13\Results\map.ppm" >> %IntegrationTests%%TestResults%

	echo 13.2 - ship-1.lcf >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test13\ship-1.lcf" "%IntegrationTests%Test13\Results\ship-1.lcf" >> %IntegrationTests%%TestResults%

	echo 13.3 - SHIP-1-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test13\SHIP-1-site_report.txt" "%IntegrationTests%Test13\Results\SHIP-1-site_report.txt" >> %IntegrationTests%%TestResults%

	echo 13.4 - SHIP-2-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test13\SHIP-2-site_report.txt" "%IntegrationTests%Test13\Results\SHIP-2-site_report.txt" >> %IntegrationTests%%TestResults%

	echo 13.5 - splat.lrp >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test13\splat.lrp" "%IntegrationTests%Test13\Results\splat.lrp" >> %IntegrationTests%%TestResults%

	echo 13.6 - test_13_trace.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test13\test_13_trace.txt" "%IntegrationTests%Test13\Results\test_13_trace.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 14.

if /i "%RunTest14%" EQU "1" (

	if exist %IntegrationTests%Test14\antenna1-site_report.txt del %IntegrationTests%Test14\antenna1-site_report.txt
	if exist %IntegrationTests%Test14\antenna2-site_report.txt del %IntegrationTests%Test14\antenna2-site_report.txt
	if exist %IntegrationTests%Test14\map.ppm del %IntegrationTests%Test14\map.ppm
	if exist %IntegrationTests%Test14\test_14_trace.txt del %IntegrationTests%Test14\test_14_trace.txt
	if exist %IntegrationTests%Test14\test_14_time.txt del %IntegrationTests%Test14\test_14_time.txt

	splat.exe -t "%IntegrationTests%Test14\antenna1.qth" "%IntegrationTests%Test14\antenna2.qth" -L 15 -o "%IntegrationTests%Test14\map.ppm" -d "%IntegrationTests%Test14\DEM\\" -rsp "%IntegrationTests%Test14\\" -ano "%IntegrationTests%Test14\test_14_trace.txt" -time "%IntegrationTests%Test14\test_14_time.txt" -metric -hd 0 -mxpg 9

	echo *** Test 14 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 14.1 - antenna1-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test14\antenna1-site_report.txt" "%IntegrationTests%Test14\Results\antenna1-site_report.txt" >> %IntegrationTests%%TestResults%

	echo 14.2 - antenna2-site_report.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test14\antenna2-site_report.txt" "%IntegrationTests%Test14\Results\antenna2-site_report.txt" >> %IntegrationTests%%TestResults%

	echo 14.3 - map.ppm >> %IntegrationTests%%TestResults%
	fc.exe /b "%IntegrationTests%Test14\map.ppm" "%IntegrationTests%Test14\Results\map.ppm" >> %IntegrationTests%%TestResults%

	echo 14.4 - test_14_trace.txt >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test14\test_14_trace.txt" "%IntegrationTests%Test14\Results\test_14_trace.txt" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 15.

if /i "%RunTest15%" EQU "1" (

	if exist %IntegrationTests%Test15\41x42x73x74.sdf del %IntegrationTests%Test15\41x42x73x74.sdf

	srtm2sdf.exe -d "%IntegrationTests%Test15\\" "%IntegrationTests%Test15\N41W074.hgt"

	echo *** Test 15 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 15.1 - 41x42x73x74.sdf >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test15\41x42x73x74.sdf" "%IntegrationTests%Test15\Results\41x42x73x74.sdf" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


REM - Test 16.

if /i "%RunTest16%" EQU "1" (

	if exist %IntegrationTests%Test16\39x40x76x77.sdf del %IntegrationTests%Test16\39x40x76x77.sdf
	if exist %IntegrationTests%Test16\39x40x77x78.sdf del %IntegrationTests%Test16\39x40x77x78.sdf

	usgs2sdf.exe -d "%IntegrationTests%Test16\\" "%IntegrationTests%Test16\baltimore-e" "%IntegrationTests%Test16\baltimore-w"

	echo *** Test 16 Results *** >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

	echo 16.1 - 39x40x76x77.sdf >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test16\39x40x76x77.sdf" "%IntegrationTests%Test16\Results\39x40x76x77.sdf" >> %IntegrationTests%%TestResults%

	echo 16.2 - 39x40x77x78.sdf >> %IntegrationTests%%TestResults%
	fc.exe /a "%IntegrationTests%Test16\39x40x77x78.sdf" "%IntegrationTests%Test16\Results\39x40x77x78.sdf" >> %IntegrationTests%%TestResults%

	echo.>> %IntegrationTests%%TestResults%

)


echo.
echo.
echo.
echo IntegrationTestRunner complete.
echo.
pause

exit
