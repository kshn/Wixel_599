function packetChannelTest2( SCOM1, SCOM2, RCOM )
    % Perform basic testing with the wixels
    % Need to have sending program on two wixels, receiving on one
    
    global output;
    output = [];
    global switched;
    switched = 0;
    
    global s1;
    global s2;
    
    % Open the serial connections
    s1 = serial(SCOM1);
    s2 = serial(SCOM2);
    r = serial(RCOM,'BaudRate',96000);
    fopen(s1);
    fopen(s2);
    fopen(r);
    
    s1.ReadAsyncMode = 'manual';
    s2.ReadAsyncMode = 'manual';
    r.ReadAsyncMode = 'continuous';
    
    r.BytesAvailableFcnMode = 'terminator';
    r.BytesAvailableFcn = {@processData};
    
    % Send packets continously
    %fprintf(r,'c');
    fprintf(s1,'s');
    fprintf(s2,'s');
    fprintf(r,'c'); 
    
    % Close the connections for rhe senders
    fclose(s1);
    fclose(s2);

    % Close the connections for the receiver
    fclose(r);
end


function processData(obj, event)
    global output;
    global s1;
    global s2;
    global switched;
    
    if(size(output,1) < 500)
        temp = fscanf(obj);
        C = textscan(temp,'%u32%s','Delimiter',',');
        output(size(output,1) + 1,:) = [C{1}, hex2dec(C{2})];
        save('packetChannelTest2Output.mat', 'output');
    end
    
    if(size(output,1) >= 250 && switched == 0)
        % Change channel receiving on
        %stopasync(obj);
        %fprintf(obj, 'c');
        %obj.ReadAsyncMode = 'continuous';
        switched = 1;
    elseif(size(output,1) >= 200)
        % Stop sending packets
        %fprintf(s1,'o');
        %fprintf(s2,'o');
    end
end