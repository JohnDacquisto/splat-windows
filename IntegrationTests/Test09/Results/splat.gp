set grid
set yrange [-16.062 to 244.872]
set y2range [-20.189 to 240.744]
set xrange [-0.5 to 28.968]
set encoding iso_8859_1
set term png
set title "SPLAT Height Profile Between OSAGE-RX and TYSONS-TX (253.39° azimuth)"
set xlabel "Distance Between OSAGE-RX and TYSONS-TX (27.78 kilometers)"
set ylabel "Normalized Height Referenced To LOS Path Between\nOSAGE-RX and TYSONS-TX (meters)"
set output "C:\\DEV\\SPLAT\\IntegrationTests\\Test09\\terrain_height_profile_normalized.png"
plot "C:\\DEV\\SPLAT\\IntegrationTests\\Test09\\profile.gp" title "Point-to-Point Profile" with lines, "C:\\DEV\\SPLAT\\IntegrationTests\\Test09\\reference.gp" title "Line Of Sight Path" with lines, "C:\\DEV\\SPLAT\\IntegrationTests\\Test09\\curvature.gp" axes x1y2 title "Earth's Curvature Contour" with lines
