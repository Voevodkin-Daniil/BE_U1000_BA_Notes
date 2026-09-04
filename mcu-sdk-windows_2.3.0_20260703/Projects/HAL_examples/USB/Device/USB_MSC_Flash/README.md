### Description
USB Mass Storage class example.
The external Micron MT25QL128ABA flash chip is used as storage memory.
The communication interface to the external flash is QSPI in Quad mode.
Write operation to storage is buffered. The data remaining in RAM buffer
flushed to non-volatile flash memory on unmount, suspend and 5s after
the last write operation.
