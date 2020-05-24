
%Fs = 2181823; %Sampling frequency [Hz]
%Fs = 683761; %Sampling frequency [Hz]
Fs = 6e-7;

Y = fft(sample(:));
%Y = fft(adc(:));
%L = length(adc);
L = length(sample);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = Fs*(0:(L/2))/L;

figure(2)
plot(f,P1) 
title('Single-Sided Amplitude Spectrum of X(t)')
xlabel('f (Hz)')
ylabel('|P1(f)|')

%f = (0:length(samplefft)-1);
%figure(21)
%plot(f,abs(samplefft));