# Description

PWMG output compare mode example.

It shows how to configure the timer to generate a PWM waveform on an output.

The timer operates in upcounting mode with the frequency of 100Hz defined
by the autoreload value.

The counter uses PWM1 output compare mode. It means that the output is active
while CNT <= CCR, otherwise it goes inactive. The duty cycle of the output is
controlled by setting of CCR register value. The counter starts at 50% duty cycle.
Afterwards it can be modified in the range of 0 to 100% with the step of 10%
by pressing the user button.
