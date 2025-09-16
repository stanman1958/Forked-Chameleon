# Forked-Chameleon
This is the collection of parts, BOMs, code, etc for my fork of the 3DChameleon, specifically for the FlashForge AD5M
You are free to copy, modify, etc with attribution.

I started this after trying to get the original 3DChameleon to work.  I am sure that it would have worked, with a lot of tuning and adjustments to 
compensate for any tolerance issues.  I decided to take a different approach and the result was what I am calling the Forked Chameleon.

My issues were primarily around the selector portion of the 3DChameleon.  I could not consistently/repeatedly select a specific filament, at times have two filaments active.  
By splitting the 3DChameleon into two separate pieces, each one only selecting between 1 of 2 possible filaments and that selection excluding the other filament I was able to 
achieve repeatable and predictable fialment selection.

The original 3DChameleon used two stepper motors, one as an extruder the fed the filaments to the toolhead with the other stepper basically function as a servo, rotating a cam
with two bearings, driving the two halves of the 3DChameleon and forcing a filament against the extruder gear.  My fork uses two steppers but each is part of a separate assembly,
a Chameleon.  In place of the stepper used to rotate the cam, I use a servo that rotates a modifies cam with a single bearing that then selects 1 of the 2 filaments.  So, at the expense of adding two servos I was able to simplify the mechanics of selecting and feeding the filaments. 
