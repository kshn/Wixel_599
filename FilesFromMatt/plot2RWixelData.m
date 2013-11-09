function plot2RWixelData(p,p2)
    % Display Data
    figure(1);
    hold on;
    
    st = p(1,2);
    p(:,2) = p(:,2) - st;               % Adjust time to starting time value
    p2(:,2) = p2(:,2) - st;             % Adjust time to starting time value
    
    plot(p(:,2),p(:,1));                % Plot Sequence Number vs. Time
    plot(p2(:,2),p2(:,1),'y');          % Plot Sequence Number vs. Time
    
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
    lostp = [];
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
end