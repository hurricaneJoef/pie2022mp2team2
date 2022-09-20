% File designed to collect data from the Pan-Tilt Mechanism servos and the
% IR sensor that scans the geometry of an object.
function arduino3DScanner(logfile,doScan)
    function x = get_x_pos(phi,theta,r)
        x = r*sin(phi)*cos(theta);
    end
    function y = get_y_pos(phi,theta,r)
        y = r*cos(phi)*sin(theta);
    end
    function z = get_z_val(theta,r)
        r = r*sin(theta)/4;
        z = uint8(r);
    end
    function handleKeyPress(fig,event) %function to stop and save a scan with the spacebar, p to save
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
f = figure(); % start the figure for where we'll see the XYZ coordinates
set(f,'KeyPressFcn',@handleKeyPress); % look out for the press that that will finalize the figure
a = arduino(); %set up arduino
port = a.Port;
clear a
a = arduino(port);
dataRate = 10; %This is pretty arbitrary
data = [0,0,0];
C = zeros(90);
currElement = 1;
shouldStop = false; 
figure(f);
pause(2);
startTime = tic;
while ~shouldStop
    t = toc(startTime);
    data(currElement, 1) = t;
for phi = 0:90            %sweep through servo angles
    for theta = 0:90
        writeline(arduino, num2str(phi)+', '+num2str(theta)+'\n') %make the arduino reach a position
        pause(2)
        r = str2double(readline(arduino)); %read IR data for the phi,theta
        x = get_x_pos(phi,theta,r);
        y = get_y_pos(phi,theta,r);
        z = get_z_val(r);
        data = [data;[x,y,z]]; %store values in a matrix
        C(phi,theta) = z; %store z values in array for mapping the image
        %TODO plot data live
    end
end
end

image(C); %plots data as an image
% remove keyboard handler to allow connection to the Arduino to clear
    set(f,'KeyPressFcn',@(a, b) 1);
    save(logfile, 'data');
end
