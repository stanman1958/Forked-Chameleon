G28
M190 S[bed_temperature_initial_layer_single]
M104 S[nozzle_temperature_initial_layer]

G90
M83
;Clear any filament in the tool head before starting...if already removed nothing happens
;this is for those times I cancel a print...the normal end print gcode will not run (I think)
; go to holding position
G0 X-110 Y107 F2000  ; <<----- LOCATION OF THE BUTTON
G1 Z{layer_height+75} F1000
G91 ; move to relative mode
M83
G0 Y3
G4 P4800 ; dwell for 4.8 seconds - adjust this to match your machines single pulse time
G0 Y-3
;G90 ; move back to absolute mode
M83 ; but make sure the extruder stays in relative
G4 P10000 ; delay incase everthing is still hot....waiting for the unload, if needed

; Select the initial tool
T{initial_tool}

; Use a conditional statement to select the correct button press time
;G91 ; Switch to relative positioning
M83
G0 y3
{if initial_tool == 0}
G4 P800 ; dwell for 0.8 seconds
{endif}
{if initial_tool == 1}
G4 P1800 ; dwell for 2 seconds
{endif}
{if initial_tool == 2}
G4 P2700 ; dwell for 3 seconds
{endif}
{if initial_tool == 3}
G4 P4000 ; dwell for 4 seconds
{endif}
G0 Y-3


G1 E500 F1000 ; Load filament
G1 E-3 F2400
G92 E0
g4 p5000

; Return to absolute positioning and prepare for print
G90 ; Switch back to absolute mode
M83
G0 X110 Y110 F6000

; Priming and wiping sequence
;M117 Priming nozzle...
G92 E0
G1 Z5 F6000
G1 E-0.2 F800
G1 X110 Y110 F6000
G1 E2 F800
G1 X110 Y55 Z0.25 F4800
G1 Y-55 E8 F2400
G1 X109.6 F2400
G1 Y55 E5 F2400
G1 X110 Y55 Z0.45 F4800
G1 Y-55 E8 F2400
G1 X109.6 F2400
G1 Y55 E5 F2400
G92 E0
;M117 Printing...
