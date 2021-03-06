import serial, time, os
from matplotlib import pyplot as plt
for i in range(0,3):
    f = open('port'+str(i)+'.log','w')
    f.close()
    
def write_log(portNum, text):
    f = open('port'+str(portNum)+'.log','a')
    f.write(text)
    f.close()
    
# Ser0 is the transmitting hub
ser0 = serial.Serial()
# Ser1 is one of the receiving hubs
ser1 = serial.Serial()
# Ser2 is one of the receiving hubs
ser2 = serial.Serial()

ls = os.popen('ls /dev/ttyACM*')
for i in range(0,3):
    port = ls.readline()
    

ser0.port = '/dev/ttyACM5'
ser0.baudrate = 9600
ser0.open()
ser0.flushOutput()
ser0.flushInput()
ser0.timeout = 0.25
out = []


ser1.port = '/dev/ttyACM3'
ser1.baudrate = 9600
ser1.open()
ser1.flushOutput()
ser1.flushInput()
ser1.timeout = 0.25
in1 = []


ser2.port = '/dev/ttyACM4'
ser2.baudrate = 9600
ser2.open()
ser2.flushOutput()
ser2.flushInput()
ser2.timeout = 0.25
in2 = []

#count = 0
t0 = time.time()
max_count = 10
for i in range(1,max_count):
    text0 = i
    ser0.write(text0)
    write_log(0,text0)
    out.append(text0)
#    count+=1
    print i
#    ser0.flush()
    time.sleep(0.1)
    text1 = ser1.readline()
    if text1 =='':
        text1 = '0'
    in1.append(text1)
    write_log(1, text1)
    text2 = ser2.readline()
    in2.append(text2)
    write_log(2, text2)
time_period = time.time() - t0
time_axis = [a * max_count/time_period for a in range(0,max_count)]
plt.plot(time_axis, out,'k.-')
plt.plot(time_axis, in1,'r*-')
plt.plot(time_axis, in2,'bo-')
plt.legend('Transmitted data','Received data 1','Received data 2')
ser0.close()
ser1.close()
ser2.close()
