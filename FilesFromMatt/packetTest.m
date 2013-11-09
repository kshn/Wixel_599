function [p, p2] = packetTest( SCOM, RCOM, RCOM2 )
    % Perform basic testing with the wixels
    
    % Open the serial connections
    s = serial(SCOM);
    s.ReadAsyncMode = 'manual';
    r = serial(RCOM,'BaudRate',96000);
    r2 = serial(RCOM2,'BaudRate',96000);
    fopen(s);
    fopen(r);
    fopen(r2);
    
    % Send packets
    fprintf(s,'d');
    
    % Receive packets
    looping = 1;
    looping2 = 1;
    counter = 1;
    counter2 = 1;
    while(looping == 1 || looping2 == 1)
        
        if(looping == 1)
            temp = fscanf(r);
            if (r.ValuesReceived == 0 || isempty(temp))
                looping = 0;
            else
                C = textscan(temp,'%u32%s','Delimiter',',');
                p(counter,:) = [C{1}, hex2dec(C{2})];
                counter = counter + 1;
            end

            if (r.BytesAvailable == 0 && p(counter-1,1) == 999)
                looping = 0;
            end
        end
        
        
        if(looping2 == 1)
            temp2 = fscanf(r2);
            if (r2.ValuesReceived == 0 || isempty(temp2))
                looping2 = 0;
            else
                C2 = textscan(temp2,'%u32%s','Delimiter',',');
                p2(counter2,:) = [C2{1}, hex2dec(C2{2})];
                counter2 = counter2 + 1;
            end

            if (r2.BytesAvailable == 0 && p2(counter2-1,1) == 999)
                looping2 = 0;
            end
        end
    end
    
    if(size(p,1) < 1)
        p = [0,0];
    end
    if(size(p2,1) < 1)
       p2 = [0,0];
    end
    
    % Close the serial connections
    fclose(s);
    fclose(r);
    fclose(r2);
    
    % Display Data
    figure(1);
    hold on;
    
    p(:,2) = p(:,2) - p(1,2);       % Adjust time to starting time value
    p2(:,2) = p2(:,2) - p2(1,2);    % Adjust time to starting time value
    
    plot(p(:,2),p(:,1));            % Plot Sequence Number vs. Time
    plot(p2(:,2),p2(:,1));          % Plot Sequence Number vs. Time
    
    xlabel('Time');
    ylabel('Sequence Number');
    
    disp(['Packets Lost: ',num2str(1000-size(p,1)),' (', num2str(100*(1000-size(p,1))/1000),'%)']);
    disp(['Packets Lost2: ',num2str(1000-size(p2,1)),' (', num2str(100*(1000-size(p2,1))/1000),'%)']);
    
    SeqNums = 0:999;
    missingvalues = setdiff(SeqNums,p(:,1));
    for x=1:size(missingvalues,2)
        c=1;
        ind = [];
        while(isempty(ind))
            ind = find(p(:,1) == missingvalues(x) + c);
            c = c+1;
            if(missingvalues(x) + c > 999) 
                ind = size(p(:,1));
            end
        end
        upbound = ind(1);
        while(isempty(ind))
            ind = find(p(:,1) == missingvalues(x) - c);
            c = c+1;
            if(missingvalues(x) - c < 0) 
                ind = 1;
            end
        end
        lowbound = ind(1);
        t = (p(lowbound,2) + p(upbound,2))/2;
        lostp(x,:) = [missingvalues(x),t];
    end
    plot(lostp(:,2),lostp(:,1),'x','MarkerEdgeColor','r','MarkerFaceColor','r');
    
    % Do the same for the second receiver
    SeqNums = 0:999;
    missingvalues = setdiff(SeqNums,p2(:,1));
    for x=1:size(missingvalues,2)
        c=1;
        ind = [];
        while(isempty(ind))
            ind = find(p2(:,1) == missingvalues(x) + c);
            c = c+1;
            if(missingvalues(x) + c > 999) 
                ind = size(p2(:,1));
            end
        end
        upbound = ind(1);
        while(isempty(ind))
            ind = find(p2(:,1) == missingvalues(x) - c);
            c = c+1;
            if(missingvalues(x) - c < 0) 
                ind = 1;
            end
        end
        lowbound = ind(1);
        t = (p2(lowbound,2) + p2(upbound,2))/2;
        lostp(x,:) = [missingvalues(x),t];
    end
    plot(lostp(:,2),lostp(:,1),'x','MarkerEdgeColor','g','MarkerFaceColor','g');
    
    % Create bit rate plot
    counter = 1;
    for x=1:size(p(:,2))
        if(x > 1)
            if(p(x,2) ~= p(x-1,2))
                b(counter,:) = [18*x, p(x,2)];
                counter = counter + 1;
            end
        else
            if(p(2,2) ~= p(1,2))
                b(counter,:) = [18*x, p(x,2)];
                counter = counter + 1;
            end
        end
    end
    figure(2);
    hold on;
    plot(b(:,2),b(:,1));          % Plot Bytes vs. Time
    xlabel('Time');
    ylabel('Bytes');
end