clc;close;clear;

kp=5;
kd=2;
ki=0.01;

x0=0;
y0=0.1;
theta0=0;
phiR0=0;
phiL0=0;

Vd=1;

sim('DD_Control.slx');

figure;
plot(xd,yd,'r--','LineWidth',2)
hold on
plot(x,y,'b-.','LineWidth',2)
legend('xd-yd','x-y','location','best')
axis equal;
h1=plot(x(1),y(1),'ko','MarkerFaceColor','k','LineWidth',4);
h2=line([x(1) x(1)+0.3*cos(theta(1))],[y(1) y(1)+0.3*sin(theta(1))],'color','k','LineWidth',6);

for ii=2:length(x)
    set(h1,'XData',x(ii));
    set(h1,'YData',y(ii));
    set(h2,'XData',[x(ii) x(ii)+0.1*cos(theta(ii))]);
    set(h2,'YData',[y(ii) y(ii)+0.1*sin(theta(ii))]);
    drawnow;
    pause(0.1);
end
