function packetChannelTest( SCOM, RCOM, RCOM2 )
    % Perform basic testing with the wixels
    % Need to have sending on one wixel, and receiving on two
    
    global output1;
    global output2;
    output1 = [];
    output2 = [];
    
    % Open the serial connections
    s = serial(SCOM);
    r = serial(RCOM,'BaudRate',96000);
    r2 = serial(RCOM2,'BaudRate',96000);
    fopen(s);
    fopen(r);
    fopen(r2);
    
    s.ReadAsyncMode = 'manual';
    r.ReadAsyncMode = 'continuous';
    r2.ReadAsyncMode = 'continuous';
    
    r.BytesAvailableFcnMode = 'terminator';
    r.BytesAvailableFcn = {@processData, 1};
    r2.BytesAvailableFcnMode = 'terminator';
    r2.BytesAvailableFcn = {@processData, 2};
    
    % Send packets
    fprintf(s,'d');
    
    % Close the connection
    fclose(s);
    
    fclose(r);
    fclose(r2);
end


function processData(obj, event, rNum)
    global output1;
    global output2;
    
    if(rNum == 1)
        temp = fscanf(obj);
        C = textscan(temp,'%u32%s','Delimiter',',');
        output1(size(output1,1) + 1,:) = [C{1}, hex2dec(C{2})];
        save('packetChannelTestOutput.mat', 'output1', 'output2');
    else
        temp = fscanf(obj);
        C = textscan(temp,'%u32%s','Delimiter',',');
        output2(size(output2,1) + 1,:) = [C{1}, hex2dec(C{2})];
        save('packetChannelTestOutput.mat', 'output1','output2');
    end
end