Tradeoffs
=========

There's a price for everything, hehe...

#### Configurability means that the code is littered with #ifdef sections.

I've tried to increase white space and organization to make it more readable, but let's be honest... 
conditional compilation is ugly.

#### Accumulating parts means knowing which parts are valid.

Before accessing a part, you must check its `valid` flag.  Fortunately, this adds only one bit per member.  See [Streamers.cpp](/src/Streamers.cpp#L100) for an example of accessing every data member.  That file also shows how to accommodate different builds: all references to 'gps_fix' members are wrapped with conditional compilation `#ifdef`/`#endif` statements.  If you do not plan to support multiple configurations, you do not need to use `#ifdef`/`#endif` statements.

#### Parsing without buffers, or *in place*, means that you must be more careful about when you access data items.

In general, using the fix-oriented methods `available` and `read` are atomically safe.  You can access any parts of your `fix` structure, at any time.

If you are using the advanced [character-oriented methods](/extras/doc/CharOriented.md):

*  You must wait to access the internal `gps.fix()` until after the entire sentence has been parsed.
*  Only 3 example programs use these methods: NMEAblink, NMEAorder and NMEAdiagnostic.  These examples simply `decode` until a sentence is COMPLETED.  See `GPSloop()` in [NMEAdiagnostic.ino](/examples/NMEAdiagnostoc/NMEAdiagnostic.ino).
*  Member function `gps.is_safe()` can also be used to determine when it is safe to access the internal `gps.fix()`.
*  Received data errors can cause invalid field values to be set in the internal fix *before* the CRC is fully computed.  The CRC will
catch most of those, and the internal fix members will then be marked as invalid.

#### Accumulating parts into *one* fix means less RAM but more complicated code

By enabling one of the [merging](/extras/doc/Merging.md) methods, fixes will accumulate data from all received sentences.  The code required to implement those different techniques is more complicated than simply setting a structure member.

You are not restricted from having other instances of fix; you can copy or merge a some or all of a new fix into another copy if you want.

#### Full C++ OO implementation is more advanced than most Arduino libraries.

You've been warned!  ;)

#### "fast, good, cheap... pick two."

Although most of the RAM reduction is due to eliminating buffers, some of it is from trading RAM
for additional code (see **Nominal** Program Space above).  And, as I mentioned, the readabilty (i.e., goodness) suffers from its configurability.
