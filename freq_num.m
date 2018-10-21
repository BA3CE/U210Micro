close all
clear all
clc
ref=40;
fc=ref*2;
f=1116;
len=length(f);
reg5 = zeros(1,len);
for i=1:len
    if(f(i) >=3000 && f(i) <= 6000)
        reg5(i) = 2;
    elseif(f(i)>=1500 && f(i) <3000)
        reg5(i) = 4;
    elseif(f(i)>=750 && f(i) <1500)
         reg5(i) = 8;
    elseif(f(i)>=375 && f(i) <750)
         reg5(i) = 16;
    elseif(f(i)>=187.5 && f(i) <375)
         reg5(i) = 32;
    elseif(f(i)>=93.75 && f(i) <187.5)
         reg5(i) = 64;
    elseif(f(i)>=70 && f(i) <93.75)
         reg5(i) = 128;
    end
end

Ni=floor(f.*reg5/fc);
Nf=round(8388593*(f.*reg5/fc-Ni));
reg231=dec2hex(Ni);
reg233=dec2hex(bitand(int32(Nf),255));
reg234=dec2hex(bitand(bitshift(int32(Nf),-8),255));
reg235=dec2hex(bitand(bitshift(int32(Nf),-16),255));
