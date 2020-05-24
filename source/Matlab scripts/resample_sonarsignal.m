
fpath=['C:\Users\vikto\Documents\Sonar\Captured_echos\'
    ];
fname='70kHz_fish_pinger_Fs200kHz.txt';
fnamenew='70kHz_fish_pinger_Fs160kHz.txt';
Fs=200e3;
Fsnew=160e3;
sig=get_data_sonarfile([fpath,fname]); % Downloaded data
sig_resampled=resample(sig,Fsnew,Fs);

% Save file in the same format
fsave = fopen([fpath,fnamenew],'w');
fprintf(fsave,'%d;\n',round(sig_resampled));
fclose(fsave);

function d=get_data_sonarfile(filename)
    counter = 1;
    data_cnt=1;
    delim = ';';
    file = fopen(filename);
    string = fgetl(file);
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
    d = cell2mat(temp);
    fclose(file);
end