Performance
===========

#### **NeoGPS** is **40% to 70% faster**.

For comparison, the following sentences were parsed by various [Configurations](/doc/Configurations.md) of **NeoGPS**, **TinyGPS** and **TinyGPSPlus** on a 16MHz Arduino Mega2560.

```
$GPGGA,092725.00,4717.11399,N,00833.91590,E,1,8,1.01,499.6,M,48.0,M,,0*5B
$GPRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091202,,,A*57
$GPGSV,3,1,10,23,38,230,44,29,71,156,47,07,29,116,41,08,09,081,36*7F
$GPGSV,3,2,10,10,07,189,,05,05,220,,09,34,274,42,18,25,309,44*72
$GPGSV,3,3,10,26,82,187,47,28,43,056,46*77
```

<table>
<tr><td>Configuration</td><td>Sentence</td><td>NeoGPS</td><td>TinyGPS<br>Time (% faster)</td><td>TinyGPS++<br>Time (% faster)</td><td>Adafrut_GPS<br>Time (%faster)</td></tr>
<tr><td>Minimal</td><td>GGA<br>RMC</td><td>329us<br>335us</td><td>-    (78%)<br>-    (77%)</td><td>-    (78%)<br>-    (77%)</td></tr>
<tr><td>DTL</td><td>GGA<br>RMC</td><td>780us<br>803us</td><td>-    (46%)<br>-    (44%)</td><td>-    (47%)<br>-    (44%)</td></tr>
<tr><td>Nominal</td><td>GGA<br>RMC</td><td>864us<br>883us</td><td>1448us (40%)<br>1435us (39%)</td><td>1473us (41%)<br>1442us (39%)</td><td>1358us (36%)<br>1535us (42%)</td></tr>
<tr><td>Full</td><td>GGA<br>RMC<br>GSV</td><td>908us<br>899us<br>2194us</td><td>- (37%)<BR>-  (37%)<br>-   (-)</td><td>1523us (40%)<br>1560us (42%)<br>6651us (67%)</td></tr>
</table>

#### Why is **NeoGPS** faster?

Most libraries use extra buffers to accumulate parts of the sentence so they 
can be parsed all at once.  For example, an extra field buffer may hold on 
to all the characters between commas.  That buffer is then parsed into a 
single data item, like `heading`.  Some libraries even hold on to the 
*entire* sentence before attempting to parse it.  In addition to increasing 
the RAM requirements, this requires **extra CPU time** to copy the bytes and 
index through them... again.
 
**NeoGPS** parses each character immediately into the data item.  When the 
delimiting comma is received, the data item has been fully computed *in 
place* and is marked as valid.

Most libraries parse all fields of their selected sentences.  Although most 
people use GPS for obtaining lat/long, some need only time, or even just one 
pulse-per-second.

**NeoGPS** configures each item separately.  Disabled items are 
conditionally compiled, which means they will not use any RAM, program space 
or CPU time.  The characters from those fields are simply skipped; they are 
never copied into a buffer or processed.

While it is significantly faster and smaller than all NMEA parsers, these same improvements also make 
NeoGPS faster and smaller than _binary_ parsers.
