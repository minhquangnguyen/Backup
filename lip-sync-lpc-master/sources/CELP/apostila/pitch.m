function [P] = pitch(speech_frame,Pmax,Pmin,V_UV);
%  [P] = pitch(speech_frame);
%
%   A fun��o retorna o periodo de pitch (em amostras) de um sinal de voz
%   Zero � retornado caso no frame n�o seja detectado periocidade
%
%   Pmax     - pitch maximo (em amostras)
%   Pmin     - pitch minimo (em amostras)
%   V_UV     - fator de decis�o 
%
%  Ex.: [P] = pitch(speech_frame,160,16,1.5);


% Calcula a autocorrela��o normalizada
C = xcorr(speech_frame,Pmax,'coeff');  % Calcula s� at� um delay de Pmax
C=C(Pmax+1:2*Pmax+1);         % retira s� a parte direita da autocorrela��o

% Busca pelo valor maximo de autocorrela��o (pitch)
[Cmax,i]=max(C(Pmin:end));    % s� procura em valores validos para o pitch
P = (Pmin+i-2);               % corre��o para o valor retornado pela busca

% Decide se o sinal � Vozeado ou n�o-Vozeado
if  Cmax < V_UV             % Mede a raz�o entre a corela��o do sinal com 
    P = 0;                  % si mesmo e da corela��o do pitch
end;                        % Obs: a correla��o foi normalizada por R(0)

% Grafico de autocorrela��o
% subplot(211);
% plot(speech_frame);
% subplot(212);
% plot([0:Pmax],C);
% hold on;
% plot(P,Cmax,'r*')
% title(sprintf('Pitch at: %d , max %f ',P,Cmax));
% hold off