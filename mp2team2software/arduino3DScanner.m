% File designed to collect data from the Pan-Tilt Mechanism servos and the
% IR sensor that scans the geometry of an object.

function arduino3DScanner(logfile)
    clear s
    tic;
    spacing = 1;
    pan_limits  = [20 70]; 
    tilt_limits = [20 70];
    function [x,y,z] = get_coords(phi,theta,r)
        phi = 45-phi;
        theta = 45-theta;
        tilt_arm = 2; %inch
        pan_arm = 3/8;%inch
        xs=(tilt_arm*cosd(theta)+pan_arm)*sind(phi);
        zs=(tilt_arm*cosd(theta)+pan_arm)*cosd(phi);
        ys=tilt_arm*sind(theta);
        x= (r*cosd(theta)*sind(phi))+xs;
        z= r*cosd(theta)*cosd(phi)-zs;
        y= -r*sind(theta)+ys;
        
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
        logfile = 'data.mat';
    end
    
clear s
f = figure(); % start the figure for where we'll see the XYZ coordinates
set(f,'KeyPressFcn',@handleKeyPress); % look out for the press that that will finalize the figure
%establish a hardcoded serial connection with the arduino.
port = '/dev/ttyACM0';%COM on windows
baudrate = 115200;
s = serialport(port,baudrate);
configureTerminator(s,"CR");
data = [0,0,0];
shouldStop = false; 
figure(f);
pause(4); % wait to ensure the connection is 
writeline(s, '45,45')
%pause(4)
while ~shouldStop
%sweep through the servo angles we'll be using
flipIt = false;
for phi = pan_limits(1):spacing:pan_limits(2)
    flipIt = ~flipIt;
    thetaVals = (tilt_limits(1):spacing:tilt_limits(2));
    if flipIt
        thetaVals = flip(thetaVals);
    end
    for theta = thetaVals
        coord = strcat(num2str(phi),',',num2str(theta));
        writeline(s, coord) %make the arduino reach a position
        %disp(strcat('sending ',coord))
        timeout = 0;
        ready = 0;
        while((timeout < 100) && (ready<1)) %breaks once a signal is received and is within the timeout threshold
            while s.NumBytesAvailable > 1
                read = readline(s);
                %disp(read)
                ready = 1;
                vals = str2double(split(read, ','));
                r = (vals(3)/16); %read IR data for the phi,theta
                [x,y,z] = get_coords(vals(1),vals(2),r);
                data = [data;[x,y,z]]; %store values in a matrix
                
            end
            if ~ready
                pause(0.05)
                timeout = timeout + 1;
            end
        end
    end
end
shouldStop = true;
end
    data = data(2:end,:); % to remove 0,0,0 at start
    disp(toc)
    point = 20;
    scatter(data(:,1), data(:,2), point, data(:,3), 'filled')
% remove keyboard handler to allow connection to the Arduino to clear
    set(f,'KeyPressFcn',@(a, b) 1);
    save(logfile, 'data');
end
