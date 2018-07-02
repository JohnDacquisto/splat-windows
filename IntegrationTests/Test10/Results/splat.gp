set grid
set yrange [13.630 to 137.000]
set encoding iso_8859_1
set term png
set title "SPLAT Terrain Profile Between OSAGE-RX and TYSONS-TX (253.39° Azimuth)"
set xlabel "Distance Between OSAGE-RX and TYSONS-TX (27.78 kilometers)"
set ylabel "Ground Elevation Above Sea Level (meters)"
set output "C:\\DEV\\SPLAT\\IntegrationTests\\Test10\\terrain_profile.png"
plot "C:\\DEV\\SPLAT\\IntegrationTests\\Test10\\profile.gp" title "" with lines
