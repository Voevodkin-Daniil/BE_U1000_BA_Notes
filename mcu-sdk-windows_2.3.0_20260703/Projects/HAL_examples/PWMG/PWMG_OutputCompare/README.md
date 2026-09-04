# Description

PWMG output compare mode example.

It shows how to configure the timer to generate an output waveform.

The timer operates in upcounting mode with the frequency of 10Hz defined
by the autoreload value.

The next output modes are implemented:
 - Toggle mode: When the counter reaches the value set in capture/compare register
                (set to half the auto-reload value), the output toggles its state
                and the Capture/Compare event interrupt is generated.
 - Forced inactive mode: The output forced to inactive state.
 - Forced active mode: The output forced to active state.

The counter starts in the toggle output mode. It may be changed by pressing the user button.

The output state is visualized with LED (that is controlled by software).
