ECE382_Lab5
===========

IR remote control

__REMOTE USED: iLIVE model: ITP280B (DFEC #1)__

__*Day 1*__

I thought that each timer would take 1 us, but we must be using b2 because I measured the time that it takes the timer to roll over to be 1/8 us.

|Pulse|Duration (ms)|Timer A counts|
|:--|:--|:--|
|Start logic 0 half-pulse|9.01||	
|Start logic 1 half-pulse|4.45||	
|Data 1 logic 0 half-pulse|0.597||	
|Data 1 logic 1 half-pulse|0.601||		
|Data 0 logic 0 half-pulse|0.599||		
|Data 0 logic 1 half-pulse|1.667||		
|Stop logic 0 half-pulse|39.4||
|Stop logic 1 half-pulse|21.9||

I then collected and tabulated in Excel 8 samples of timer A counts for each of the following pulse types (in decimal): Data 1, logic 1 half-pulse; Data 0, logic 0 half-pulse; Data 0, logic 1 half-pulse. Then I computed the average and standard deviation of each pulse type. I would suggest just grabbing it from the CCS variables tab.
 
Then I wrote the codes (in hex) for several remote control buttons.
|Button|code (not including start and stop bits)|
|:--|:--|
|0||	
|1||
|2||
|3||
|Power||	
|VOL +||
|VOL -||
|CH +||
|CH -||	


