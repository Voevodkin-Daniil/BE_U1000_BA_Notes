# Description

Advanced timer usage example.
Configures the timer channel in Output Compare mode to generate complementary output PWM signal with 1s period and various duty cycle.
The values of duty cycle are in the buffer BurstDmaBuffer[] and are equal to: 10%, 15%, 20%, 25%, 30%, 40%, 50%, 60%.
DMAR is used for burst accesses.
In this example the timer DMA burst feature is used to update the contents of the CCRx
registers (x = 0, 1, 2, 3) with the DMA transferring words into the CCRx registers.

This is done in the following steps:
1. Configure the corresponding DMA channel as follows:
    – DMA channel peripheral address is the DMAR register address
    – DMA channel memory address is the address of the buffer in the RAM containing
      the data to be transferred by DMA into CCRx registers.
    – Number of data to transfer = 4.
    – Circular mode disabled.

2.  Configure the DCR register by configuring the DBA and DBL bit fields as follows:
    DBL = 4 transfers, DBA = 0xD (CCR0 register).

3.  Enable the PWMA update DMA request (set the UDE bit in the DIER register)

4.  Enable PWMA

5.  Enable the DMA channel

## Hardware configuration

PWMA2 Channel 0,1,2,3 (PC0, PC2, PC4 and PC11 pins)
