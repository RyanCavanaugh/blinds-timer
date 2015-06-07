# Overview
The control unit has 4 buttons
 * Add Large Time
 * Add Small Time
 * Go Up
 * Go Down

There are also 5 indicator lights
 * 30 minutes
 * 1 hours
 * 2 hours
 * 4 hours
 * 8 hours

To read the countdown timer, add up the values
of the lit indicator lights. For example, if the
"30 minutes" and "4 hour" lights are lit, the
countdown timer has 4 hours and 30 minutes remaining.

# Buttons
### Go Up
Hold down this button to raise the blinds

### Go Down
Hold down this button to lower the blinds

### Add Large Time
This button adds 2 hours to the countdown timer

### Add Small Time
This button adds 30 minutes to the countdown timer

# Setting the Timer
To set the timer, press the Add Time buttons until the desired length of time
is shown in the indicator lights. After 5 seconds of no button presses, the indicator
lights will flash to confirm that the countdown has started.

While the countdown is running, the current time remaining will blink
in the indicator lights at 1-second intervals.

To cancel the countdown, press the Go Up or Go Down buttons. The indicator lights
will flash to confirm cancellation, and the countdown timer will reset to 0.

# Adjusting the Raise Timing
When the counter expires, the blinds will raise for the number of seconds
indicated by the DIP switch. The 4 pins of the switch correspond to 1,
2, 4, and 8 seconds, read as a binary value.

# Startup Sequence and Diagnostics
When the control unit first receives power, it will go through a short
self-diagnostic sequence. You should see each LED light up in sequence,
followed by a display of the current values of the DIP switch.

### Clock Test Mode
If all DIP switches are in the Off position, the unit enters Clock Test Mode.
This counts up, in binary, in half-second intervals. You can use this to measure
the clock calibration quality.

### Button Test Mode
If only the '1' position DIP switch is On, the unit enters 16 seconds of
Button Test Mode. In this mode, the LEDs will light up the current value
of the DIP switches. If a button is pressed, the LEDs will instead light up
the '8 hour' LED plus a light corresponding to the button that is being pressed:
 * Add Small: 30m
 * Add Large: 1 hr
 * Go Up: 2 hr
 * Go Down: 8 hr

### Button Error Mode
If any button is pressed during startup, the corresponding LED from the
'Button Test Mode' setting will blink continuously.
