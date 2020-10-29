Coherency
==========
Coherency guarantees that all members of a fix are from the same GPS time.  For example, lat/long members may have been set by the newest sentence, but the altitude may be from the previous time interval.  Most applications do not care that the fix members are not coherent.  However, if you are controlling a drone or other autonomous vehicle, you may need coherency.

NeoGPS achieves coherency by detecting the "quiet" time between batches of sentences.   When new data starts coming in, the fix will get emptied or initialized, and all new sentences will be accumulated in the internal fix.

If coherency is desired, **you must choose the correct LAST_SENTENCE_IN_INTERVAL.**  If you're not sure which sentence is sent last (and therefore, when the quiet time begins), use NMEAorder.ino to analyze your GPS device.

You must also use **EXPLICIT_MERGING**.  Implicit merging cannot be used with coherency is because a sentence has to be parsed to know its timestamp.  If it were implicitly merged, the old data would not have been invalidated.  Invalidating data from a previous update period must be performed _before_ the sentence parsing begins.  That can only be accomplished with a second 'safe' copy of the fix data and explicit merging (i.e., FIX_MAX >= 1).  With implicit merging, new data has already been mixed with old data by the time DECODE_COMPLETED occurs and timestamps can be checked.

When you have correctly chosen the LAST_SENTENCE_IN_INTERVAL *and* EXPLICIT_MERGING, the fix-oriented methods `available` and `read()` will return a coherent fix.

NOTE: If you use the [character-oriented methods](CharOriented.md) `decode`, `is_safe` and `fix()` to handle individual sentences, you must check `intervalComplete()` to know when the GPS update interval is completed, and the GPS quiet time has started.
