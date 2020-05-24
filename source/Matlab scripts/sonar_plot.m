%% Analysing output from KTH sonar prototype 1 (Clemens)
clearvars
close all
clc

%% Set what data file to read
% path = 'F:\data\Sonar_data\';
path = 'C:\Users\vikto\Documents\Sonar\Captured_echos\';
fname='asdf1.TXT';
% fnameref='asdf2.txt';
% fnameref='calibsig_freq_piccalculated.txt';
% fname='bergshamra_weak_time.TXT';
% fnameref='bergshamra_weak_corr.txt';
% pathref = path;
% fnameref='70kHz_fish_pinger_Fs160kHz.txt';
% fnameref='brunnsviken_160kHzsample_weak.txt';
pathref = ['../../reference_measurements/',...
    'FOI_tank_ca_4m_middle_of_the_pool/with_output_filter/',...
    ];
fnameref='square_tank_Fs160kHz.txt';

%% Read data file
datacnt=1;
data=get_data_sonarfile([path,fname],datacnt); % Downloaded data
%% Read reference file
ref=get_data_sonarfile([pathref,fnameref],datacnt);

%% Analyse data file
% vs = 343; % Speed of sound in air
vs = 1460; % Speed of sound in water
Fs=1.6e5;
% Fs=2e5;
dt=1/Fs;

datacnt=2;
% data2=get_data_sonarfile([path,fname],datacnt);
% S=data2(:,1);
% N=data2(:,2);

% figure(99)
% plot(S);
% hold on;
% plot(N);
% hold off;
% 
% 
% data = datadl(:,1);
dataref = ref(:,1);



% % Remove zero values
% i=1;
% while(i<=length(data))
%    if(data(i)==0)
%         data(i)=[];
%    else
%        i=i+1;
%    end
% end

% i=1;
% while(i<=length(dataref))
%    if(dataref(i)==0)
%         dataref(i)=[];
%    else
%        i=i+1;
%    end
% end

%% Bandpass filter reference data
% bpFilt = designfilt('bandpassfir','FilterOrder',45, ...
%          'CutoffFrequency1',50e3-40e3,'CutoffFrequency2',50e3+25e3, ...
%          'SampleRate',Fs);
% data_bpfilt=filtfilt(bpFilt,data);
% dataref_bpfilt=filtfilt(bpFilt,dataref);


%% For frequency plots
NFFT=200000;
f=Fs*(0:NFFT-1)/NFFT;


%% Slice out some data
% slice=fref(1178:1409);
% Slice=fft(slice,FFT_LEN);

%% Plot
% deadtime=3.77e-3;
deadtime=2.324e-3;
% deadtime=0.547e-3;
deadtime=0;
Ndead=ceil(deadtime*Fs);
data=[zeros(Ndead,1);data];

tadc=dt*(0:length(data)-1);
range=tadc*vs/2;


% figure(1);plot(range,data);
% title('Data vs range');
% figure(2);plot(tadc,data);
% title('Data vs time');
NF=length(data);
ff=Fs*(0:NF-1)/NF;
figure(3);plot(data);
title('Data');

figure(4);plot(dataref);
title('Data ref')

spc=dataref_bpfilt;
% figure(5)
% spectrogram(spc,256,250,[],Fs,'yaxis');

% title('Reference vs range');
% figure(5);plot(dataref);
% title('Reference vs time');
% figure(6);plot(dataref);
% title('Reference');

% figure(10)
% spectrogram(adc,128,120,128,Fs);


%% END


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