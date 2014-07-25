"""chargerComm_x_x.py
script that setsup communication with the charger though ethernet telnet
"""

__author__ = "Nima Ghods (nima.ghods@ga.com)"
__version__ = "$Revision: 1.0 $"
__date__ = "$Date: 2014/02/22$"
__copyright__ = "Copyright (c) 2012 General Atomics"
__license__ = "Python"

import telnetlib
import time

import os
		
if __name__ == '__main__':
	
    comms=0
    host= "192.168.0.195"
    port= 9221
    waitTime=.000001
    print "Charging test"
	
    #~ subprocess.call(["ChargeCon.sh", domid])
    os.system("bash ChargerCon.sh|telnet")
    #~ while True:
        #~ maxVol=input('give me the voltage') 
    while True:
        if comms:
            try:
                ChargerComms.write("*IDN?\n")
                time.sleep(waitTime)
                reply=ChargerComms.read_until("\r\n",1)
                print reply.strip()
                time.sleep(waitTime)
                #~ ChargerComms.write("*CLS\n")
                #~ time.sleep(10*waitTime)
                
                #~ ChargerComms.write("*RST\n")
                ChargerComms.write("sour:curr 1\n")
                time.sleep(waitTime)
                ChargerComms.write("sour:curr?\n")
                reply=ChargerComms.read_until("\r\n",1)
                print reply.strip()+" A"
                ChargerComms.write("sour:volt 5\n")
                ChargerComms.write("sour:volt?\n")
                reply=ChargerComms.read_until("\r\n",1)
                print reply.strip()+" V"
                ChargerComms.write("meas:curr?\n")
                reply=ChargerComms.read_until("\r\n",1)
                print reply.strip()+" A"
                ChargerComms.write("meas:volt?\n")
                reply=ChargerComms.read_until("\r\n",1)
                print reply.strip()+" V"
                print "done"
                ChargerComms.close()
                while True:
                        reply=1;
                #reply=getData(BMUcomms[ii])
                #print reply
                #setdata(dataout)
            except:
                ChargerComms.close()
                comms=0
                print "lost connected to Charger" 
        else:
            try:
                #~ os.system("telnet 192.168.0.195 9221\n")
                #~ time.sleep(waitTime)
                #~ print "*IDN?\n"
                ChargerComms=telnetlib.Telnet(host,port)
                time.sleep(waitTime)	
                comms=1
                print "connected to Charger" 
            except:
                comms=0
                print "not connected to Charger"
