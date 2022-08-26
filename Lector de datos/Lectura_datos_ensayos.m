%% LECTOR DATOS BANCO DE ENSAYOS

clc 
clear all
close all


%% Escritura en el script de arduino

% tiempo                  (ms)
% temperatura_BMP280      (ºC)
% presion_BMP280          (hPa)
% humedad_DHT11           (%)
% temperatura_DHT11       (ºC)
% lect
% mes
% A_x
% A_y
% A_z
% T_dallas_1              (ºC)
% T_dallas_4              (ºC)
% T_dallas_3              (ºC)
% termopar_1              (ºC)
% termopar_2              (ºC)
% termopar_3              (ºC)
% termopar_4              (ºC)


%% Lectura de datos

fileID = fopen("DATOS.TXT",'r');
h = 0;
paquetes_perdidos = 0;

while (~feof(fileID))
    h = h+1;
    check = fread(fileID,2,'char');
             if (check(1) ~= 'E' || check(2) ~= 'E')
                 paquetes_perdidos = paquetes_perdidos + 1;
             end    
                  while (check(1) ~= 'E' || check(2) ~= 'E')
                      check(1) = check(2);
                      check(2) = fread(fileID,1,'char');
                  end

             Data(h,1) = fread(fileID,1,'uint32');
    for i=2:5
        Data(h,i) = fread(fileID,1,'single');
    end 
    for i=6:7
        Data(h,i) = fread(fileID,1,'int32');
    end 
end
fclose(fileID);


%% Plot de los datos

tiempo             = Data(:,1)./1000;  
temperatura_BMP    = Data(:,2);  
presion_BMP        = Data(:,3); 
humedad_DHT11      = Data(:,4);  
temperatura_DHT11  = Data(:,5); 
lectura_celula     = Data(:,6);  
empuje_celula      = Data(:,7);  
% a_x                = Data(:,8);
% a_x                = Data(:,9);
% a_x                = Data(:,10);
% tem_dallas_1       = Data(:,11);  
% tem_dallas_2       = Data(:,12);
% tem_dallas_3       = Data(:,13);
% termopar_1         = Data(:,14);
% termopar_2         = Data(:,15);
% termopar_3         = Data(:,16);
% termopar_4         = Data(:,17);


datap = (lectura_celula-5700)*440.22/(15600-9365)*9.8/1000; % Esto es para pasar de lect a N

figure (1)
plot(tiempo,presion_BMP,'b')
title('PRESIÓN (hPa)')
xlabel('Tiempo (s)')
ylabel('Presión (hPa)')
grid on

fig = figure (2);
set(fig,'defaultAxesColorOrder',[[0 0 0]; [0 0 0]]);
title('Empuje en N y Kg del motor')
xlabel('Time (s)');
yyaxis left
plot(tiempo, datap,'b')
ylabel('Thrust (N)');
yyaxis right
plot(tiempo, datap./9.8,'c')
ylabel('Thrust (Kg)');
legend({'Newtons','Kg'},'Location','NorthEastOutside')
grid on

figure (3)
plot(tiempo,humedad_DHT11,'b')
title('HUMEDAD (%)')
xlabel('Tiempo (s)')
ylabel('Humedad (%)')
grid on

figure(4)
plot(tiempo,temperatura_BMP,'b')
hold on
plot(tiempo,temperatura_DHT11,'c')
hold off
title('TEMPERATURA (º)')
xlabel('Tiempo (s)')
ylabel('Temperatura (º)')
legend({'BMP280','DHT11'},'Location','northeast')
grid on


% figure(5) 
% plot(tiempo,a_x,'b')
% hold on
% plot(tiempo,a_y,'c')
% hold on
% plot(tiempo,a_y,'g')
% hold off
% title('ACELERACIÓN (g)')
% xlabel('Tiempo (s)')
% ylabel('Aceleración (g)')
% legend({'Acc_x','Acc_y','Acc_z'},'Location','northeast')

% figure(6) 
% plot(tiempo,tem_dallas_1,'r')
% hold on
% plot(tiempo,tem_dallas_2,'y')
% hold on
% plot(tiempo,tem_dallas_3,'m')
% hold off
% title('Temperatura DS18B20 (ºC)')
% xlabel('Tiempo (s)')
% ylabel('Temperatura (ºC)')
% legend({'Dallas_1','Dallas_2','Dallas_3'},'Location','northeast')

% figure(7) 
% plot(tiempo,termopar_1,'r')
% hold on
% plot(tiempo,termopar_2,'y')
% hold on
% plot(tiempo,termopar_3,'m')
% hold on
% plot(tiempo,termopar_4,'g')
% hold off
% title('Temperatura Termopares (ºC)')
% xlabel('Tiempo (s)')
% ylabel('Temperatura (ºC)')
% legend({'Termopar_1','Termopar_2','Termopar_3','Termopar_4'},'Location','northeast')
