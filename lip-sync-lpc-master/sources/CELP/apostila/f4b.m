%1.2.4b LP analise e sintese com pitch fixo
frame_size = 240;   % frame de 30ms
frame_avan = 80;    % cada frame � avan�ado de 10ms
p = 10;             % Numero de coef. do filtro LP
f0 = 40;            % Pitch de reprodu��o (em amostras)

% inicia as variaveis de saida e de estado do filtro
Z = zeros(p,1);     % condi��es iniciais nulas
offset = 0;         % offset do sinal de exita��o come�a em zero


% adiciona padding ao final para que o dado seja multiplo das janelas
njanelas = (length(y)-frame_size)/frame_avan;
padsize  = (njanelas - floor(njanelas))*frame_size;
y = padarray(y,padsize,0,'post');

% examina cada frame
for i=1:(length(y)-frame_size)/frame_avan   
    
    %-------------- Analize LPC --------------------
    % Extrai um frame para analizar
    frame = y((i-1)*(frame_avan)+[1:frame_size]);
    
    % calculo dos coeficientes LPC, aplica uma janela de hamming no frame
    [A,sigma2] = lpc(frame.*hamming(length(frame)),p);  
    sigma = sqrt(sigma2);            %desvio padr�o do sinal de exita��o
    
    
    %------------- Sintese LPC --------------------
    % gera 10ms (80 amostras) sinal de exita��o estimado,
    % contado com qualquer offset adicionado por frames anteriores
    exc = zeros(frame_avan,1);      % sinal come�a como nulo
    if offset >= frame_avan         % caso n�o exist�o pulsos nesse frame
        offset = offset-frame_avan; % apenas diminue o offset
    else
        exc(offset+1:f0:end) = 1;   % sen�o adiciona os pulsos necessarios
        if mod(frame_avan-offset,f0) == 0
            offset = 0;             % caso corretamente o offset � zero
        else                        % sen�o calcula o novo offset
            offset=f0-mod(frame_avan-offset,f0);
        end;
    end;  
    
    % ganho para que o residuo e seu estimador tenham a mesma variancia
    gain = sigma/sqrt(1/f0);                                             
    
    % Aplica o filtro LP de sintese no sinal de exita��o estimado
    [recon,Z] = filter(1,A,exc*gain,Z);  % condi��es iniciais em loop
    
    % encadeando frames de sintese
    if i==1
        o = [recon];   
        e = [exc];         
    else
        o = [o;recon];   
        e = [e;exc];   
    end;
end;

% tocar o sinal sintetizado
wavplay(o,Fs)