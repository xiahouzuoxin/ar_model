% Read wave file and storage it in C Format array
% xiahouzuoxin
% 2014.03.03

clc;
clear;

% 选择读取文件
[fname, pname] = uigetfile( ...
       {'*.wav';'*.*'}, ...
        'Pick a file');

[testcase fs nbits] = wavread(fullfile(pname, fname));
sound(testcase,fs);  

N = 1024;    % 提取音频长度
Sn = 1;      % 音频数据首地址偏移量，从1开始

% 格式化输出
fid = fopen('testcase.h','w+');
fprintf(fid, '#ifndef _TESTCASE_H\n');
fprintf(fid, '#define _TESTCASE_H\n\n');

fprintf(fid,'float testcase[] = {\n');
for i = Sn:Sn+N-1
    fprintf(fid,'%.4f,', testcase(i,1));
    if mod(i,10)==0
        fprintf(fid,'\n');
    end
end
% fprintf(fid,'%.4f', testcase(N,1));
fprintf(fid,'\n};\n');
fprintf(fid,'\n#endif\n');

fclose(fid);
