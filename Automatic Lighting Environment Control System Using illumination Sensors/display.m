clc;
clear;
y = 0
timeDisplay = 20
DisplayLength = 3600
delete(instrfindall)
s1 = serial('COM4')
set(s1, 'BaudRate',9600)
fopen(s1);
modestack = 0
mode = {'Cooling','Healing','Care','Sleep','Control'}
labelIndex = 4
Index = 1
tx = 'mode = '
while 1
    for i= Index:DisplayLength
        a = str2num(fscanf(s1))
        if i > timeDisplay 
             t = i - timeDisplay
        else
             t = 0;
        end
        if a <0  
            %tx = strcat(label,',')
            labelIndex = abs(a)
            n = 0
            %clf('reset')
            plot(n,y)
            Index = i
            break
        else    
            y(1,i) = a
            n = linspace(0,length(y)-1,length(y));
            figure(1)
            plot(n,y,'k')
            xlim([0+t,timeDisplay - 1+t])
            ylim([0,1500])
            text = char(mode(labelIndex))
            label = strcat(tx, text)
            xlabel({'Time(s)',label})
            %xlabel({'Time(s)','control Mode'})
            ylabel('illuminance(Lux)')
            hold on
            grid on
            drawnow
        end
    end
        if length(y) == DisplayLength
            break  
        end
    end
fclose(s1)
delete(instrfindall)
