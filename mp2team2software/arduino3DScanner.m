% File designed to collect data from the Pan-Tilt Mechanism servos and the
% IR sensor that scans the geometry of an object.
function arduino3DScanner(logfile,doScan)
    function x = get_x_pos(phi,theta,r)
        x = phi;%r*sin(phi)*cos(theta);
    end
    function y = get_y_pos(phi,theta,r)
        y = theta;%r*cos(phi)*sin(theta);
    end
    function z = get_z_val(theta,r)
        %r = r*sin(theta)/4;
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
clear s
f = figure(); % start the figure for where we'll see the XYZ coordinates
set(f,'KeyPressFcn',@handleKeyPress); % look out for the press that that will finalize the figure
%a = arduino(); %set up arduino
port = 'COM6';%a.Port;
%clear a
%a = arduino(port);
baudrate = 9600;
s = serialport(port,baudrate);
data = [0,0,0];
%C = zeros(90);
shouldStop = false; 
figure(f);
pause(4);
timeout = 0;
while ~shouldStop
for phi = 10:2:80            %sweep through servo angles
    for theta = 10:2:80
        coord = strcat(num2str(phi),',',num2str(theta));
        disp(strcat('sending ',coord))
        writeline(s, coord) %make the arduino reach a position
        timeout = 0;
        ready = 0;
        while timeout < 20 && ~ready
            while s.NumBytesAvailable > 0
                read = readline(s);
                ready = 1;
                vals = split(read, ',');
                r = str2double(vals(3)); %read IR data for the phi,theta
                x = get_x_pos(phi,theta,r);
                y = get_y_pos(phi,theta,r);
                z = get_z_val(theta,r);
                data = [data;[x,y,z]]; %store values in a matrix
                %C(phi,theta) = z; %store z values in array for mapping the image
                % vv TODO plot data live
                %image(C)
                point = 50;
                scatter(x, y, point, z, 'filled')
                axis([5 85 5 85])
                hold on
                drawnow
                %timeout = 20;
            end
            pause(0.5)
            timeout = timeout + 1;
        end
    end
end
shouldStop = true;
end

%image(C); %plots data as an image
% remove keyboard handler to allow connection to the Arduino to clear
    set(f,'KeyPressFcn',@(a, b) 1);
    save(logfile, 'data');
end
