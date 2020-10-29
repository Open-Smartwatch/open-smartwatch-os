RAM requirements
=======

#### **NeoGPS** requires **72% to 96% _less_ RAM, saving 140 to 1100 bytes.**

Because you can select what data members are stored, the RAM savings depends on the [configuration](Configurations.md):

<table>
<tr><td>Configuration</td><td>NeoGPS<br>Size</td><td>TinyGPS<br>Size (% smaller)</td><td>TinyGPS++<br>Size (% smaller)</td><td>Adafruit_GPS<br>Size (% smaller)</td></tr>
<tr><td>Minimal</td><td>10</td><td>-    (95%)</td><td>-    (96%)</td></tr>
<tr><td>DTL</td><td>25</td><td>-    (86%)</td><td>-    (90%)</td></tr>
<tr><td>Nominal</td><td>41</td><td>180 (72%)</td><td>240 (83%)</td><td>326 (87%)</td></tr>
<tr><td>Full</td><td>242</td><td>- (-)</td><td>~1400 (83%)</td></tr>
</table>

#### Why does **NeoGPS** use less RAM?

As data is received from the device, various portions of a `fix` are 
modified.  In fact, _**no buffering RAM is required**_.  Each character 
affects the internal state machine and may also contribute to a data member 
(e.g., latitude). 

If your application only requires an accurate one pulse-per-second, you 
can configure it to parse *no* sentence types and retain *no* data members. 
This is the **Minimal** configuration.  Although the 
`fix().status` can be checked, no valid flags are available.  Even 
though no sentences are parsed and no data members are stored, the 
application will  still receive an empty `fix` once per second:

```
while (gps.available( gpsPort )) {
  gps_fix nothingButStatus = gps.read();
  sentenceReceived();
}
```

The `ubloxNMEA` derived class doesn't use any extra bytes of RAM.

The `ubloxGPS` derived class adds 20 bytes to handle the more-complicated protocol, 
plus 5 static bytes for converting GPS time and Time Of Week to UTC.

