.section .text._start
.globl _start
.type _start, %function

/* Entry point */
_start:
    j Reset_Handler

/* Reset vector */
.section .text
.globl Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
    /* Set stack pointer (SP) to the top of the RAM */
    la      sp, __SP

    /* Initialize global pointer (GP) */
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop

    /* Set interrupt vector mode and vector table address */
    la      t0, mtvec_clint_vector_table
    ori     t0, t0, 0x1
    csrw    mtvec, t0

    /* Initialize data in RAM */
copy_data_init:
    la      t0, __DATA_START__
    la      t1, __DATA_IMAGE_START__
    la      t2, __DATA_END__
    beq     t0, t2, copy_data_end
copy_data_loop:
    lw      t3, (t1)
    sw      t3, (t0)
    add     t1, t1, 4
    add     t0, t0, 4
    blt     t0, t2, copy_data_loop
copy_data_end:

    /* Clear the .bss section */
clear_bss_init:
    la      t0, __BSS_START__
    la      t1, __BSS_END__
clear_bss_loop:
    sw      zero, (t0)
    add     t0, t0, 4
    blt     t0, t1, clear_bss_loop

    /* Call the main function */
call_main:
    li      a0, 0
    li      a1, 0
    jal     main

    /* If main returns, loop infinitely */
    j       .

/* Default ISR handler */
.balign 4, 0
.type Default_Handler, %function
Default_Handler:
    /* Infinite loop */
    j       .

/* ISR vector table */
.section .text.isr_vector,"a",%progbits

.balign 4, 0
.globl mtvec_clint_vector_table
.type mtvec_clint_vector_table, %object
mtvec_clint_vector_table:
    j Exception_Handler             /* Synchronous exceptions handler */
    j Default_Handler               /* Default handler for unused vectors */
    j Default_Handler               /* Default handler for unused vectors */
    j Software_IRQHandler           /* Machine software interrupt handler */
    j Default_Handler               /* Default handler for unused vectors */
    j Default_Handler               /* Default handler for unused vectors */
    j Default_Handler               /* Default handler for unused vectors */
    j Timer_IRQHandler              /* Machine timer interrupt handler */
    j Default_Handler               /* Default handler for unused vectors */
    j Default_Handler               /* Default handler for unused vectors */
    j Default_Handler               /* Default handler for unused vectors */
    j Default_Handler               /* Default handler for unused vectors */
    j Default_Handler               /* Default handler for unused vectors */
    j Default_Handler               /* Default handler for unused vectors */
    j Default_Handler               /* Default handler for unused vectors */
    j Default_Handler               /* Default handler for unused vectors */
    j MB0_IRQHandler                /* Mailbox 0 interrupt handler */
    j MB1_IRQHandler                /* Mailbox 1 interrupt handler */
    j EXTI_PA_IRQHandler            /* EXTI PA interrupt handler */
    j EXTI_PB_IRQHandler            /* EXTI PB interrupt handler */
    j EXTI_PC_IRQHandler            /* EXTI PC interrupt handler */
    j USB_IRQHandler                /* USB interrupt handler */
    j USB_DMA_IRQHandler            /* USB DMA interrupt handler */

.size mtvec_clint_vector_table, . - mtvec_clint_vector_table

/* Provide weak alias for each handler to the Default_Handler */
.weak Exception_Handler
.set Exception_Handler,Default_Handler

.weak Software_IRQHandler
.set Software_IRQHandler,Default_Handler

.weak Timer_IRQHandler
.set Timer_IRQHandler,Default_Handler

.weak MB0_IRQHandler
.set MB0_IRQHandler,Default_Handler

.weak MB1_IRQHandler
.set MB1_IRQHandler,Default_Handler

.weak EXTI_PA_IRQHandler
.set EXTI_PA_IRQHandler,Default_Handler

.weak EXTI_PB_IRQHandler
.set EXTI_PB_IRQHandler,Default_Handler

.weak EXTI_PC_IRQHandler
.set EXTI_PC_IRQHandler,Default_Handler

.weak USB_IRQHandler
.set USB_IRQHandler,Default_Handler

.weak USB_DMA_IRQHandler
.set USB_DMA_IRQHandler,Default_Handler
