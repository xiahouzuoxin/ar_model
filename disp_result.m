% Display or not

clc;
clear;
close all;

% 选择读取文件
[fname, pname] = uigetfile( ...
       {'*.txt';'*.*'}, ...
        'Pick a file');
    
% 按格式读：格式在test_mem_alloc.c中定义好的
tmp = load(fullfile(pname,fname));

N = tmp(1);
ORDER = tmp(2);
N_FFT = tmp(3);
x = tmp(4:4+N-1);
psd = tmp(4+N:end);
clear tmp;

% 原始信号
figure,
plot(x);
title('Original Signal');grid on

% 互相关对比
% figure,
% r = xcorr(x, 'biased');
% subplot(2,1,1);plot(r);
% title('互相关: Matlab计算');grid on
% subplot(2,1,2);plot(x_cor);
% title('互相关: C计算');grid on

% AR功率谱,因为功率谱是对称的，所以只画出了一半
figure,
xpsd = pyulear(x, ORDER, N_FFT);
xpsd=xpsd/max(xpsd);
% xpsd=10*log10(xpsd+0.000001);
subplot(2,1,1);plot(xpsd);grid on;
title('基于Levension的AR谱估计：Matlab');grid on
psd = psd/max(psd);
% psd=10*log10(psd+0.000001);
subplot(2,1,2);plot(psd(1:N_FFT/2+1));
title('基于Levension的AR谱估计：C');grid on

% Matlab与C计算结果的偏差
err = sum((psd(1:N_FFT/2)-xpsd(1:N_FFT/2)).^2);
fprintf('%.4f\n', err);

% figure, 
% y = fft(x);
% plot(abs(y));



