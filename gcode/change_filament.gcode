;begin change filament gcode
{if previous_extruder != next_extruder}

    ;Forked Chameleon GCode
    G1 Z{layer_height+75} F1000
    G92 E0
    G0 X-110 Y107 F2000  ; <<----- LOCATION OF THE BUTTON

    ; *** FIX 1: ADDED G91 TO ENSURE RELATIVE MOVES ***
    G91 ; move to relative mode
    M83
    G0 y3

    {if next_extruder==0}
    G4 P800 ; dwell for 0.8 seconds - adjust this to match your machines time
    {endif}
    {if next_extruder==1}
    G4 P1800 ; dwell for 2 seconds - adjust this to match your machines time
    {endif}
    {if next_extruder==2}
    G4 P2700 ; dwell for 3 seconds - adjust this to match your machines time
    {endif}
    {if next_extruder==3}
    G4 P4000 ; dwell for 4 seconds - adjust this to match your machines time
    {endif}

    G0 Y-3

    G1 E500 F1000 ; Load filament
    G1 E-3 F2400
    G92 E0
    g4 p5000

    G90 ; move back to absolute mode
    M83 ; but make sure the extruder stays in relative
    G0 X110 Y110 F6000
    G4 P5000
{endif}
;end change filament gcode
