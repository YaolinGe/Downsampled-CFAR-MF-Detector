% clear; 
close all;
path = 'C:\Users\geyao\Desktop\';


fname1='test.txt';
% fname1='pwmsig_60e3_passband.txt';
% fname1='pwmsigshort.txt';

datacnt=1;
dfile1=get_data_sonarfile([path,fname1],1); % Downloaded data
dat=dfile1(:,1);
dat(isnan(dat))=[]; iall=find(dat); dat=dat(1:iall(end));
% dat_c=dfile1(:,1)+1j*dfile1(:,2);

% Ref signal
fname2='test.txt';
% fname2='pwmsig_60e3_baseband.txt';
dfile2=get_data_sonarfile([path,fname2],1); % Downloaded data
datref=dfile2(:,1);
datref(isnan(datref))=[]; iall=find(datref); datref=datref(1:iall(end));

vs = 1460; % Speed of sound in water
Fs=1.6e5;
% Fs=2e5;
dt=1/Fs;    
f=(0:length(dat)-1)*Fs/length(dat);
%% Plots
figure(3);
plot(dat(:)-mean(dat));

figure(4);
plot(abs(datref(:)-mean(datref)));

figure(5); 
plot(envelope(dat))

figure(6)
plot(f,abs(fft(dat-mean(dat))))









% B=10000; dtB=1/B;
% idat_nonzero=find(dat);
% dat_passband=dat(1:idat_nonzero(end));
% % idatref_nonzero=find(datref);
% dat_baseband=datref((0:length(datref)-1)*dtB<=idat_nonzero(end)*dt);
% % dat_baseband=datref(1:905);
% 
% figure(3);
% Np=length(dat_passband); Nb=length(dat_baseband);
% dat_passbandabs=abs(dat_passband-mean(dat_passband));
% plot((0:Np-1)*dt,dat_passbandabs/max(dat_passbandabs));
% 
% hold on;
% plot((0:Nb-1)*dtB,dat_baseband(:)/max(dat_baseband(:)),'-o');
% hold off;








% figure(1);
% d.re=transpose(dfile1(:,1)); d.im=transpose(dfile1(:,2));
% plot(absC(d));

% plot(datref(:)/max(abs(datref)));
% hold on;
% d.re=transpose(dfile1(:,1)); d.im=transpose(dfile1(:,2));
% ad=absC(d);
% plot([zeros(1,3),ad/max(ad),zeros(1,2)],'k-o');
% hold off;

% Fsb=10000;
% dtb=1/Fsb;
% Ndc=floor(length(datref)*Fsb/Fs);
% % figure(2);
% plot((0:length(datref)-1)*dt,abs(datref(:))/max(abs(datref)));
% hold on;
% d.re=transpose(dfile1(:,1)); d.im=transpose(dfile1(:,2));
% ad=absC(d);
% plot((0:Ndc-1)*dtb,ad(1:Ndc)/max(ad),'ko-');
% hold off
% 
% 
% 
% inds=mod(0:length(datref)-1,16); 
% inds(inds~=0)=1; inds(1)=0; 
% inds=~inds;
% test.re=INdclp.re(inds);
% test.im=INdclp.im(inds);
% 
% figure(2);
% INpl=absC(INdclp(:));
% plot((0:length(INdclp.re)-1)*dt,INpl/max(INpl) );
% hold on;
% plot((0:length(test.re)-1)*dtb,absC(test)/max(absC(test)),'-o');
% hold off;






% test.re=transpose(dfile1(:,1)); test.im=transpose(dfile1(:,2));
% 
% figure;plot(absC(test));
% figure;plot(absC(INdc));
% figure;plot(absC(INdclp));
% 
% lp=filterC(test,filt_ADCc,Nadc-Nfilt+1,Nfilt);
% figure;plot(absC( lp ) );







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


function [x_c]=re2complex(x)
x_c.re=x;
x_c.im=zeros(1,length(x));
end

function y=rescale_maxC(y,maxvaly)
maxy=0;
for i=1:length(y.re)
    if(abs(y.re(i))>maxy)
        maxy=abs(y.re(i));
    end
end

for i=1:length(y.im)
    if(abs(y.im(i))>maxy)
        maxy=abs(y.im(i));
    end
end

if maxy>maxvaly
%     scale=ceil(maxvaly/maxy); % to avoid overflow
    for i=1:length(y.re)
        y.re(i)=(y.re(i)*maxvaly)/maxy;
        y.im(i)=(y.im(i)*maxvaly)/maxy;
    end
end

end

function y=rescale_minC(y,minvaly)
miny=2^31-1;
for i=1:length(y.re)
    if( abs(y.re(i))<miny && abs(y.re(i))~=0 )
        miny=abs(y.re(i));
    end
end

for i=1:length(y.im)
    if( abs(y.im(i))<miny && abs(y.im(i))~=0 )
        miny=abs(y.im(i));
    end
end

if miny>minvaly
    for i=1:length(y.re)
        y.re(i)=(y.re(i)*minvaly)/miny;
        y.im(i)=(y.im(i)*minvaly)/miny;
    end
end
y.re=round(y.re);
y.im=round(y.im);
end

function [xpad]=filter_prepad(x,padval,Nfilt)
% IN THE SONAR THE PADDING NEEDS TO BE SAMPLED VALUES
Nrot=floor(Nfilt/2); % Number of taps rotated by filter
Nrot_end=ceil(Nfilt/2)-1;
xpad=[ones(1,Nrot)*padval,x,ones(1,Nrot_end)*padval];
% % IN THE SONAR THE PADDING NEEDS TO BE SAMPLED VALUES
% Npad=Nfilt-1; % Number of taps rotated by filter
% % Nrot_end=ceil(Nfilt/2)-1;
% xpad=[ones(1,Npad)*padval,x];
end

function [ADC]=sig2ADC(x,ADCRES,INPUTGAIN)
% Convert analog signal to sampled complex sonar fixed point representation
% ADC=round((INPUTGAIN*(x/max(abs(x))/2)+0.5)*ADCRES); % Convert to ADC input
ADC=round((INPUTGAIN*(x/max(abs(x))/2))*ADCRES); % Convert to ADC input

end

function [ADC]=sig2FXP(x,FXP_RES)
% Convert analog signal to sampled complex sonar fixed point representation
ADC=round( (x/max(abs(x))/2)*FXP_RES ); % Convert to ADC input
end

function y=filt_derot(y_rotated,Ntaps)
Nrot=floor(Ntaps/2);
% Compensate for filter offset 
if(length(y_rotated(:,1))<length(y_rotated(1,:)))
    y=fliplr(circshift(fliplr(y_rotated),Nrot));
else
    y=flipud(circshift(flipud(y_rotated),Nrot));
end
y(end-Nrot+1:end)=0;
end

function [res,Nvalid]=filter(data,filt,Ndata,Nfilt)

% res=zeros(1,Ndata+2-2*Nfilt);
res=zeros(1,Ndata);
intersum=zeros(1,Nfilt);

iires=1; % Index of result
for i=1:Ndata
    if(i==Ndata)
        a=1;
    end
    intersum(:)=data(i:i+Nfilt-1).*filt;
    res(iires)=sum(intersum);
    iires=iires+1;
%     i
end
Nvalid=iires-1;
end

function xabs=absC(x)
    xabs=sqrt((x.re).^2+(x.im).^2);
end

function [res,Nvalid]=correlateC(data,filt,Ndata,Nfilt)

% res=zeros(1,Ndata+2-2*Nfilt);
res=zeros(1,Ndata);
intersum=zeros(1,Nfilt);

iires=1; % Index of result
for i=1:Ndata
    if(i==Ndata)
        a=1;
    end
    intersum(:)=mult_complexC(data(i:i+Nfilt-1),fliplr(filt)); %% FLIPPED ORDER
%         intersum(:)=data(i:i+Nfilt-1).*filt;
    res(iires)=sum(intersum);
    iires=iires+1;
%     i
end
Nvalid=iires-1;
end

function [res]=filterC(data,filt,Ndata,Nfilt)

% res=zeros(1,Ndata+2-2*Nfilt);
res=re2complex(zeros(1,Ndata));
intersum=re2complex(zeros(1,Nfilt));

% iires=1; % Index of result
for i=1:Ndata
    if(i==Ndata)
        a=1;
    end
    dataslice.re=data.re(i:i+Nfilt-1); 
    dataslice.im=data.im(i:i+Nfilt-1); 
    intersum(:)=mult_complexC(dataslice,filt);
%         intersum(:)=data(i:i+Nfilt-1).*filt;
    res=assign_complex(res,i,sum_complexC(intersum));
%     iires=iires+1;
%     i
end
% Nvalid=iires-1;
end

function output=assign_complex(output,index,value)
    output.re(index)=value.re;
    output.im(index)=value.im;
end

function res=sum_complexC(val)
    res.re=sum(val.re);
    res.im=sum(val.im);
end

% Vector & single value function
function res=mult_complexC(in1,in2)
    res.re=in1.re.*in2.re-in1.im.*in2.im;
    res.im=in1.re.*in2.im+in1.im.*in2.re;
end