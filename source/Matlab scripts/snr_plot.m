clearvars
close all
% clc

% Some notes: 
% If SNR is 0, then something is wrong in the embedded calculation:
% dB(0)=-inf -> dB is set to zero. This is then probably caused by 
% underflow in integer calculation (values smaller than one are rounded
% to zero). Keep an eye out for this!
% 
% Path to data-file
% path = ['C:\Users\vikto\Documents\Sonar\Captured_echos\',...
%     'fiskpinger_brunnsviken\mätning_1_brygga-brygga_ca_93m\'];
path = ['C:\Users\vikto\Documents\Sonar\Captured_echos\',...
    'fiskpinger_brunnsviken\mätning_2_ brygga-1dmunderyta_0_5mdjup_ca_$_avstånd\'];

% fname='asdf1.txt';
fname='NFFT=512_30s.txt';
% fname='NFFT=1024_30s.txt';
% fname='NFFT=2048_30s.txt';
% fname='NFFT=4096_30s.txt';
fnamecmp='NFFT=512_30s.txt';

% Load data file
datacnt=2; % Number of data points
data=get_data_sonarfile([path,fname],datacnt);
datacmp=get_data_sonarfile([path,fnamecmp],2);

S=data(:,1); % Signal values
N=data(:,2); % Noise values

% Set sampling parameters
Fs=160e3;
% winlen=4096; % adc values per snr data point
winlen=2048; % adc values per snr data point
dt=winlen/Fs;
t=(0:length(data(:,1))-1)*dt;

figure(97)
% plot(t,S,'DisplayName','Pinger @ 71kHz');
plot(t,S,'DisplayName','Signal SPL');

hold on;
plot(t,N,'--','DisplayName','NL (10 [s] average)');
hold off;
ylabel('SPL [dB]')
xlabel('Time [s]')
legend();

% figure(98)
% plot(t,S-N,'DisplayName','SNR');
% ylabel('SPL [dB]')
% xlabel('Time [s]')
% legend(); 
% title('Signal SNR')

% figure(99)
% Scmp=datacmp(:,1);
% Ncmp=datacmp(:,2);
% plot((0:length(datacmp(:,1))-1)*dt,Scmp);
% title('Reference plot SNR')

% adc=dataraw(:,1); % ADC values
% Vref=3.661; % V max on adc, measured
% v2adc=(2^12)/Vref;
% transducer_sensitivity=-200; % dBV ref 1 uPa
% Gin=200; % Gain of input amplifier
% NFFT=length(adc);
% freq=71e3;
% ADC=abs(fft(adc-mean(adc),NFFT));
% figure(1);plot(adc);
% f=Fs*(0:NFFT-1)/NFFT;
% VV=sqrt(((2*ADC/v2adc/Gin).^2)/NFFT);
% figure(2);plot(f,ADC);
% figure(3);plot(f,20*log10(ADC/v2adc/Gin)-transducer_sensitivity); title('SPL')
% % figure(4);plot(f,2*ADC/v2adc/Gin/NFFT); title('voltage')
% figure(4);plot(f,VV); title('voltage')
% figure(5);plot(f,20*log10(VV)-transducer_sensitivity); title('SPL freq')
% 
% % N=round((4096/freq)*Fs);
% % A=2*ADC(f<freq+10&f>freq)/N;
% % Vin_est=(A/v2adc)/Gin;
% % SPL=20*log10(Vin_est)-transducer_sensitivity
% 
% vin_t=sqrt(var((adc-mean(adc))/Gin/v2adc));
% splt=20*log10(vin_t)-transducer_sensitivity
% splf=20*log10(sqrt(var(VV)))-transducer_sensitivity


function d=get_data_sonarfile(filename,data_cnt)
    counter = 1;
%     data_cnt=1;
    delim = ';';
    file = fopen(filename);
    string = fgetl(file);
    temp={};
    while ischar(string)

        % Count number of ';'
        if(sum(ismember(string,lower(delim))) == data_cnt)

            for( i = 1 : 1 : data_cnt)
                [value,remainder] = strtok(string,';');
                temp{counter,i} = str2double(value);
                string = remainder;
            end
        end
        counter = counter+1;
        string = fgetl(file);
    end
    if(isempty(temp)~=1)
        d = cell2mat(temp);
    else
        d=[];
    end
    fclose(file);
end