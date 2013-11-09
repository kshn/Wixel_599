 packetChannelTest('COM6', 'COM7', 'COM5');
 load('packetChannelTestOutput.mat');
 p1 = sortrows(output1,2);
 p2 = sortrows(output2,2);
 plot2RWixelData(p1,p2);