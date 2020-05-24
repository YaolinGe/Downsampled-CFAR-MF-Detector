f=50e3;
Tp=1/f;
Fs=400e3;
dt=1/Fs;
T=Tp*5;

tsymbol = (0:dt:T-dt);
x=hann(length(tsymbol));
waveform=transpose(sin(2*pi*f*tsymbol)).*x;
bias=5;

s=[zeros(Fs*10*T,1);waveform;zeros(Fs*10*T,1)]+bias;

% s(end-(length(waveform)-1):end)=[];
% s=s/max(abs(s));

w=waveform;
% c=conv(s,waveform);
% c=c/max(abs(c));

S=fft(s);
W=fft(w,length(s));
C=conj(S).*W;
c=ifft(C);

c=c/max(abs(c));

figure(888);
plot(s,'b');
hold on;
plot(abs(c),'r');
% plot(envelope(c),'r');
hold off;

