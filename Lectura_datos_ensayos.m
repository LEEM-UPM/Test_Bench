file = fopen('DATOS');
data = fread(file,'int')
datap = (data-5700)*440.22/(15600-9365)*9.8/1000;
tiempo = linspace(0,8960,448);
plot(tiempo, datap)
grid on
xlabel('Time (ms)');
ylabel('Thrust (N)');
axis([0 9000 -10 80]);