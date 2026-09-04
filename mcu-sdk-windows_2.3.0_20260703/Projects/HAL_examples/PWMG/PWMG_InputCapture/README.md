# Description

PWMG input capture mode example.

It shows how to configure the timer to capture the input signal and calculate its frequency.

The timer is configured to measure frequency in the range of about 500Hz to 25kHz. The signal
on input may be provided either from an external generator or from another PWMG timer
instance configured in the example. Use the external wire to connect input capture (pin PB15)
to the output of the second PWMG instance (pin PA15). The on-board output provides with a set
of reference frequencies in the range of 1kHz to 25kHz that can be switched by pressing
the user button.
