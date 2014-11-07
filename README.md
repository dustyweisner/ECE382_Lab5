ECE382_Lab5
===========

IR remote control

__REMOTE USED: iLIVE model: ITP280B (DFEC #1)__

__*Day 1*__

The time that it takes the timer to roll over is 16ms, and each counter lasts 1 us (8MHz/8).

|Pulse|Duration-logic analayzer (ms)|Timer A counts avg|Timer A counts st dev|
|:--|:--|:--|:--|
|Start logic 0 half-pulse|9.01|8937|0|
|Start logic 1 half-pulse|4.45|4411|0|
|Data 1 logic 0 half-pulse|0.601|595.6875|22.32104762|
|Data 1 logic 1 half-pulse|1.67|1626.125|22.51184873|
|Data 0 logic 0 half-pulse|0.599|588.5|20.33715811|
|Data 0 logic 1 half-pulse|0.597|521.8125|14.41396892|
|Stop logic 0 half-pulse|0.599|595|0|
|Stop logic 1 half-pulse|39.4|39445|0|

I collected and tabulated in Excel 8 samples of timer A counts for each of the following pulse types (in decimal): Data 1, logic 1 half-pulse; Data 0, logic 0 half-pulse; Data 0, logic 1 half-pulse. Then I computed the average and standard deviation of each pulse type. I would suggest just grabbing it from the CCS variables tab.

Then I wrote the codes (in hex) for several remote control buttons.

|Button|code (not including start and stop bits)|
|:--|:--|
|0|0x00FF50AF|	
|1|0x00FF00FF|
|2|0x00FF807F|
|3|0x00FF40BF|
|Power|0x00FF30CF|	
|VOL +|0x00FF42BD|
|VOL -|0x00FFC23C|
|CH +|0x00FF08F7|
|CH -|0x00FF8876|	


