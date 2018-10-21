close all
clear all
clc
ref=40;
fc=ref*2;
reg5=2;
reg231=hex2dec('66');
reg233=hex2dec('49');
reg234=hex2dec('52');
reg235=hex2dec('60');
f0=floor(reg231/reg5*fc);
nf=bitshift(reg235,16)+bitshift(reg234,8)+reg233;
f1=nf/8388593/reg5*fc;
freq=f0+f1;
