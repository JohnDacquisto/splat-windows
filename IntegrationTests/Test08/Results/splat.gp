set grid
set yrange [-234.008 to 7.549]
set y2range [-241.459 to 0.099]
set xrange [-0.5 to 28.968]
set encoding iso_8859_1
set term png
set title "SPLAT Path Profile Between OSAGE-RX and TYSONS-TX (253.39° azimuth)\nWith First Fresnel Zone"
set xlabel "Distance Between OSAGE-RX and TYSONS-TX (27.78 kilometers)"
set ylabel "Height Referenced To LOS Path Between\nOSAGE-RX and TYSONS-TX (meters)"
set output "C:\\DEV\\SPLAT\\IntegrationTests\\Test08\\terrain_height_profile.png"
plot "C:\\DEV\\SPLAT\\IntegrationTests\\Test08\\profile.gp" title "Point-to-Point Profile" with lines, "C:\\DEV\\SPLAT\\IntegrationTests\\Test08\\reference.gp" title "Line of Sight Path" with lines, "C:\\DEV\\SPLAT\\IntegrationTests\\Test08\\curvature.gp" axes x1y2 title "Earth's Curvature Contour" with lines, "C:\\DEV\\SPLAT\\IntegrationTests\\Test08\\fresnel.gp" axes x1y1 title "First Fresnel Zone (725.000 MHz)" with lines, "C:\\DEV\\SPLAT\\IntegrationTests\\Test08\\fresnel_pt_6.gp" title "70% of First Fresnel Zone" with lines
