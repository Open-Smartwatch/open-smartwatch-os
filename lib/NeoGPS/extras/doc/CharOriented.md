Character-oriented methods
===========================

Sometimes, individual characters or sentences must be processed or filtered, long before a fix structure is completed (i.e., `available()`).  In this advanced technique, your sketch should read each character and pass it to the character-oriented method `gps.decode()`:
```
void loop()
{
  while (serial.available()) {
    char c = serial.read();
    if (gps.decode( c ) == DECODE_COMPLETED) {
      ... do something with gps.fix()...
  }
```
As `gps` decodes those bytes, it will gradually fill out the pieces of its internal fix structure, `gps.fix()` (members described [here](Data%20Model.md).  When you want to use some of the fix data, you can access it like this:
```
  Serial.print( gps.fix().latitude() );
  Serial.print( ',' );
  Serial.println( gps.fix().longitude() );
```
However, you must wait for the sentence to be completely decoded.  You can't access `gps.fix()` unless you know that it is COMPLETED.  You must copy it to another fix variable if you need to access it at any time.

**IMPORTANT:** `gps.fix()` **IS ONLY VALID WHEN:**
  - `gps.decode()` just returned `DECODE_COMPLETED`, or
  - `gps.is_safe()`

This is because `gps.fix().speed` may be half-formed.  You must either do all your accessing immediately after `gps.decode()` returns `DECODE_COMPLETED`:
```
void loop()
{
  // At this point of the code, speed could be half-decoded.
  if (gps.fix().speed <= 5)  // NOT A GOOD IDEA!
    Serial.println( F("Too slow!") );

  while (serial.available()) {
    char c = serial.read();
    if (gps.decode( serial.read() ) == NMEAGPS::DECODE_COMPLETED) {
    
      // Access any piece of gps.fix() in here...
    
      if (gps.fix().speed <= 5)  // OK!
        Serial.println( F("Too slow!") );
    
      if (gps.fix().lat ...
    }
  }
  
```
Or you must call `gps.is_safe()` before using `gps.fix()`.  It is safest to copy `gps.fix()` into your own variable for use at any time:
```
gps_fix my_fix;

void loop()
{
  while (serial.available()) {
    char c = serial.read();
    if (gps.decode( serial.read() ) == NMEAGPS::DECODE_COMPLETED) {
      my_fix = gps.fix(); // save for later...
    }
  }
  
  if (my_fix.speed <= 5)  // OK
    DigitalWrite( UNDERSPEED_INDICATOR, HIGH );
```
Although the character-oriented program structure gives you a finer granularity of control, you must be more careful when accessing `gps.fix()`.
