
import datetime
import math
from PID import *
import socket
import time
import MySQLdb as mdb
import os
  
debug=True
BMUcomms=['BMU1','BMU2','BMU3','BMU4']
BMUcommand=['stop','stop','stop','stop']
chargerComm=['charger1','charger2']
sqlComm=['sqlcon','sqlcur']
comms=[0,0,0,0,0,0,0]
BMUnum=1
host= "192.168.0.17"
port= 40
charger_host="192.168.0.19"
charger_port=9221
chargeVol=[0,0,0,0]
waitTime=0.2
maxVol=180#360
cMax=44.5
cStop=4.0
pidControl=['pid1','pid2']
Kp=60
Kd=-150
Ki=80
if Ki==0: iMax=cMax-2
else: iMax=(cMax-2)/Ki
iMin=-1000
pidControl[0]=PID(Kp,Ki,Kd,0,0,iMax,iMin)
pidControl[1]=PID(Kp,Ki,Kd,0,0,iMax,iMin)
cvStart=4.15
volGoal=4.2
slop=(cStop-cMax)/(volGoal-cvStart)
chargeDone=[1,1]

def setData(_sqlComm,bmuData):
  """ send BMU ii Data to mysql database """
  dataout=",".join(str(i) for i in bmuData)
  timestr=str(datetime.datetime.now())
  query="INSERT INTO arduino_data%d (arduino_data%d.values,time) VALUES('%s','%s')" %(bmuData[2], bmuData[2], dataout,  timestr)
  #print query
  try:
    _sqlComm[1].execute(query)
    _sqlComm[0].commit()
    result = 1
  except:
    result=0
  return result

def getCommand(_sqlComm,n):
  """ gets BMU ii command from mysql database """
  query="SELECT arduino_command%d.values FROM arduino_command%d ORDER BY id DESC LIMIT 1" %((n+1),(n+1))
  try:
    _sqlComm[1].execute(query)
    result = _sqlComm[1].fetchone()
    result =result[0] 
  except:
    result="stop"
    comms[6]=0
  return result

def connect2mysql(_sqlComm):
  """ connects to a mysql """
  try:
    _sqlComm[0]=setupMysql()
    comms[6]=1
    _sqlComm[1]=_sqlComm[0].cursor()
    if debug: print "connected to database" 
  except:
    comms[6]=0
    if debug: print "not connected to database" 

def connect2BMU(ii):
  """ connects to a BMU """
  try:
    BMUcomms[ii]=setupComm(host+str(ii),port+ii)  
    comms[ii]=1
    if debug: print "connected to BMU #%d" % (ii+1)
  except:
    comms[ii]=0
    if debug: print "not connected to BMU #%d" % (ii+1)
    
  
def setupMysql():
  result = False
  try:
    result=mdb.connect('localhost','userbme','Bme135','bme')
  except:
    result = False
  return result
  
def setupComm(host,port):
  """ setup communication with the BMU and return server """
  result = False
  try:
    result=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    result.settimeout(1)
    result.connect((host,port))
  except:
    result=False
  return result
  
def sendCommand(_s,_c): 
  """
  sets the sepcified data (writes the specified data into database).
  Please note that the data must be string of value sperated by comma.
  """
  try:
    _s.send(_c+'\n')
    result = 1
  except:
    result = 0
  return result
  
#~ def BMUclose(_s):
  #~ """ closes the socket """
  #~ _s.close()

def getData(_s,ii):
    """ gets the header from BMU and time stamps it """
    condition=1
    reply=""
    while condition:
      try:
	c= _s.recv(1000)
	reply+=c
	if '\n' in c or c=="":
	  condition=0
      except:
	comms[ii]=0
	condition=0
    return reply
    
def parseData(v):
    """ parses the data in to a dictionary"""
    startInx=11
    BMEnum=14
    value=v.rstrip('\n')
    value=value.split(",")
    value[0]=int(value[0],16)
    for i in range(startInx,startInx+BMEnum*10):
        value[i]=int(value[i],16)
    value=[float(i) for i in value]
    #convert dtime from uS to S
    value[1]=value[1]/1000000
    #convert cell voltage
    for i in range(startInx,startInx+BMEnum*3):
      value[i]=value[i]*0.0001
    #convert module voltage
    for i in range(startInx+BMEnum*3,startInx+BMEnum*4):
      value[i]=value[i]*0.002
    #convert referenc 2 voltage
    for i in range(startInx+BMEnum*4,startInx+BMEnum*5):
      value[i]=value[i]*0.0001
    #convert auxiliary temperature
    Binv= .00023866  # 1/ B-value where B-value is 4190 K
    T0inv= 0.003354  # 1/T_0 where T_0 is 298.15 K
    for i in range(startInx+BMEnum*5,startInx+BMEnum*9):
      value[i]=value[i]*0.0001
      VINidx=((i-startInx-BMEnum*5)%4)+startInx+BMEnum*4
      if value[i]<=0 or value[i]>=value[VINidx]:
	value[i]=-42
      else:
	value[i]=value[i]/(value[VINidx]-value[i])
	value[i]=T0inv + Binv*math.log(value[i])
	value[i]=1.0/value[i]-273
    #convert internal chip temperature
    for i in range(startInx+BMEnum*9,startInx+BMEnum*10):
        value[i]=value[i]/75-273
    value=[round(i,4) for i in value]
    value[0]=int(value[0])
    value[2]=int(value[2])
    return value
