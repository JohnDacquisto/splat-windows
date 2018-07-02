set grid
set yrange [69.692 to 155.537]
set encoding iso_8859_1
set term png
set title "SPLAT Loss Profile Along Path Between OSAGE-RX and TYSONS-TX (253.39° azimuth)"
set xlabel "Distance Between OSAGE-RX and TYSONS-TX (27.78 kilometers)"
set ylabel "ITWOM Version 3.0 Path Loss (dB)"
set output "C:\\DEV\\SPLAT\\IntegrationTests\\Test07\\path_loss_profile.png"
plot "C:\\DEV\\SPLAT\\IntegrationTests\\Test07\\profile.gp" title "Path Loss" with lines
