# Description
A example of using [SMP](https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/13-Symmetric-multiprocessing-introduction) (Symmetric Multiprocessing) [FreeRTOS](https://www.freertos.org/) on the Baikal RISC-V MCU with execution on 2 MCU cores. This example has three user tasks (`Tx`, `Rx`, `Sem`) with different priorities. Task `Tx` is allowed to run on MCU core `0`. Tasks `Rx` and `Sem` are allowed to run on MCU core `1`.  
Task prioritities:
```C
#define mainQUEUE_RECEIVE_TASK_PRIORITY     (tskIDLE_PRIORITY + 2)
#define mainQUEUE_SEND_TASK_PRIORITY        (tskIDLE_PRIORITY + 1)
#define mainEVENT_SEMAPHORE_TASK_PRIORITY   (configMAX_PRIORITIES - 1)
```
The project demonstrates task allocation across MCU cores, simple way to exchange information between tasks using a queue and the implementation of deferred interrupt processing with synchronization using a binary semaphore.

The `Sem` task is unblocked every 500 runs of the Mashine Time interrupt handler (every 0.5 seconds).

Task execution diagram:
```text
core 0

"Tx"      --    --    --    --    --    --
         ↑  |  ↑  |  ↑  |  ↑  |  ↑  |  ↑  |
"Idle"   |  ↓__|  ↓__|  ↓__|  ↓__|  ↓__|  ↓__
        ---------------------------------------------------> time

core 1

"Sem"    +              ++              ++
          ↓            ↑  ↓            ↑  ↓
"Rx"       **      **  |   **   **   **    **
             |    ↑  | |     | ↑  | ↑        |
             |    |  | |     | |  | |        |
             |    |  | |     | |  | |        |
"Idle"       ↓____|  ↓_|     ↓_|  ↓_|        ↓___
        ---------------------------------------------------> time

Task "Sem"  - '+'
Task "Rx"   - '*'
Task "Tx"   - '-'
Task "Idle" - '_'
```

The project uses implementation of pvPortMalloc() and vPortFree() that combines
(coalescences) adjacent memory blocks as they are freed, and in so doing
limits memory fragmentation (heap_4.c file).

## Application console output
The `Tx` task print to console symbol '2', task `Rx` - '1'.
```text
FreeRTOS SMP demo example
2121212121...
```

# Hardware configuration
Any evaluation board supported by SDK can be used.  
UART interface (debug output): `UART0`.  
Serial port configuration: 
 - baud rate `115200`
 - data `8-bit`
 - parity `none`
 - stop bits `1-bit`
 - flow control `none`
