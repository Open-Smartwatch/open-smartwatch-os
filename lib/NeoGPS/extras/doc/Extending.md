Extending NeoGPS
=========
Using features that are unique to your device fall into three categories:

#### 1. Configuring the device with special commands
Many devices allow you to configure which standard messages are emitted, or the rate at which they are emitted.  It may be as simple as sending a proprietary command to the device.  Simply use the NMEAGPS `send` or `send_P` method.

For example, to set the baudrate of the ublox NEO-6M gps device, send it a 
`UBX,41` message:
```
  gps.send_P( F("PUBX,41,1,0007,0003,19200,0") );
```

#### 2. Parsing additional message types
Some devices provide additional messages with extra information, or more efficient groupings.  This will require deriving a class from `NMEAGPS`.  The derived class needs to
* declare a PROGMEM table of the new message types,
* point that table back to the NMEAGPS table
* override the `parseField` method to extract information from each new message type

Please see ubxNMEA.h and .cpp for an example of adding two ublox-proprietary messages.

#### 3. Handling new protocols
Some devices provide additional protocols.  They are frequently binary, which requires 
fewer bytes than NMEA 0183.  Because they can both be transmitted on the same port, it is 
very likely that they can be distinguished at the message framing level.

For example, NMEA messages always start with a '$' and end with CR/LF.  ublox messages start 
with 0xB5 and 0x62 bytes, a message class and id, and a 2-byte message length.  There is no 
terminating character; the message completed when `length` bytes have been received.

This will require deriving a class from `NMEAGPS`.  The derived class needs 
to 
* define new `rxState` values for the protocol state machine.  These should 
be unique from the NMEA state values, but they should share the IDLE state 
value.
* override the `decode` method to watch for its messages.  As bytes are 
received, it may transition out of the IDLE state and into its own set of 
state values.  If the character is not valid for this protocol, it should 
delegate it to the NMEAGPS base clase, which may begin processing an NMEAGPS 
message.  If the rxState is not one of the derived states (i.e., it is in 
one of the NMEAGPS states), the character should be delegated to 
NMEAGPS::decode.
* implement something like the `parseField` method if parse-in-place 
behavior is desirable.  This is not necessarily `virtual`, as it will only 
be called from the derived `decode`.
* You are free to add methods and data members as required for handling the 
protocol.  Only `decode` must be overridden.

Please see ubxGPS.h and .cpp for an example of implementing the 
ublox-proprietary protocol, UBX.  The derived `ubloxGPS` class provides both 
parse-in-place *and* buffered messages.  See the `send` and `poll` methods.

