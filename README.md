# Claw-Machine-Arduino-Mega

Arcade Game Claw Machine

Uses Arduino Mega, Nema 17 stepper motors (1 for X axis, 2 for Y axis, 1 for Z axis) and hobby servo for operating the claw mechanism. 2 endstop switches for X and Y axes, 1 for Z axis. LED lights connected through a relay - to pin 23. Arcade joystick controls gantry movement (not analog - it consists of 4 micro switches). Credit button - it's IR gate, that detects coin, while it's passing through coin slot.

Simulate this project on https://wokwi.com/projects/360071199419997185

Added sketch with MANUAL MODE, which can be toggled while there's no credits available, instructions within sketch.

Added rotary encoder for adjusting game time limit (stored in eeprom), updated messages displayed, added pushbuttons leds.
