
import socket
import time

def setupComm(host,port):
  """ setup communication with the BMU and return server """
  result = False
  try:
    result=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    result.settimeout(1)
    result.connect((host,port))
    print 'connected'
    try: 
		result.bind((host,port))
		print 'Bind worked'
		result.listen(10)
		print 'Listening'
    #~ except:
		#~ print "here"
    except socket.error,msg:
		print 'Bind failed Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
  except:
    print 'Failed to create socket'
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
  
def getData(_s):
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
	condition=0
    return reply
  
if __name__ == '__main__':
	host="192.168.0.196"  
	port=9222
	chargerComm=setupComm(host,port)
	sendCommand(chargerComm,"*IDN? \n")
	time.sleep(0.2)
	reply=getData(chargerComm)
	print reply
