# Description

This example is a USB benchmark. It is used to test the maximal speed of USB
bulk endpoints. The benchmark is unidirectional, it only tests IN endpoint (Tx).
Except the first received data packet, any USB stack processing is excluded,
the outgoing data packets is transmitted at the maximal possible rate. The CDC
is used as a base class in this example, but its only purpose is to ensure proper
enumeration by the host side. The application will not transmit any data until
the first packet is received, it is made so to give the user possibility to
prepare test on the host side.

The Linux host and dd utility may be used to perform test. In this case the test
commands would be:

1. Configure the serial port to raw mode and disables local echo:

```
stty -F /dev/ttyACM0 raw -echo
```

2. Connect to the device and start test by sending a data packet:

```
echo -ne "#" > /dev/ttyACM0
```

3. Measure USB speed:

```
dd if=/dev/ttyACM0 of=/dev/null status=progress
```

The ttyACM0 must be replaced with the actual device name.
