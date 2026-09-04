# Description

The example demonstrates usage of nested interrupts.
CLIC interrupts from 4 TIM timer channels are configured at different levels.

Channel configuration:
TIM_CH0: period = 200ms, interrupt level = 4, interrupt priority = 0
TIM_CH1: period = 500ms, interrupt level = 3, interrupt priority = 0
TIM_CH2: period = 1s, interrupt level = 2, interrupt priority = 0
TIM_CH3: period = 5s, interrupt level = 1, interrupt priority = 0

Thus, the interrupt from a channel with the lower number can preempt the interrupt
from a channel with the higher number. For the demonstration purpose it is made so,
that the interrupt of the lower priority takes more processing time.

The nested interrupts are supported by usage of 2 macros - ENTER_NESTED_INTERRUPT()
and EXIT_NESTED_INTERRUPT(). The former saves the values of mcause and mepc
registers to stack and re-enables interrupts globally in mstatus register, while
the latter disables interrupts and restores the values of the registers from stack.

The values of mcause and mepc registers reported before they are saved to stack
and after they are restored from stack, so they could be checked for equality.