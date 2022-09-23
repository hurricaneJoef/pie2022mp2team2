% File designed to collect data from the Pan-Tilt Mechanism servos and the
% IR sensor that scans the geometry of an object.


function arduino3DScanner(logfile,doScan)
    spacing = 2;
    function [x,y,z] = get_coords(phi,theta,r)
        phi = 45-phi;
        theta = 45-theta;
        x= (r*cosd(theta)*sind(phi));
        z= r*cosd(theta)*cosd(phi);
        y= -r*sind(theta);
    end
    %the function below was adapted from QEA3s
    %arduinoLibraryConnectionTherma to act as a 'stop' in case of an
    %emergency.
    function handleKeyPress(fig,event) 
        %function to stop and save a scan with the spacebar, p to save
        if strcmp(event.Key, 'space') == 1 && strcmp(event.EventName, 'KeyPress') == 1
            shouldStop = true;
        end
        if strcmp(event.Key, 'p') == 1 && strcmp(event.EventName, 'KeyPress') == 1
            [pathstr,name,ext] = fileparts(logfile);
            previewPath = fullfile(pathstr, [[name,'_preview'],ext]);
            dataCopy = data;
            data = (data(1:currElement-1,:));
            save(previewPath, '3Ddata');
            data = dataCopy;
        end
    end

    if nargin < 1
        disp('Please specify a file to write the data to as the first argument');
        return;
    end
    if nargin < 2
        doScan = true; % turn on the scan once we tell it to on the command window
    end
    
clear s
f = figure(); % start the figure for where we'll see the XYZ coordinates
set(f,'KeyPressFcn',@handleKeyPress); % look out for the press that that will finalize the figure
%establish a hardcoded serial connection with the arduino.
port = 'COM6';
baudrate = 9600;
s = serialport(port,baudrate);
data = [0,0,0];
shouldStop = false; 
figure(f);
pause(4); % wait to ensure the connection is 
writeline(s, '45,45')
pause(4)
while ~shouldStop
%sweep through the servo angles we'll be using
for phi = 10:spacing:80
    for theta = 10:spacing:80
        coord = strcat(num2str(phi),',',num2str(theta));
        disp(strcat('sending ',coord))
        writeline(s, coord) %make the arduino reach a position
        timeout = 0;
        ready = 0;
        while timeout < 20 && ~ready %breaks once a signal is received and is within the timeout threshold
            while s.NumBytesAvailable > 0
                read = readline(s);
                ready = 1;
                vals = split(read, ',');
                r = (str2double(vals(3))/16)+2.25; %read IR data for the phi,theta
                [x,y,z] = get_coords(phi,theta,r);
                data = [data;[x,y,z]]; %store values in a matrix
                point = 60;
                scatter(x, y, point, z, 'filled')
                %axis([-20 20 -20 20])
                hold on
                drawnow
            end
            pause(0.5)
            timeout = timeout + 1;
        end
    end
end
shouldStop = true;
end
% remove keyboard handler to allow connection to the Arduino to clear
    set(f,'KeyPressFcn',@(a, b) 1);
    save(logfile, 'data');
end
