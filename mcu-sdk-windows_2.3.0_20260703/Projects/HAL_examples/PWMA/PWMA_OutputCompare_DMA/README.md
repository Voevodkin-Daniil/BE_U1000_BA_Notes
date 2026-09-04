# Description

Advanced timer usage example.
Configures the timer channel in Output Compare mode to generate complementary output PWM signal with 1s period and various duty cycle.
The values of duty cycle are in the buffer pwm_data[] and are equal to: 10%, 20%, 30%, 40%, 50%, 60%, 70%, 80%, 90%, 15%.
DMA is used to set the duty cycle value.
A LED is driven synchronously in the interrupt routine for visualization.

## Hardware configuration

PWMA2 Channel 0 (PC0 and PC1 pins)
