"""socketTest_x_x.py
script that setsup communication with the arduino though ethernet
"""

__author__ = "Nima Ghods (nima.ghods@ga.com)"
__version__ = "$Revision: 4.2 $"
__date__ = "$Date: 2014/05/10$"
__copyright__ = "Copyright (c) 2012 General Atomics"
__license__ = "Python"

from BMUcom import *

def setupCharge(ii):
        sendData(chargerComm[ii],"*IDN \n")
        time.sleep(waitTime)
        reply=getData(chargerComm[ii],ii+4)
        print reply
        sendData(chargerComm[ii],"outp:stat 1\n")
        time.sleep(waitTime)
        sendData(chargerComm[ii],"sour:volt" +str(maxVol)+'\n')
        time.sleep(waitTime)
        pidControl[ii].setPoint(volGoal)
        pidControl[ii].setDerivator(max(chargeVol[2*ii],chargeVol[2*ii+1]))
        
def setChargeCur(ii):
        topCell = max(chargeVol[2*ii],chargeVol[2*ii+1])
        if topCell>cvStart : cur=cMax+slop*(topCell-cvStart)
        else : cur=cMax
##        cur=4+pidControl[ii].update(topCell)
        cur=max(min(cur,cMax),0)
        sendData(chargerComm[0],"sour:curr" +str(cur)+'\n')


if __name__ == '__main__':  
  
  while True:
    if comms[6]==0: connect2mysql(sqlComm)  
    else :  
      for ii in range(0,BMUnum):
        BMUcommand[ii]=getCommand(sqlComm,ii)
        if comms[ii]==0: connect2BMU(ii)
        comms[ii]=sendCommand(BMUcomms[ii],BMUcommand[ii])
        reply=getData(BMUcomms[ii],ii)
        if reply!="" and comms[ii]==1:
          reply=parseData(reply)
          chargeVol[ii]=reply[7]
          comms[6]=setData(sqlComm,reply) 
          if debug: print reply
          #~ if debug: print reply[2]
        else:
			if debug: print "Lost communicaion to BMU %d" %(ii+1)
      for ii in range(0,2):
        if BMUcommand[2*ii]=="charge" and BMUcommand[2*ii+1]=="charge":
                if comms[ii+4]==0:
                        connect2Charger(ii)
                        if comms[ii+4]==1:setupCharger(ii)
                else : setChargeCur(ii)
                                        
    
