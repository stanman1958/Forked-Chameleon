G1 Z{layer_height+75} F6000
;cut and remove the last filament used
G90
; go to holding position to start...forked chameleon cuts and unloads the last filament, when the print finishes...send end print gcode!!!
G0 X-110 Y107 F2000  ; <<----- LOCATION OF THE BUTTON

G91 ; move to relative mode
M83
G1 E-3 F2000
G0 Y3
G4 P4800 ; dwell for 4.8 seconds - adjust this to match your machines single pulse time
G0 Y-3

G90 ; move back to absolute mode
M83 ; but make sure the extruder stays in relative


G0 X110 Y110 F6000
M104 S0 ; turn off temperature
