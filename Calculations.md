
# Required Motor Torque
```
The total area of the window is 70" x 46" = 2 square meters.
Typical blackout cloth has a weight of 250 grams / square meter.
 -> Total weight of cloth should be 500 grams.

At max extension, the entire 500 gram load is pulled on
the outside of the curtain rod, which has a radius of 1.6 cm.
500 grams * 1.6cm * 9.8 m/s^2 = 7.84 N*cm

With a 1:1 gear ratio, this well within range for our 30 N*cm motor.
```

# Timing of Motor Pull
```
The window is 46" high, and we're wrapping the cloth around a
1.25" PVC pipe (circumference = 4"). With a 60 RPM motor at
a 1:1 gear ratio, we'll need to run it for around
(46" / 4") * (1 / 1) * (60 / 60) = 11.5 seconds
```

