clear all;

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
c_x = zeros(13,4);
r_x = zeros(13,4);
s_x = zeros(13,4);

for a=1:1:13
  for b=1:1:4
    if i_x(a,b) != 0
      i1_x = bitshift(uint16(i_x(a,b)), 6);
      %startbit = m;
      restbits = 0; %Number 0 bits for the division
      for i=m:-1:n
        if bitget(uint16(i1_x), i) == 1
          %startbit = i;
          restbits = i-n-1;
          if restbits < 0
            break;
          endif
          gs_x = bitshift(uint16(g_x), restbits);
          i3_x = bitxor(uint16(i1_x), uint16(gs_x));
          i1_x = i3_x;
        endif
      end
      d_x(a,b) = i1_x;
      c_x(a,b) = bitshift(uint16(i_x(a,b)),n) + d_x(a,b);
    end
  end
end

G_nsys = [ 0b110111100000;
           0b011011110000;
           0b001101111000;
           0b000110111100;
           0b000011011110;
           0b000001101111; ];

G_ksys = [ 0b100000000111;
           0b010000110100;
           0b001000011010;
           0b000100001101;
           0b000010110001;
           0b000001101111; ];

H = [ 0b010011100000;
      0b011010010000;
      0b001101001000;
      0b110101000100;
      0b101001000010;
      0b100111000001; ];
      
r_x = c_x;

for a=1:1:13
  for b=1:1:4
    if r_x(a,b) != 0
      for c=1:1:3 % 3 bit error
        ri = randi(12, 1, 1);
        if bitget(uint16(r_x(a,b)), ri) == 1
          r_x(a,b) = bitset(uint16(r_x(a,b)),ri,0);
        else
          r_x(a,b) = bitset(uint16(r_x(a,b)),ri,1);
        endif
      end
    endif
  end
end

for a=1:1:13
  for b=1:1:4
    for k=1:1:n
      test = bitand(uint16(r_x(a,b)),uint16(H(k)));
      cntbit = 0;
      for i=1:1:m
        bg = bitget(uint16(test), i);
        if bg == 1
          cntbit++;
        endif
      end
      if bitget(uint16(cntbit), 1) == 1
        s_x(a,b) = bitset(uint16(0),k);
      endif
    end
  end
end

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

