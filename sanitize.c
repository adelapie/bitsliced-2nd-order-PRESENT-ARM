

#include <stdio.h>

int bit_isset(int bitidx, char *buf)
{
  int i = bitidx / 8;
  //int j = bitidx % 8;

  int j = 7 - (bitidx % 8);

  return ((buf[i] & (1 << j)) != 0);
}

void hex_to_bit_array(unsigned char *input, unsigned *output)
{

 int i = 0;

 for(i=0; i<64; i++)
    output[63 - i] = bit_isset(63 - i, input);

}

void bit_array_to_hex(unsigned *bitsliced, unsigned char *output)
{

 unsigned data1 = 0, data2 = 0;
 unsigned word = 0;
 int idx = 0;

 for (idx = 0; idx < 32; ++idx) {
  word = bitsliced[idx];
  data1 |= (word & 1) << (31 - idx);
 }

 output[0] = (data1 >> 24) & 0xFF;
 output[1] = (data1 >> 16) & 0xFF;
 output[2] = (data1 >> 8) & 0xFF;
 output[3] = data1 & 0xFF;

 for (idx = 32; idx < 64; ++idx) {
  word = bitsliced[idx];
  data2 |= (word & 1) << (31 - (idx - 32));
 }

 output[4] = (data2 >> 24) & 0xFF;
 output[5] = (data2 >> 16) & 0xFF;
 output[6] = (data2 >> 8) & 0xFF;
 output[7] = data2 & 0xFF;

}