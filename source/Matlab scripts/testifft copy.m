Fs=100e3;
dt=1/Fs;
f=1e3;
T=1/f;

Np=10;

N=round(Fs*(Np*T));
t=dt*(0:N-1);

s=sin(2*pi*f*t);
S=fft(s);
figure;plot(s);

figure;plot(abs(S));

figure;plot(abs(fft(conj(S))));