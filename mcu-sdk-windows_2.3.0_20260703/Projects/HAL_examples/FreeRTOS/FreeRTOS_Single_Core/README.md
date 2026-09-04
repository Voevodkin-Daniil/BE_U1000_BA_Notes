# Description
A simple example of using [FreeRTOS](https://www.freertos.org/) on the Baikal MCU. This example has three user tasks (`Tx`, `Rx`, `Sem`) with different priorities.  
Task prioritities:
```C
#define mainQUEUE_RECEIVE_TASK_PRIORITY     (tskIDLE_PRIORITY + 2)
#define mainQUEUE_SEND_TASK_PRIORITY        (tskIDLE_PRIORITY + 1)
#define mainEVENT_SEMAPHORE_TASK_PRIORITY   (configMAX_PRIORITIES - 1)
```
The project demonstrates a way to exchange information between tasks using a queue and the implementation of deferred interrupt processing with synchronization using a binary semaphore.

 The `Sem` task is unblocked every 500 runs of the Mashine Time interrupt handler (every 0.5 seconds). 

Task execution diagram:
```text
"Sem"    +                    ++                    ++
          ↓                  ↑  |                  ↑  |
"Rx"       *    **       **  |  |     **       **  |  |
            ↓  ↑  |     ↑  | |  |    ↑  |     ↑  | |  |
"Tx"         --   |   --   | |  |  --   |   --   | |  |
                  |  ↑     | |  | ↑     |  ↑     | |  |
"Idle"            ↓__|     ↓_|  ↓_|     ↓__|     ↓_|  ↓_
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
Task `Tx` prints character '2', task `Rx` - '1'.
```
FreeRTOS single core example
1212121212...
```

## ROM region
This example is intended to use `TCMA`, `TCMB` or `EFLASH` as ROM memory region.  
Example:
```sh
$ make MEM_REG_ROM=TCMA
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
