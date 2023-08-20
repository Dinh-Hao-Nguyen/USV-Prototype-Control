% Simulation of Control Allocation Solution for 3 Azimuth Thrusters

clear; close all; clc;

%% Symbolic Computation

% Prototype parameters
syms l1x l1y l2x l2y l3x l3y real

% Generalized forces & torques
syms Fx Fy Tz real

% Define mapping matrix
M = [   1    0    1    0    1    0
        0    1    0    1    0    1
     -l1y  l1x -l2y  l2x -l3y  l3x];    % General case in which azimuth thruster i is located at
                                        % position (lix,liy) in a NED frame centered at the CoM

% Calculate actuator forces
tau = [Fx; Fy; Tz];
F = pinv(M) * tau;
F_symbolic = F;             % F is the numerically pre-computed actuator forces. F_symbolic is the symbolic expression.

% Set prototype parameters
F = subs(F,l1x,-116.53);
F = subs(F,l1y,-54.5);      % value slightly adjusted w.r.t. SolidWorks
F = subs(F,l2x,-116.53);
F = subs(F,l2y,54.5);       % value slightly adjusted w.r.t. SolidWorks
F = subs(F,l3x,101.47);
F = subs(F,l3y,0);

%%
% Set desired generalized forces
F = subs(F,Fx,2.4); % 2.4 N
F = subs(F,Fy,0);   % 2.4 N
F = subs(F,Tz,0);   % 0.3 N.m = 300 N.mm

% Calculate results (for given parameters and generalized forces)
F1x = eval(F(1)); F1y = eval(F(2)); F2x = eval(F(3)); F2y = eval(F(4)); F3x = eval(F(5)); F3y = eval(F(6));

% Convert cartesian forces into thrusts & azimuth angles
T1 = sqrt(F1x^2 + F1y^2);
T2 = sqrt(F2x^2 + F2y^2);
T3 = sqrt(F3x^2 + F3y^2);
theta1 = rad2deg(atan2(F1y,F1x));
theta2 = rad2deg(atan2(F2y,F2x));
theta3 = rad2deg(atan2(F3y,F3x));

% Print thrusts and azimuth angles
disp("thrustLeft   = " + T1 + "N");
disp("thrustRight  = " + T2 + "N");
disp("thrustMiddle = " + T3 + "N");
disp("angleLeft    = " + theta1 + "°");
disp("angleRight   = " + theta2 + "°");
disp("angleMiddle  = " + theta3 + "°");

%% Graphical Interface

% Boat point coordinates
Ax = 84.47;
Ay = -100;
Bx = -145.53;
By = -100;
Cx = -145.53;
Cy = 100;
Dx = 84.47;
Dy = 100;
Ex = 214.47;
Ey = 0;

% Prototype parameters
l1x = -116.53;
l1y = -54.5;
l2x = -116.53;
l2y = 54.5;
l3x = 101.47;
l3y = 0;

figure;
hold on;
axis equal; axis off;

drawArrow = @(x,y,varargin) quiver( x(1),y(1),x(2)-x(1),y(2)-y(1),0, varargin{:} );

% Draw boat shape
line([Ay By],[Ax Bx],'color','k','LineWidth',5);
line([By Cy],[Bx Cx],'color','k','LineWidth',5);
line([Cy Dy],[Cx Dx],'color','k','LineWidth',5);
line([Ay Ey],[Ax Ex],'color','k','LineWidth',5);
line([Dy Ey],[Dx Ex],'color','k','LineWidth',5);

% Draw center of origin
plot(0,0,'color','k','Marker','.','MarkerSize',20);

% Draw arrow for each actuator thrust
scalingFactor = 100;
drawArrow([l1y l1y+scalingFactor*F1y],[l1x l1x+scalingFactor*F1x],'color','r','LineWidth',2,'MaxHeadSize',5);
drawArrow([l2y l2y+scalingFactor*F2y],[l2x l2x+scalingFactor*F2x],'color','g','LineWidth',2,'MaxHeadSize',5);
drawArrow([l3y l3y+scalingFactor*F3y],[l3x l3x+scalingFactor*F3x],'color','b','LineWidth',2,'MaxHeadSize',5);
