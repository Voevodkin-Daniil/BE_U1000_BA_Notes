# Description

This example is a USB benchmark. It is used to test the maximal speed of USB
bulk endpoints. The benchmark is unidirectional, it only tests OUT endpoint (Rx).
Any USB stack processing is excluded, the incoming data packets is received
at the maximal possible rate. The CDC is used as a base class in this example,
but its only purpose is to ensure proper enumeration by the host side.

The Linux host and dd utility may be used to perform test. In this case the test
command would be:

```
dd if=/dev/zero of=/dev/ttyACM0 status=progress
```

The ttyACM0 must be replaced with the actual device name.
