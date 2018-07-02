set grid
set yrange [-0.496 to 0.447]
set encoding iso_8859_1
set term png
set title "SPLAT Elevation Profile Between OSAGE-RX and TYSONS-TX (253.39° azimuth)"
set xlabel "Distance Between OSAGE-RX and TYSONS-TX (27.78 kilometers)"
set ylabel "Elevation Angle Along LOS Path Between\nOSAGE-RX and TYSONS-TX (degrees)"
set output "C:\\DEV\\SPLAT\\IntegrationTests\\Test11\\terrain_elevation_profile.png"
plot "C:\\DEV\\SPLAT\\IntegrationTests\\Test11\\profile.gp" title "Real Earth Profile" with lines, "C:\\DEV\\SPLAT\\IntegrationTests\\Test11\\reference.gp" title "Line of Sight Path (0.25° elevation)" with lines
