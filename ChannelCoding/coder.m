clear all;
pkg load signal;

%format( 'short' );

m = 12;
n = 6;
g_x = 0b1101111;

i_x = [0, 0, 0, 0;
       0b000100, 0b000101, 0b000110, 0b000111; 
       0b001000, 0b001001, 0b001010, 0b001011;
       0b001100, 0b001101, 0b001110, 0b001111;
       0, 0, 0, 0;
       0, 0, 0, 0;
       0, 0, 0, 0;
       0, 0, 0, 0;
       0, 0, 0, 0;
       0, 0, 0, 0;
       0, 0, 0, 0;
       0b101100, 0b101101, 0b101110, 0b101111;
       0b110000, 0b110001, 0b110010, 0b110011;
       ];

d_x = zeros(13,4);

for a=1:1:13
  for b=1:1:4
    if i_x(a,b) != 0
      i1_x = bitshift(i_x(a,b), 6);
      startbit = m;
      restbits = 0; %Number 0 bits for the division
      for i=m:-1:n
        if bitget(i1_x, i) == 1
          startbit = i;
          restbits = i-n-1;
          break
        endif
      end

      i2_x = bitshift(i1_x, -restbits);
      i3_x = bitxor(i2_x, g_x);

      for i=restbits:-1:1
        i4_x = bitshift(i3_x, 1);
        i5_x = bitxor(i4_x, g_x);
        i3_x = i5_x;
      end
      d_x(a,b) = i3_x;
    end
  end
end


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

