


% Vectores de datos
data = struct;
data.time.v  = single(zeros(1,10000));
data.force.v  = single(zeros(1,10000));
data.acc_x.v  = single(zeros(1,10000));
data.acc_y.v  = single(zeros(1,10000));
data.acc_z.v  = single(zeros(1,10000));
data.ang_x.v  = single(zeros(1,10000));
data.ang_y.v  = single(zeros(1,10000));
data.ang_z.v  = single(zeros(1,10000));
data.t_wt.v  = single(zeros(1,10000));
data.t_1.v  = single(zeros(1,10000));
data.t_2.v  = single(zeros(1,10000));
data.t_3.v  = single(zeros(1,10000));
data.t_bmp.v  = single(zeros(1,10000));
data.pre.v  = single(zeros(1,10000));
data.hum.v  = single(zeros(1,10000));

data.time.i = 1;
data.force.i = 1;
data.acc_x.i = 1;
data.acc_y.i = 1;
data.acc_z.i = 1;
data.ang_x.i = 1;
data.ang_y.i = 1;
data.ang_z.i = 1;
data.t_wt.i = 1;
data.t_1.i = 1;
data.t_2.i = 1;
data.t_3.i = 1;
data.t_bmp.i = 1;
data.pre.i = 1;
data.hum.i = 1;


% Graficas
figure(1);
subplot(3,4,1);
p1 = line(nan, nan);
title("Cell");
grid on;
subplot(3,4,2);
p2 = line(nan, nan);
title("Pressure");
grid on;
subplot(3,4,3);
p3 = line(nan, nan);
title("Humidity");
grid on;
subplot(3,4,4);
p4 = line(nan, nan);
title("T1");
grid on;
subplot(3,4,5);
p5 = line(nan, nan);
title("Ax");
grid on;
subplot(3,4,6);
p6 = line(nan, nan);
title("Ay");
grid on;
subplot(3,4,7);
p7 = line(nan, nan);
title("Az");
grid on;
subplot(3,4,8);
p8 = line(nan, nan);
title("T2");
grid on;
subplot(3,4,9);
p9 = line(nan, nan);
title("θx");
grid on;
subplot(3,4,10);
p10 = line(nan, nan);
title("θy");
grid on;
subplot(3,4,11);
p11 = line(nan, nan);
title("θz");
grid on;
subplot(3,4,12);
p12 = line(nan, nan);
title("T3");
grid on;

X1 = get(p1, 'Xdata');
Y1 = get(p1, 'Ydata');
X2 = get(p2, 'Xdata');
Y2 = get(p2, 'Ydata');
X3 = get(p3, 'Xdata');
Y3 = get(p3, 'Ydata');
X4 = get(p4, 'Xdata');
Y4 = get(p4, 'Ydata');
X5 = get(p5, 'Xdata');
Y5 = get(p5, 'Ydata');
X6 = get(p6, 'Xdata');
Y6 = get(p6, 'Ydata');
X7 = get(p7, 'Xdata');
Y7 = get(p7, 'Ydata');
X8 = get(p8, 'Xdata');
Y8 = get(p8, 'Ydata');
X9 = get(p9, 'Xdata');
Y9 = get(p9, 'Ydata');
X10 = get(p10, 'Xdata');
Y10 = get(p10, 'Ydata');
X11 = get(p11, 'Xdata');
Y11 = get(p11, 'Ydata');
X12 = get(p12, 'Xdata');
Y12 = get(p12, 'Ydata');


pause(5);

% Conectar y verificar conexión
try
    u = udpport("IPV4");
    writeline(u,"TEST","192.168.1.176",80);
catch 
    questdlg('No se pudo establecer la comunicación con el banco de ensayos. No está conectado','Error','OK','OK');
    clear u;
    %error('FIN DEL PROGRAMA. NO CONECTADO.');
end

tic;
while true
    if u.NumBytesAvailable > 0
         c = readline(u);
         if c == "READY"
             break;
         end
     end

    if toc > 3.0
        questdlg('No se pudo establecer la comunicación con el banco de ensayos. El puerto UDP está conectado, pero no responde.','Error','OK','OK');
        clear u;
        error('FIN DEL PROGRAMA. NO RESPONDE.');
    end
end


% Esperar inicio ensayo
questdlg(['Conexión UDP con el banco de ensayos establecida. ' ...
    'Pulse Iniciar para comenzar el ensayo.'],'Espera','Iniciar','Iniciar');
writeline(u,"START","192.168.1.176",80);



% Cosas útiles
INV=[0,0,0,1;0,0,1,0;0,1,0,0;1,0,0,0];

%fread(s,1,'float');

tic;

while true
    if u.NumBytesAvailable > 0
        

        % Detectar el inicio del paquete:
        byte_1 = read(u,1,"uint8");
        byte_2 = 0;
        while true
            while u.NumBytesAvailable < 1
            end       
            byte_2 = read(u,1,"uint8");
    
            % Es el identificador del paquete
            if byte_1 == 83 && byte_2 == 84 
                break;
            end
            byte_1 = byte_2;
            
        end

        paquete = zeros(1,100);
        paquete(1,1) = 83;
        paquete(1,2) = 84;
        bytes_recibidos = 2;
    
        while true
            
            if u.NumBytesAvailable > 1
                
    
                % Leer identificador
                identificador = read(u,1,"uint8");
                paquete(1,bytes_recibidos + 1) = identificador;
                bytes_recibidos = bytes_recibidos + 1;
    
    
                % No es el identificador de fin
                if identificador ~= 69
    
                    while u.NumBytesAvailable < 4
                        % Espera a recibir float
                    end
                    valor = single(read(u,1,"single"));
                    %paquete(1,(bytes_recibidos+2):(bytes_recibidos+5)) = ...
                    %    uint8(INV*(single((typecast(single(valor), 'uint8'))')));
                    
                    bytes_recibidos = bytes_recibidos + 4;
    
                    switch identificador
                        case 116  % t
                            data.time.v(data.time.i) = valor;
                            data.time.i = data.time.i + 1;
                        case 97   % a
                            data.force.v(data.force.i) = valor;
                            data.force.i = data.force.i + 1;
                        case 98   % b
                            data.acc_x.v(data.acc_x.i) = valor;
                            data.acc_x.i = data.acc_x.i + 1;
                        case 99   % c
                            data.acc_y.v(data.acc_y.i) = valor;
                            data.acc_y.i = data.acc_y.i + 1;
                        case 100  % d
                            data.acc_z.v(data.acc_z.i) = valor;
                            data.acc_z.i = data.acc_z.i + 1;
                        case 101  % e
                            data.ang_x.v(data.ang_x.i) = valor;
                            data.ang_x.i = data.ang_x.i + 1;
                        case 102  % f
                            data.ang_y.v(data.ang_y.i) = valor;
                            data.ang_y.i = data.ang_y.i + 1;
                        case 103  % g
                            data.ang_z.v(data.ang_z.i) = valor;
                            data.ang_z.i = data.ang_z.i + 1;
                        case 104  % h
                            data.t_wt.v(data.t_wt.i) = valor;
                            data.t_wt.i = data.t_wt.i + 1;
                        case 105  % i
                            data.t_1.v(data.t_1.i) = valor;
                            data.t_1.i = data.t_1.i + 1;
                        case 106  % j
                            data.t_2.v(data.time.i) = valor;
                            data.t_2.i = data.t_2.i + 1;
                        case 107  % k
                            data.t_3.v(data.t_3.i) = valor;
                            data.t_3.i = data.t_3.i + 1;
                        case 108  % l
                            data.t_bmp.v(data.t_bmp.i) = valor;
                            data.t_bmp.i = data.t_bmp.i + 1;
                        case 109  % m
                            data.pre.v(data.pre.i) = valor;
                            data.pre.i = data.pre.i + 1;
                        case 110  % n
                            data.hum.v(data.hum.i) = valor;
                            data.hum.i = data.hum.i + 1;
                    end
    
                
                % Es el identificador de fin
                else
                    checksum_c = Checksum8(paquete,bytes_recibidos);
                    checksum_r = read(u,1,'uint8');
                    break;
                end
    
    
            end
           
    
        end
    
    end

    % Actualizar las gráficas

    if toc > 1
        tic;
%         subplot(3,4,1);
%         set(p1, 'XData',  data.time.v(1,1:(data.time.i-1)), 'Ydata', data.force.v(1,1:(data.force.i-1)));
%         subplot(3,4,2);
%         set(p2, 'XData',  data.time.v(1,1:(data.time.i-1)), 'Ydata', data.pre.v(1,1:(data.pre.i-1)));
%         subplot(3,4,3);
%         set(p3, 'XData',  data.time.v(1,1:(data.time.i-1)), 'Ydata', data.hum.v(1,1:(data.hum.i-1)));
%         subplot(3,4,4);
%         set(p4, 'XData',  data.time.v(1,1:(data.time.i-1)), 'Ydata', data.t_1.v(1,1:(data.t_1.i-1)));
%         subplot(3,4,5);
%         set(p5, 'XData',  data.time.v(1,1:(data.time.i-1)), 'Ydata', data.acc_x.v(1,1:(data.acc_x.i-1)));
%         subplot(3,4,6);
%         set(p6, 'XData',  data.time.v(1,1:(data.time.i-1)), 'Ydata', data.acc_y.v(1,1:(data.acc_y.i-1)));
%         subplot(3,4,7);
%         set(p7, 'XData',  data.time.v(1,1:(data.time.i-1)), 'Ydata', data.acc_z.v(1,1:(data.acc_z.i-1)));
%         subplot(3,4,8);
%         set(p8, 'XData',  data.time.v(1,1:(data.time.i-1)), 'Ydata', data.t_2.v(1,1:(data.t_2.i-1)));
%         subplot(3,4,9);
%         set(p9, 'XData',  data.time.v(1,1:(data.time.i-1)), 'Ydata', data.ang_x.v(1,1:(data.ang_x.i-1)));
%         subplot(3,4,10);
%         set(p10, 'XData',  data.time.v(1,1:(data.time.i-1)), 'Ydata', data.ang_y.v(1,1:(data.ang_y.i-1)));
%         subplot(3,4,11);
%         set(p11, 'XData',  data.time.v(1,1:(data.time.i-1)), 'Ydata', data.ang_z.v(1,1:(data.ang_z.i-1)));
%         subplot(3,4,12);
%         set(p12, 'XData',  data.time.v(1,1:(data.time.i-1)), 'Ydata', data.t_3.v(1,1:(data.t_3.i-1)));
        

        X1 = data.time.v(1,1:(data.time.i-1));
        Y1 = data.force.v(1,1:(data.force.i-1));
        X2 = data.time.v(1,1:(data.time.i-1));
        Y2 = data.pre.v(1,1:(data.pre.i-1));
        X3 = data.time.v(1,1:(data.time.i-1));
        Y3 = data.hum.v(1,1:(data.hum.i-1));
        X4 = data.time.v(1,1:(data.time.i-1));
        Y4 = data.t_1.v(1,1:(data.t_1.i-1));
        X5 = data.time.v(1,1:(data.time.i-1));
        Y5 = data.acc_x.v(1,1:(data.acc_x.i-1));
        X6 = data.time.v(1,1:(data.time.i-1));
        Y6 = data.acc_y.v(1,1:(data.acc_y.i-1));
        X7 = data.time.v(1,1:(data.time.i-1));
        Y7 = data.acc_z.v(1,1:(data.acc_z.i-1));
        X8 = data.time.v(1,1:(data.time.i-1));
        Y8 = data.t_2.v(1,1:(data.t_2.i-1));
        X9 = data.time.v(1,1:(data.time.i-1));
        Y9 = data.ang_x.v(1,1:(data.ang_x.i-1));
        X10 = data.time.v(1,1:(data.time.i-1));
        Y10 = data.ang_y.v(1,1:(data.ang_y.i-1));
        X11 = data.time.v(1,1:(data.time.i-1));
        Y11 = data.ang_z.v(1,1:(data.ang_z.i-1));
        X12 = data.time.v(1,1:(data.time.i-1));
        Y12 = data.t_3.v(1,1:(data.t_3.i-1));

        set(p1, 'XData', X1, 'YData', Y1);
        set(p2, 'XData', X2, 'YData', Y2);
        set(p3, 'XData', X3, 'YData', Y3);
        set(p4, 'XData', X4, 'YData', Y4);
        set(p5, 'XData', X5, 'YData', Y5);
        set(p6, 'XData', X6, 'YData', Y6);
        set(p7, 'XData', X7, 'YData', Y7);
        set(p8, 'XData', X8, 'YData', Y8);
        set(p9, 'XData', X9, 'YData', Y9);
        set(p10, 'XData', X10, 'YData', Y10);
        set(p11, 'XData', X11, 'YData', Y11);
        set(p12, 'XData', X12, 'YData', Y12);
        
        a = toc;

        drawnow;
        
        b = toc;
        
        A = [a,b]


    end



end

