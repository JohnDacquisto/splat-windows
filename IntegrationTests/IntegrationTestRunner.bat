@echo off
REM - IntegrationTestRunner
REM - To run integration tests, build SPLAT x64 Release, and unzip all regression files in IntegrationTests/TestXX/Results/ folders.
REM - Also, for test 12, unzip test_12_input in the IntegrationTests/Test12 folder.
REM - After running, check TestResults file, to see if any tests resulted in different files to investigate further.


REM - Testing setup.

REM - JAD TODO: Use relative file paths instead of SPLAT variables.
set SPLAT=C:\DEV\SPLAT
set SPLAT2=C:\\DEV\\SPLAT

cd %SPLAT%\x64\Release\

set TestResults=IntegrationTestResults.txt

if exist %SPLAT%\IntegrationTests\%TestResults% del %SPLAT%\IntegrationTests\%TestResults%


echo IntegrationTestRunner.
echo.
echo To run the integration tests, click any button. To not run the tests, exit out of the command prompt.
echo.
pause


REM - Test 01.

splat.exe -t "%SPLAT%\IntegrationTests\Test01\wnju-dt.qth" -L 20 -o "%SPLAT%\IntegrationTests\Test01\map.ppm" -d "%SPLAT%\IntegrationTests\Test01\DEM\\" -rsp "%SPLAT%\IntegrationTests\Test01\\" -ano "%SPLAT%\IntegrationTests\Test01\test_1_trace.txt" -time "%SPLAT%\IntegrationTests\Test01\test_1_time.txt" -metric -hd 0 -mxpg 4

echo *** Test 01 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 01.1 - map.ppm >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test01\map.ppm" "%SPLAT%\IntegrationTests\Test01\Results\map.ppm" >> %SPLAT%\IntegrationTests\%TestResults%

echo 01.2 - test_1_trace.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test01\test_1_trace.txt" "%SPLAT%\IntegrationTests\Test01\Results\test_1_trace.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 01.3 - WNJU-DT-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test01\WNJU-DT-site_report.txt" "%SPLAT%\IntegrationTests\Test01\Results\WNJU-DT-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 02.

splat.exe -t "%SPLAT%\IntegrationTests\Test02\wnju-dt.qth" -L 40 -gc 10 -R 50 -o "%SPLAT%\IntegrationTests\Test02\map.ppm" -d "%SPLAT%\IntegrationTests\Test02\DEM\\" -rsp "%SPLAT%\IntegrationTests\Test02\\" -ano "%SPLAT%\IntegrationTests\Test02\test_2_trace.txt" -time "%SPLAT%\IntegrationTests\Test02\test_2_time.txt" -geo -erp -dbm -ngs -hd 0 -mxpg 9

echo *** Test 02 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 02.1 - map.geo >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test02\map.geo" "%SPLAT%\IntegrationTests\Test02\Results\map.geo" >> %SPLAT%\IntegrationTests\%TestResults%

echo 02.2 - map.ppm >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test02\map.ppm" "%SPLAT%\IntegrationTests\Test02\Results\map.ppm" >> %SPLAT%\IntegrationTests\%TestResults%

echo 02.3 - test_2_trace.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test02\test_2_trace.txt" "%SPLAT%\IntegrationTests\Test02\Results\test_2_trace.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 02.4 - WNJU-DT-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test02\WNJU-DT_-site_report.txt" "%SPLAT%\IntegrationTests\Test02\Results\WNJU-DT_-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 03.

splat.exe -t "%SPLAT%\IntegrationTests\Test03\wndc-dt.qth" -L 10 -gc 3 -o "%SPLAT%\IntegrationTests\Test03\map.ppm" -d "%SPLAT%\IntegrationTests\Test03\DEM\\" -rsp "%SPLAT%\IntegrationTests\Test03\\" -s "%SPLAT%\IntegrationTests\Test03\dc_sites.dat" "%SPLAT%\IntegrationTests\Test03\md_sites.dat" "%SPLAT%\IntegrationTests\Test03\va_sites.dat" -ano "%SPLAT%\IntegrationTests\Test03\test_3_trace.txt" -time "%SPLAT%\IntegrationTests\Test03\test_3_time.txt" -n -metric -kml -hd 0 -mxpg 4

echo *** Test 03 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 03.1 - map.kml >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test03\map.kml" "%SPLAT%\IntegrationTests\Test03\Results\map.kml" >> %SPLAT%\IntegrationTests\%TestResults%

echo 03.2 - map.ppm >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test03\map.ppm" "%SPLAT%\IntegrationTests\Test03\Results\map.ppm" >> %SPLAT%\IntegrationTests\%TestResults%

echo 03.3 - map-ck.ppm >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test03\map-ck.ppm" "%SPLAT%\IntegrationTests\Test03\Results\map-ck.ppm" >> %SPLAT%\IntegrationTests\%TestResults%

echo 03.4 - test_3_trace.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test03\test_3_trace.txt" "%SPLAT%\IntegrationTests\Test03\Results\test_3_trace.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 03.5 - WNDC-DT-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test03\WNDC-DT_-site_report.txt" "%SPLAT%\IntegrationTests\Test03\Results\WNDC-DT_-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 04.

splat.exe -t "%SPLAT%\IntegrationTests\Test04\wndc-dt.qth" -L 25 -gc 8 -o "%SPLAT%\IntegrationTests\Test04\map.ppm" -d "%SPLAT%\IntegrationTests\Test04\DEM\\" -rsp "%SPLAT%\IntegrationTests\Test04\\" -s "%SPLAT%\IntegrationTests\Test04\dc_sites.dat" "%SPLAT%\IntegrationTests\Test04\md_sites.dat" "%SPLAT%\IntegrationTests\Test04\va_sites.dat" -ano "%SPLAT%\IntegrationTests\Test04\test_4_trace.txt" -b "%SPLAT%\IntegrationTests\Test04\dc_counties.dat" "%SPLAT%\IntegrationTests\Test04\md_counties.dat" "%SPLAT%\IntegrationTests\Test04\va_counties.dat" -udt "%SPLAT%\IntegrationTests\Test04\1000_ft_wall.txt" -db 150 -time "%SPLAT%\IntegrationTests\Test04\test_4_time.txt" -olditm -hd 0 -mxpg 4

echo *** Test 04 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 04.1 - map.ppm >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test04\map.ppm" "%SPLAT%\IntegrationTests\Test04\Results\map.ppm" >> %SPLAT%\IntegrationTests\%TestResults%

echo 04.2 - test_4_trace.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test04\test_4_trace.txt" "%SPLAT%\IntegrationTests\Test04\Results\test_4_trace.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 04.3 - WNDC-DT-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test04\WNDC-DT-site_report.txt" "%SPLAT%\IntegrationTests\Test04\Results\WNDC-DT-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 05.

splat.exe -t "%SPLAT%\IntegrationTests\Test05\wnju-dt.qth" -c 40 -R 100 -m 1.333 -o "%SPLAT%\IntegrationTests\Test05\map.ppm" -d "%SPLAT%\IntegrationTests\Test05\DEM\\" -rsp "%SPLAT%\IntegrationTests\Test05\\" -ano "%SPLAT%\IntegrationTests\Test05\test_5_trace.txt" -b "%SPLAT%\IntegrationTests\Test05\nj_counties.dat" "%SPLAT%\IntegrationTests\Test05\ny_counties.dat" "%SPLAT%\IntegrationTests\Test05\pa_counties.dat" -log "%SPLAT%\IntegrationTests\Test05\test_5_log.txt" -time "%SPLAT%\IntegrationTests\Test05\test_5_time.txt" -metric -hd 0 -mxpg 9

echo *** Test 05 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 05.1 - map.ppm >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test05\map.ppm" "%SPLAT%\IntegrationTests\Test05\Results\map.ppm" >> %SPLAT%\IntegrationTests\%TestResults%

echo 05.2 - test_5_log.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test05\test_5_log.txt" "%SPLAT%\IntegrationTests\Test05\Results\test_5_log.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 05.3 - WNJU-DT-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test05\WNJU-DT-site_report.txt" "%SPLAT%\IntegrationTests\Test05\Results\WNJU-DT-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 06.

splat.exe -t "%SPLAT%\IntegrationTests\Test06\wnju-dt.qth" -L 15 -gc 4 -R 60 -o "%SPLAT%\IntegrationTests\Test06\map.ppm" -d "%SPLAT%\IntegrationTests\Test06\DEM\\" -rsp "%SPLAT%\IntegrationTests\Test06\\" -ano "%SPLAT%\IntegrationTests\Test06\test_6_trace.txt" -b "%SPLAT%\IntegrationTests\Test06\nj_counties.dat" "%SPLAT%\IntegrationTests\Test06\ny_counties.dat" "%SPLAT%\IntegrationTests\Test06\pa_counties.dat" -time "%SPLAT%\IntegrationTests\Test06\test_6_time.txt" -metric -erp -dbm -N -sc -hd 0 -mxpg 9

echo *** Test 06 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 06.1 - map.ppm >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test06\map.ppm" "%SPLAT%\IntegrationTests\Test06\Results\map.ppm" >> %SPLAT%\IntegrationTests\%TestResults%

echo 06.2 - test_6_trace.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test06\test_6_trace.txt" "%SPLAT%\IntegrationTests\Test06\Results\test_6_trace.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 06.3 - WNJU-DT-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test06\WNJU-DT-site_report.txt" "%SPLAT%\IntegrationTests\Test06\Results\WNJU-DT-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 07.

splat.exe -t "%SPLAT%\IntegrationTests\Test07\tysons-tx.qth" -r "%SPLAT%\IntegrationTests\Test07\osage-rx.qth" -d "%SPLAT%\IntegrationTests\Test07\DEM\\" -rsp "%SPLAT2%\\IntegrationTests\\Test07\\" -l "%SPLAT2%\\IntegrationTests\\Test07\\path_loss_profile.png" -time "%SPLAT%\IntegrationTests\Test07\test_7_time.txt" -metric -gpsav -hd 0 -mxpg 4

echo *** Test 07 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 07.1 - path_loss_profile.png >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test07\path_loss_profile.png" "%SPLAT%\IntegrationTests\Test07\Results\path_loss_profile.png" >> %SPLAT%\IntegrationTests\%TestResults%

echo 07.2 - profile.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test07\profile.gp" "%SPLAT%\IntegrationTests\Test07\Results\profile.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 07.3 - splat.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test07\splat.gp" "%SPLAT%\IntegrationTests\Test07\Results\splat.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 07.4 - TYSONS-TX-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test07\TYSONS-TX-site_report.txt" "%SPLAT%\IntegrationTests\Test07\Results\TYSONS-TX-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 07.5 - TYSONS-TX-to-OSAGE-RX.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test07\TYSONS-TX-to-OSAGE-RX.txt" "%SPLAT%\IntegrationTests\Test07\Results\TYSONS-TX-to-OSAGE-RX.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 08.

splat.exe -t "%SPLAT%\IntegrationTests\Test08\tysons-tx.qth" -r "%SPLAT%\IntegrationTests\Test08\osage-rx.qth" -d "%SPLAT%\IntegrationTests\Test08\DEM\\" -rsp "%SPLAT2%\\IntegrationTests\\Test08\\" -h "%SPLAT2%\\IntegrationTests\\Test08\\terrain_height_profile.png" -f 725 -fz 70 -time "%SPLAT%\IntegrationTests\Test08\test_8_time.txt" -metric -gpsav -hd 0 -mxpg 4

echo *** Test 08 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 08.1 - curvature.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test08\curvature.gp" "%SPLAT%\IntegrationTests\Test08\Results\curvature.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 08.2 - fresnel.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test08\fresnel.gp" "%SPLAT%\IntegrationTests\Test08\Results\fresnel.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 08.3 - fresnel_pt_6.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test08\fresnel_pt_6.gp" "%SPLAT%\IntegrationTests\Test08\Results\fresnel_pt_6.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 08.4 - profile.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test08\profile.gp" "%SPLAT%\IntegrationTests\Test08\Results\profile.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 08.5 - reference.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test08\reference.gp" "%SPLAT%\IntegrationTests\Test08\Results\reference.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 08.6 - splat.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test08\splat.gp" "%SPLAT%\IntegrationTests\Test08\Results\splat.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 08.7 - terrain_height_profile.png >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test08\terrain_height_profile.png" "%SPLAT%\IntegrationTests\Test08\Results\terrain_height_profile.png" >> %SPLAT%\IntegrationTests\%TestResults%

echo 08.8 - TYSONS-TX-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test08\TYSONS-TX-site_report.txt" "%SPLAT%\IntegrationTests\Test08\Results\TYSONS-TX-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 08.9 - TYSONS-TX-to-OSAGE-RX.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test08\TYSONS-TX-to-OSAGE-RX.txt" "%SPLAT%\IntegrationTests\Test08\Results\TYSONS-TX-to-OSAGE-RX.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 09.

splat.exe -t "%SPLAT%\IntegrationTests\Test09\tysons-tx.qth" -r "%SPLAT%\IntegrationTests\Test09\osage-rx.qth" -d "%SPLAT%\IntegrationTests\Test09\DEM\\" -rsp "%SPLAT2%\\IntegrationTests\\Test09\\" -H "%SPLAT2%\\IntegrationTests\\Test09\\terrain_height_profile_normalized.png" -time "%SPLAT%\IntegrationTests\Test09\test_9_time.txt" -nf -metric -gpsav -hd 0 -mxpg 4

echo *** Test 09 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 09.1 - curvature.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test09\curvature.gp" "%SPLAT%\IntegrationTests\Test09\Results\curvature.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 09.2 - profile.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test09\profile.gp" "%SPLAT%\IntegrationTests\Test09\Results\profile.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 09.3 - reference.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test09\reference.gp" "%SPLAT%\IntegrationTests\Test09\Results\reference.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 09.4 - splat.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test09\splat.gp" "%SPLAT%\IntegrationTests\Test09\Results\splat.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 09.5 - terrain_height_profile_normalized.png >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test09\terrain_height_profile_normalized.png" "%SPLAT%\IntegrationTests\Test09\Results\terrain_height_profile_normalized.png" >> %SPLAT%\IntegrationTests\%TestResults%

echo 09.6 - TYSONS-TX-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test09\TYSONS-TX-site_report.txt" "%SPLAT%\IntegrationTests\Test09\Results\TYSONS-TX-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 09.7 - TYSONS-TX-to-OSAGE-RX.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test09\TYSONS-TX-to-OSAGE-RX.txt" "%SPLAT%\IntegrationTests\Test09\Results\TYSONS-TX-to-OSAGE-RX.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 10.

splat.exe -t "%SPLAT%\IntegrationTests\Test10\tysons-tx.qth" -r "%SPLAT%\IntegrationTests\Test10\osage-rx.qth" -d "%SPLAT%\IntegrationTests\Test10\DEM\\" -rsp "%SPLAT2%\\IntegrationTests\\Test10\\" -p "%SPLAT2%\\IntegrationTests\\Test10\\terrain_profile.png" -time "%SPLAT%\IntegrationTests\Test10\test_10_time.txt" -metric -gpsav -hd 0 -mxpg 4

echo *** Test 10 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 10.1 - profile.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test10\profile.gp" "%SPLAT%\IntegrationTests\Test10\Results\profile.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 10.2 - splat.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test10\splat.gp" "%SPLAT%\IntegrationTests\Test10\Results\splat.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 10.3 - terrain_profile.png >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test10\terrain_profile.png" "%SPLAT%\IntegrationTests\Test10\Results\terrain_profile.png" >> %SPLAT%\IntegrationTests\%TestResults%

echo 10.4 - TYSONS-TX-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test10\TYSONS-TX-site_report.txt" "%SPLAT%\IntegrationTests\Test10\Results\TYSONS-TX-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 10.5 - TYSONS-TX-to-OSAGE-RX.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test10\TYSONS-TX-to-OSAGE-RX.txt" "%SPLAT%\IntegrationTests\Test10\Results\TYSONS-TX-to-OSAGE-RX.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 11.

splat.exe -t "%SPLAT%\IntegrationTests\Test11\tysons-tx.qth" -r "%SPLAT%\IntegrationTests\Test11\osage-rx.qth" -d "%SPLAT%\IntegrationTests\Test11\DEM\\" -rsp "%SPLAT2%\\IntegrationTests\\Test11\\" -e "%SPLAT2%\\IntegrationTests\\Test11\\terrain_elevation_profile.png" -log "%SPLAT%\IntegrationTests\Test11\test_11_log.txt" -time "%SPLAT%\IntegrationTests\Test11\test_11_time.txt" -metric -gpsav -hd 0 -mxpg 4

echo *** Test 11 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 11.1 - profile.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test11\profile.gp" "%SPLAT%\IntegrationTests\Test11\Results\profile.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 11.2 - reference.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test11\reference.gp" "%SPLAT%\IntegrationTests\Test11\Results\reference.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 11.3 - splat.gp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test11\splat.gp" "%SPLAT%\IntegrationTests\Test11\Results\splat.gp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 11.4 - terrain_elevation_profile.png >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test11\terrain_elevation_profile.png" "%SPLAT%\IntegrationTests\Test11\Results\terrain_elevation_profile.png" >> %SPLAT%\IntegrationTests\%TestResults%

echo 11.5 - test_11_log.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test11\test_11_log.txt" "%SPLAT%\IntegrationTests\Test11\Results\test_11_log.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 11.6 - TYSONS-TX-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test11\TYSONS-TX-site_report.txt" "%SPLAT%\IntegrationTests\Test11\Results\TYSONS-TX-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 11.7 - TYSONS-TX-to-OSAGE-RX.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test11\TYSONS-TX-to-OSAGE-RX.txt" "%SPLAT%\IntegrationTests\Test11\Results\TYSONS-TX-to-OSAGE-RX.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 12.

splat.exe -t "%SPLAT%\IntegrationTests\Test12\wnju-dt.qth" -d "%SPLAT%\IntegrationTests\Test12\DEM\\" -ani "%SPLAT%\IntegrationTests\Test12\test_12_input.txt" -s "%SPLAT%\IntegrationTests\Test12\sites.dat" -b "%SPLAT%\IntegrationTests\Test12\nj_counties.dat" "%SPLAT%\IntegrationTests\Test12\ny_counties.dat" "%SPLAT%\IntegrationTests\Test12\pa_counties.dat" -time "%SPLAT%\IntegrationTests\Test12\test_12_time.txt" -metric -hd 0 -mxpg 4

echo *** Test 12 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 12.1 - wnju-dt.ppm >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test12\wnju-dt.ppm" "%SPLAT%\IntegrationTests\Test12\Results\wnju-dt.ppm" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 13.

REM - This test involves creating a default splat.lrp file, so if it already exists, delete it before running the test.
if exist %SPLAT%\IntegrationTests\Test13\splat.lrp del %SPLAT%\IntegrationTests\Test13\splat.lrp

splat.exe -t "%SPLAT%\IntegrationTests\Test13\ship-1.qth" "%SPLAT%\IntegrationTests\Test13\ship-2.qth" -L 10.0 -o "%SPLAT%\IntegrationTests\Test13\map.ppm" -rsp "%SPLAT%\IntegrationTests\Test13\\" -ano "%SPLAT%\IntegrationTests\Test13\test_13_trace.txt" -time "%SPLAT%\IntegrationTests\Test13\test_13_time.txt" -metric -hd 1 -mxpg 4

echo *** Test 13 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 13.1 - map.ppm >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test13\map.ppm" "%SPLAT%\IntegrationTests\Test13\Results\map.ppm" >> %SPLAT%\IntegrationTests\%TestResults%

echo 13.2 - ship-1.lcf >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test13\ship-1.lcf" "%SPLAT%\IntegrationTests\Test13\Results\ship-1.lcf" >> %SPLAT%\IntegrationTests\%TestResults%

echo 13.3 - SHIP-1-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test13\SHIP-1-site_report.txt" "%SPLAT%\IntegrationTests\Test13\Results\SHIP-1-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 13.4 - SHIP-2-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test13\SHIP-2-site_report.txt" "%SPLAT%\IntegrationTests\Test13\Results\SHIP-2-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 13.5 - splat.lrp >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test13\splat.lrp" "%SPLAT%\IntegrationTests\Test13\Results\splat.lrp" >> %SPLAT%\IntegrationTests\%TestResults%

echo 13.6 - test_13_trace.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test13\test_13_trace.txt" "%SPLAT%\IntegrationTests\Test13\Results\test_13_trace.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 14.

splat.exe -t "%SPLAT%\IntegrationTests\Test14\antenna1.qth" "%SPLAT%\IntegrationTests\Test14\antenna2.qth" -L 15 -o "%SPLAT%\IntegrationTests\Test14\map.ppm" -d "%SPLAT%\IntegrationTests\Test14\DEM\\" -rsp "%SPLAT%\IntegrationTests\Test14\\" -ano "%SPLAT%\IntegrationTests\Test14\test_14_trace.txt" -time "%SPLAT%\IntegrationTests\Test14\test_14_time.txt" -metric -hd 0 -mxpg 9

echo *** Test 14 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 14.1 - antenna1-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test14\antenna1-site_report.txt" "%SPLAT%\IntegrationTests\Test14\Results\antenna1-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 14.2 - antenna2-site_report.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test14\antenna2-site_report.txt" "%SPLAT%\IntegrationTests\Test14\Results\antenna2-site_report.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo 14.3 - map.ppm >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /b "%SPLAT%\IntegrationTests\Test14\map.ppm" "%SPLAT%\IntegrationTests\Test14\Results\map.ppm" >> %SPLAT%\IntegrationTests\%TestResults%

echo 14.4 - test_14_trace.txt >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test14\test_14_trace.txt" "%SPLAT%\IntegrationTests\Test14\Results\test_14_trace.txt" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 15.

srtm2sdf.exe -d "%SPLAT%\IntegrationTests\Test15\\" "%SPLAT%\IntegrationTests\Test15\N41W074.hgt"

echo *** Test 15 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 15.1 - 41x42x73x74.sdf >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test15\41x42x73x74.sdf" "%SPLAT%\IntegrationTests\Test15\Results\41x42x73x74.sdf" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


REM - Test 16.

usgs2sdf.exe -d "%SPLAT%\IntegrationTests\Test16\\" "%SPLAT%\IntegrationTests\Test16\baltimore-e" "%SPLAT%\IntegrationTests\Test16\baltimore-w"

echo *** Test 16 Results *** >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%

echo 16.1 - 39x40x76x77.sdf >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test16\39x40x76x77.sdf" "%SPLAT%\IntegrationTests\Test16\Results\39x40x76x77.sdf" >> %SPLAT%\IntegrationTests\%TestResults%

echo 16.2 - 39x40x77x78.sdf >> %SPLAT%\IntegrationTests\%TestResults%
fc.exe /a "%SPLAT%\IntegrationTests\Test16\39x40x77x78.sdf" "%SPLAT%\IntegrationTests\Test16\Results\39x40x77x78.sdf" >> %SPLAT%\IntegrationTests\%TestResults%

echo.>> %SPLAT%\IntegrationTests\%TestResults%


echo.
echo.
echo.
echo IntegrationTestRunner complete.
echo.
pause

exit
