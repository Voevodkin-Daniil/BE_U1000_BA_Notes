# Description

PWMG time base example.

It shows how to configure the timer to generate base time.

The timer operates in upcounting mode with the period defined by the autoreload
value. When the timer reaches the maximal value, it's reloaded and the Update
event interrupt is generated. The Update event interrupt is used to toggle LED.
The timer period may be modified by pressing the user button. When the user button
is pressed, the timer period is switched to the next preprogrammed value.
