# Description

Advanced timer usage example.
Configures the timer channels in Input Capture mode to calculate PWM input signal parameters.
The PWMA2 channel 0 is used to generate the PWM signal.
The values of duty cycle are in the buffer pwm_data[] and are equal to: 10%, 20%, 30%, 40%, 50%, 60%, 70%, 80%, 90%, 15%.
DMA0 channel 0 is used to set the duty cycle value.
DMA0 channel 1 is used to transfer data from CCR0 and CCR1 to buf.
A LED is driven synchronously in the interrupt routine for visualization.

## Hardware configuration

Connect pin PC0 to PA8

Generate PWM:
PWMA2 Channel 0 (pins PC0, PC1)

PWM input mode
PWMA0 Channel 0 and Channel 1 (pin PA8)
