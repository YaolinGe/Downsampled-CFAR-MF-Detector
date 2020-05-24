close all;
% Path to lib with functions:
addpath('.');

msq=mseq(2,7,1,2);
wait=zeros(round(.5*length(msq)),1);


r=[wait;msq;wait];
z=conv(r+0.1*randn(length(r),1),flipud(msq));
z(end-(length(msq)-2):end)=[]; 

Msq=fft(r,5000);

figure(1);
% sgtitle('Autocorrelation of M-sequence')
subplot(3,1,1);
plot(r);
title('Time response')
xlim([0 length(z)])
ylabel('Amplitude')
subplot(3,1,2);
plot(abs(z));
xlim([0 length(z)])
xlabel('[n]'); ylabel('Auto-Correlation')

f=(0:length(Msq)-1)/length(Msq);
subplot(3,1,3);
plot(f,abs(Msq)/max(abs(Msq)));
xlim([0 0.5]);
title('Normalised frequency response');
xlabel('$\nu$','Interpreter','latex')
