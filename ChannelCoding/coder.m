clear all;
pkg load signal;

%format( 'short' );

i_x = [0b000100, 0b000101]; 

%a = bitshift(i_x(1), 6);
%disp( a );

gen = 0b1101111;
genlen = 7;
infolen = 12;

%bitxor

%{
void modulo2div(char data[], char key[], int datalen, int keylen)
{

char temp[20],rem[20];

for(int i=0;i<keylen;i++)
rem[i]=data[i];                    //considering keylen-1 bits of data for each step of binary division/EXOR 

for(int j=keylen;j<=datalen;j++)
{
    for(int i=0;i<keylen;i++)
    temp[i]=rem[i];                // remainder of previous step is divident of current step

    if(rem[0]=='0')                //if 1's bit of remainder is 0 then shift the rem by 1 bit
    {
        for(int i=0;i<keylen-1;i++)
            rem[i]=temp[i+1];
    }
    else                    //else exor the divident with generator polynomial
    {    
        for(int i=0;i<keylen-1;i++)
            rem[i]=exor(temp[i+1],key[i+1]);
            
    }
        rem[keylen-1]=data[j];        //appending next bit of data to remainder obtain by division
    
}
    
cout<<"CRC="<<rem<<"\nDataword="<<data;        //remainder obtain is crc
}
%}

%figure(1)
%subplot(1,1,1)
%clf
%plot(t,input,'Color',[0.5 0.5 0.5])
%hold on
%axis([0 0.21 -600 600])
%xlabel('Time in s','fontweight','normal','FontName','Arial', 'FontSize',10)
%legend('Noise','Low Pass','Moving Average','PT1','Median','Morphological Operators','Location','north')
%axis off
%grid on

