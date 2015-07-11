#!/usr/bin/python

# to run this script we need to get the Adafruit BMP085 and the Adafruit 7Segment modules

import time
import datetime
import Adafruit_BMP.BMP085 as BMP085

from Adafruit_7Segment import SevenSegment

segment = SevenSegment(address=0x70)

print "Press CTRL+Z to exit"

sensor = BMP085.BMP085()

presure=sensor.read_pressure()/100;


# Continually update the time on a 4 char, 7-segment display
while(True):
  presure=sensor.read_pressure()/100;
  segment.writeDigit(0, int(str(presure)[0]))     
  segment.writeDigit(1, int(str(presure)[1]))          
  segment.writeDigit(3, int(str(presure)[2]))  
  segment.writeDigit(4, int(str(presure)[3]))        
  #segment.setColon(second % 2)            
  # Wait one second
  time.sleep(1)

