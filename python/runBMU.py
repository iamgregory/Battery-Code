"""socketTest_x_x.py
script that setsup communication with the arduino though ethernet
"""

__author__ = "Nima Ghods (nima.ghods@ga.com)"
__version__ = "$Revision: 4.2 $"
__date__ = "$Date: 2014/05/10$"
__copyright__ = "Copyright (c) 2012 General Atomics"
__license__ = "Python"

from subFunc import *

def connect2Charger(ii):
  """ connects to a Charger """
  try:
    chargerComm[ii]=setupComm(charger_host+str(5+ii),charger_port+ii) 
    comms[ii+4]=1
    if debug: print "connected to charger #%d" % (ii+1)
  except:
    comms[ii+4]=0
    if debug: print "not connected to charger #%d" % (ii+1) 

def setupCharger(ii):
        #~ os.system("bash ChargerCon.sh|telnet")
        #~ sendCommand(chargerComm[ii],"*CLS \n")
        #~ time.sleep(waitTime)
        sendCommand(chargerComm[ii],"*IDN? \n")
        time.sleep(waitTime)
        reply=getData(chargerComm[ii],ii+4)
        #~ print comms[6]
        #~ print "this is " + str(len(reply))
        comms[ii+4]=sendCommand(chargerComm[ii],"outp:stat 1\n")
        time.sleep(waitTime)
        comms[ii+4]=sendCommand(chargerComm[ii],"sour:volt:prot 365\n")
        time.sleep(waitTime)
        comms[ii+4]=sendCommand(chargerComm[ii],"sour:volt " +str(maxVol)+'\n')
        #~ time.sleep(waitTime)
        #~ comms[ii+4]=sendCommand(chargerComm[0],"sour:curr 2\n")
        #~ print comms[6]
        if  len(reply)<3:comms[ii+4]=0
        pidControl[ii].setPoint(volGoal)
        pidControl[ii].setDerivator(volGoal-max(chargeVol[2*ii],chargeVol[2*ii+1]))
        
def setChargeCur(ii):
        comms[ii+4]=sendCommand(chargerComm[ii],"meas:curr? \n")
        time.sleep(waitTime)
        #~ print comms[4]
        reply=getData(chargerComm[ii],ii+4)
        #~ print reply
        topCell = max(chargeVol[2*ii],chargeVol[2*ii+1])
        #~ if topCell>cvStart : cur=cMax+slop*(topCell-cvStart)
        #~ else : cur=cMax
        cur=2+pidControl[ii].update(topCell)
        #~ print [cur, round(volGoal-topCell,4), float(reply.strip())] 
        cur=round(max(min(cur,cMax),0),2)
        #~ if (abs(volGoal-topCell)<0.01 and cur<2.25) or chargeDone[ii]:
			#~ cur=0.0
			#~ chargerComm[ii].close()
			#~ comms[ii+4]=0
			#~ chargeDone[ii]=1
        #~ print [cur, round(volGoal-topCell,4), float(reply.strip()), round(pidControl[ii].getIntegrator(),3)] 
        #~ print "sour:curr " +str(cur)+'\n'
        comms[ii+4]=sendCommand(chargerComm[ii],"sour:curr " +str(cur)+'\n')
        if  len(reply)<2: comms[ii+4]=0
        print comms[4]
        return cur


if __name__ == '__main__':  
  
  while True:
    #~ print comms[4]
    if comms[6]==0: connect2mysql(sqlComm)  
    else :  
      for ii in range(0,BMUnum):
        BMUcommand[ii]=getCommand(sqlComm,ii)
        #~ print comms[6]
        if comms[ii]==0: connect2BMU(ii)
        #~ BMUcommand[ii]="charge_39000_"
        #~ BMUcommand[ii]="stop"
        comms[ii]=sendCommand(BMUcomms[ii],BMUcommand[ii])
        reply=getData(BMUcomms[ii],ii)
        #~ BMUcommand[ii]="charge_40000_"
        if len(reply)>=2 and comms[ii]==1:
          reply=parseData(reply)
          chargeVol[ii]=reply[7]
          #~ print comms[6]
          comms[6]=setData(sqlComm,reply) 
          #~ if debug: print reply
          #~ if debug: print reply[2]
        else:
			if debug: print "Lost communicaion to BMU %d" %(ii+1)
      for ii in range(0,1):
        if 'cha' in BMUcommand[2*ii]:
			#~ print comms[6]
			#~ BMUcommand[ii]="charge_40000_"
			if comms[ii+4]==0:
					#~ print comms[6]
					os.system("bash ChargerCon.sh|telnet")
					#~ time.sleep(1)
					connect2Charger(ii)
					#~ setupCharger(ii)
					#~ os.system("bash ChargerCon.sh|telnet")
					#~ time.sleep(1)
					#~ connect2Charger(ii)
					if comms[ii+4]==1:setupCharger(ii)
			else :
				commandTemp=BMUcommand[2*ii].split('_')
				#~ volGoal=3.9
				if volGoal!=float(commandTemp[1])/10000:
					volGoal=float(commandTemp[1])/10000
					pidControl[ii].setPoint(volGoal)
					if debug: print volGoal
				setChargeCur(ii)
