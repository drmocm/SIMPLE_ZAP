# SIMPLE_ZAP

simple_zap is a DVB (S/S2/C for now) tuning program, where you can (or have to)
set all the parameters by hand, i.e.

`./simple_zap -f 12188000 -s 27500000 -p h -U 2 -j 1 1210 -i 2 1420000 -l 1170000 9750000 10600000`

Use `./simple_zap -h` to see the following help information:

	TUNING OPTIONS:
		-d delsys    : the delivery system (default DVB-S2)
		-a adapter   : the number n of the DVB adapter, i.e. 
		               /dev/dvb/adapter[n] (default=0)
        -e frontend  : the frontend/dmx/dvr to be used (default=0)
		-f frequency : center frequency in kHz
		-i input     : the physical input of the SX8 (default=0)
		-I id        : set id (do not use if you don't know)
		-l ls l1 l2  : set lofs lof1 lof2 
                 	      (default 11700000 9750000 10600000)
	    	-L n         : diseqc switch to LNB/SAT number n (default 0)
		-p pol       : polarisation 0=vertical 1=horizontal
		               (must be set for any diseqc command to be send)
	    	-s rate      : the symbol rate in Symbols/s
		-u           : use hi band of LNB
		-D           : use 1s delay to wait for LNB power up
		-U type      : lnb is unicable type (1: EN 50494, 2: TS 50607
	        	       3: Inverto LNB of type 2 with 32 pre-programmed slots)
	    	-j slot      : slot s ( default slot 1)
	    	-J freq      : freq (default 1210 MHz)

	OTHERS:
		-O           : write full TS to stdout or filename
	    	-o filename  : output filename
	    	-h           : this help message

Example:

1) Tuning a unicable Ku-band LNB with slot 1 at 1210 on input 2 of an 
   SX8 Max card as adapter 0  piped into vlc:

	./simple_zap -f 12188000 -s 27500000 -p h -U 2 -j 1 -J 1210 -i 2 1420000 -O|vlc -

2) Tuning a cable channel on a DVB-C receiver card as afapter 1
  
	./simple_zap -a 1 -d C -f 394000 -s 6900000 -O | vlc -
