/*

 EspClass::getFreeHeap



   /*
    *  send first static frame, then a pause
    *  send second frame with the command, then a long pause
    *  send first frame again, then a pause
    *  send second frame with toggle bit and checksum
    * 
    * 
unsigned int = 16 bit = 0 -> 65535
uint8_t = unsigned int 8 bit  0 -> 255
1 byte = 8 bit (0000 0000) ( da 0 -> 255 , 0x00 ->  0xFF) 
4 bit = metà di un byte, formano un nibble.  (0000)


D = Device code
S = Subdevice code
F = Function code (otherwise called OBC or Original Button Code)  OBC1   OBC2 
C = Checksum
T = Toggle.
 (<0:1|0:1,-1|0:1,-2|0:1,-3|0:1,-4|0:1,-5|0:1,-6|0:1,-7|0:1,-8|0:1,-9|0:1,-10|0:1,-11|0:1,-12|0:1,-13|0:1,-14|0:1,-15> \
  (T=0,((
         S:4:4,C1:4,S:4,15:4,OEM:8,D:8,210u,-13.8m,
         S:4:4,C2:4,T:4,S:4,F:16,      210u,[-80.4m][-80.4m][-13.8m],T=8)+,T=9)2)) \


 {C1=-(15+S+S::4+15+OEM+OEM::4+D+D::4),C2=-(15+S+S:4+T+F+F::4+F::8+F::12)} 
 {C1=-(S+S::4+15+OEM+OEM::4+D+D::4),C2=-(S+S:4+T+F+F::4+F::8+F::12)}
 

  OBC1 = command
  OBC2 = 0
  OEM = 0x41
  SUBADDR = 0x20
  DEVICE = 0x31


      subaddr  = n[0] << 4 | n[2];
      subaddr2 = n[8] << 4 | n[11];
      oem      = n[4] << 4 | n[5];
      addr     = n[6] << 4 | n[7];
      toggle   = n[10];
      obc1 = n[12] << 4 | n[13];
      obc2 = n[14] << 4 | n[15];
      
      n[3] = 15
      n[11]= 0
      n[1] = checksum
      n[9] = checksum 


      15+2  15+4+4  3+1  
      17      23     4   = 44

      0010 1100  --> 44
      1101 0011  --> complemento di 15 + altri 7 nibbles 
           0011  --> mod 16
      


      2 + 2 + 5 = 9 + 15 = 24
      0001 1000 --> 24
      1110 0111
           0111 = 7 

           2 7 0 0 2 5 0 0  

           n = 0x25;

         c1 = n >> 4;
         c2 = n & 15;

         

check = ~(15 + 2 + c1 + c2) & 15;

  frame2[4] = command >> 4;
  frame2[5] = command & 0xFF;




byte x = 0xA7;  // For example...
byte nibble1 = (byte) (x & 0x0F);
byte nibble2 = (byte)((x & 0xF0) >> 4);
// Or alternatively...
nibble2 = (byte)((x >> 4) & 0x0F);
byte original = (byte)((nibble2 << 4) | nibble1);


  OBC1 = command
  OBC2 = 0
  OEM = 0x41
  SUBADDR = 0x20
  DEVICE = 0x31

//  {C1=-(15+S+S::4+15+OEM+OEM::4+D+D::4),C2=-(15+S+S:4+T+F+F::4+F::8+F::12)} 
//  {C1=-(S+S::4+15+OEM+OEM::4+D+D::4),C2=-(S+S:4+T+F+F::4+F::8+F::12)}


       C1 = ~(0x0F + (SUBADDR >> 4) + (SUBADDR & 0x0F) + 15  + (OEM & 0x0F) + (OEM >> 4) + (DEVICE & 0x0F) + (DEVICE >> 4) ) & 0x0F;

       C2 = ~(0x0F + (SUBADDR >> 4) + (SUBADDR & 0x0F) + 0 + (COMMAND & 0x0F) + (COMMAND >> 4) + 0 + 0) & 0x0F ;

       C3 = ~(0x0F + (SUBADDR >> 4) + (SUBADDR & 0x0F) + 8 + (COMMAND & 0x0F) + (COMMAND >> 4) + 0 + 0) & 0x0F ;


      unsigned int frame1[16] = { 
                                  SUBADDR >> 4, checksum(true), SUBADDR & 0x0F,             15,     OEM >> 4,     OEM & 0x0F, DEVICE >> 4, DEVICE & 0x0F,
                                  SUBADDR >> 4, checksum(false),              0, SUBADDR & 0x0F, COMMAND >> 4, COMMAND & 0x0F,           0,             0 };

      unsigned int frame2[16] = { 
              SUBADDR >> 4, checksum(true), SUBADDR & 0x0F, 15, OEM >> 4, OEM & 0x0F, DEVICE >> 4, DEVICE & 0x0F ,
              SUBADDR >> 4, checksum(false), 8, SUBADDR & 0x0F, COMMAND >> 4 , COMMAND & 0x0F, 0 , 0 };

      

  
    */


  
 
void sendKey(int k){

   
   //                                           C  T     F  F
  //                      0  1  2   3  4  5  6  7       8  9  10 11 12 13 14 15
 //  unsigned int frame[16] = {2, 3, 0, 15, 4, 4, 3, 1,    2, 0, 0, 0, 0, 0, 0, 0};
  
  // frame[12] = command >> 4;
 // frame[13] = command & 15;
 // frame[10] = 0;
 // frame[9] =  16 - ( frame[8] + frame[12] + frame[13]) ;
// space(SPACE + (90 * PART));  

//                                                            X        X  X                                              X        X  X
unsigned long     code[] = { 2, 3, 0, 15, 4, 4, 3, 1, 90, 2 , 0, 0, 0, 0, 0, 0, 0, 593, 2, 3, 0, 15, 4, 4, 3, 1, 90, 2 , 0, 8, 0, 0, 0, 0, 0 };

                                 
  code[10] = key[k][0];
  code[13] = key[k][1];
  code[14] = key[k][2];
  code[28] = key[k][3];
  code[31] = key[k][4];
  code[32] = key[k][5];

  unsigned long irSignal[72];

  for (int i = 0 ; i < 36; i++){

      // 304uS HIGH   //   660us + n*136us LOW
      
      irSignal[ i*2 ]    = 304;
      irSignal[ i*2 + 1] = 660 + ( code[i] * 136); 
  }
    sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), 36);  
    space(0);
    delay(200);
}


void sendnKey(int k){

   unsigned long longcode[] = { 2, 3, 0, 15, 4, 4, 3, 1, 90, 2 , 0, 0, 0, 0, 0, 0, 0, 
                           593, 2, 3, 0, 15, 4, 4, 3, 1, 90, 2 , 0, 8, 0, 0, 0, 0, 0, 593,
                                2, 3, 0, 15, 4, 4, 3, 1, 90, 2 , 0, 8, 0, 0, 0, 0, 0 };
      


                                 
  longcode[10] = nkey[k][0];
  longcode[13] = nkey[k][1];
  longcode[14] = nkey[k][2];
  longcode[28] = nkey[k][3];
  longcode[31] = nkey[k][4];
  longcode[32] = nkey[k][5];
  longcode[46] = nkey[k][6];
  longcode[49] = nkey[k][7];
  longcode[50] = nkey[k][8];
  
  unsigned long irSignal[108];

  for (int i = 0 ; i < 54; i++){
      irSignal[ i*2 ]    = 304;
      irSignal[ i*2 + 1] = 660 + ( longcode[i] * 136); 
  }
    sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), 36);  
    space(0);
    delay(200);
    //delay(200);
  
}

unsigned int key[31][6] = {
{2568,  660,  660,  1476, 660,  660}, //0
{2432,  660,  796,  1340, 660,  792}, //1
{2292,  660,  932,  1204, 660,  932}, //2
{2156,  660,  1068, 1068, 660,  1064}, //3
{2024,  660,  1200, 928,  660,  1204}, //4
{1884,  660,  1340, 796,  660,  1340}, //5
{1748,  660,  1476, 660,  656,  1476}, //6
{1612,  660,  1612, 2704, 660,  1612}, //7
{1476,  660,  1748, 2564, 660,  1748}, //8
{1336,  660,  1888, 2432, 660,  1888}, //9
{2704,  660,  2700, 1612, 660,  2700}, //POWER    a
{1208,  1324, 1344, 2296, 1344, 1340}, //Info     b
{2160,  932,  800,  1068, 936,  796}, //UP        c
{2024,  932,  932,  932,  932,  932}, //DOWN      d
{1884,  932,  1068, 796,  932,  1068}, //LEFT     e
{1756,  932,  1208, 660,  932,  1208}, //RIGHT    f
{1612,  932,  1340,  2708,  932, 1340}, // OK     g              
{1480,  932,  1476, 2572, 932,  1480}, //ESC      h
{2156,  1064, 660,  1048, 1068, 660}, //REW       i
{1884,  1068, 932,  796,  1064, 936}, //FWD       j
{2024,  1048, 796,  932,  1068, 796}, //PAUSE     k
{1752,  1068, 1068, 660,  1064, 1064}, //PLAY     l
{1612,  1068, 1204, 2704, 1068, 1204}, //REC      m
{1476,  1068, 1340, 2564, 1064, 1340}, //STOP     n
{2024,  1204, 660,  932,  1204, 660}, //RED       o
{1864,  1208, 796,  796,  1208, 796}, //GREEN     p
{1752,  1204, 932,  660,  1188, 932}, //YELLOW    q
{1612,  1204, 1068, 2704, 1204, 1068}, //BLUE     r
{796,   644,  2432, 1888,  656,  2436}, // P+     s
{644,   660,  2568, 1756,  660,  2572}, // P-     t
{1068,  1340, 1480, 2164, 1340, 1480} //Ptxt     _
};

unsigned int nkey[6][9] = {
{1476,  1340, 1068, 2568, 1340, 1064, 2568, 1340, 1068}, // SKY         u
{1888,  1340,  660,  792, 1340,  660,  792, 1340,  660}, // GUIDA TV    v
{1616,  1340,  932, 2704, 1340,  932, 2704, 1340,  932}, // PRIMA FILA  w
{2296,   932,  660, 1204,  932,  660, 1200,  932,  660}, // MENU        x
{932,   1340, 1612, 2024, 1344, 1616, 2024, 1344, 1616},  // MY          y
{1340,  1344,  1204,  2416,  1340,  1204, 2428,  1340,  1204} // UINTERATTIVI z


}; 


    932, 1068, 660, 2704, 1204, 1204, 1064, 792, 12844, 932, 0,   660, 660, 0, 0, 660, 660, 81380,
    932, 1068, 660, 2704, 1204, 1204, 1064, 792, 12844, 932, 0,  1752, 660, 0, 0, 660, 660, 0

932, 1068, 660, 2704, 1204, 1204, 1064, 792, 12844, 932, 0,   660, 660, 0, 0, 660, 660, 81380,
                              932, 1068, 660, 2704, 1204, 1204, 1064, 792, 12844, 932, 0,  1752, 660, 0, 0, 660, 660, 81380,
                              932, 1068, 660, 2704, 1204, 1204, 1064, 792, 12844, 932, 0,  1752, 660, 0, 0, 660, 660, 0
      
int findPos(const char* f, size_t f_size, char s){

  int ret = -1;
  int i = 0;
  bool found = false;
  
  while (i < f_size && !found){

    found = (f[i++] == s);
    
  }

  if (found){
    ret = i - 1;
  }

  return ret;
}

<?php

for($i=0; $i<=70; $i++){
    
    $re[$i] = 660 + $i * 136;
    
}
print_r($re);

$handle = fopen("codes.txt", "r");
if ($handle) {
    while (($line = fgets($handle)) !== false) {
        $c = array();
        preg_match("/\{(.*)\},?(.*)/", $line, $m);
        $num = explode(",", $m[1]);
        
        echo "{ ";
        foreach($num as $n){
            
            
            $n = intval(trim($n));
            
            
            foreach($re as $k => $v){
                
                if ( ($n-30) < $v &&  ($n+30) > $v ) {    
                    $c[] = $k;
                }
            }
        }
        echo implode(", ",$c);
        echo " }, \t\t".$m[2]."\n";    
    }
    fclose($handle);
} 



http://www.hifi-remote.com/wiki/index.php?title=XMP
https://github.com/torvalds/linux/blob/master/drivers/media/rc/ir-xmp-decoder.c

XMP uses one burst pair to encode numbers 0 to 15, with an on duration of 210uS, and off duration of 760uS + n*136uS where n takes on values of 0 to 15. 
The checksum nibble is the complement of 15 plus the sum of the other 7 nibbles, mod 16 


 2 + 13 + 1 = 16
 
-16 + 
                      13, 0, 1,  => 5, 0, 1

                         8 3 3   =>  0 3 3

#define XMP_UNIT      136000  // ns 
#define XMP_LEADER      210000  // ns 
#define XMP_NIBBLE_PREFIX 760000  // ns 
#define XMP_HALFFRAME_SPACE 13800000 // ns
#define XMP_TRAILER_SPACE 20000000  //  should be 80ms but not all dureation supliers can go that high 


 {C1=-(15+S+S::4+15+OEM+OEM::4+D+D::4),C2=-(15+S+S:4+T+F+F::4+F::8+F::12)} \
  
for (i = 0; i < 16; i++)  n[i] = (n[i] - XMP_NIBBLE_PREFIX) / divider;

sum1 = (15 + n[0] + n[1] + n[2] + n[3] +n[4] + n[5] + n[6] + n[7]) % 16;
sum2 = (15 + n[8] + n[9] + n[10] + n[11] +n[12] + n[13] + n[14] + n[15]) % 16;
  
   X          X  X
2, 3,  0, 15, 4, 4, 3, 1

   13         0  1
2, 0 , 0,  0, 0, 0, 0, 0

   5             1
2, 0 , 8,  0, 0, 0, 0, 0 


2, 3, 0, 15, 4, 4, 3, 1, |  2 , 14, 0, 0, 0, 0, 0, 0, 
2, 3, 0, 15, 4, 4, 3, 1, |  2 ,  6, 8, 0, 0, 0, 0, 0




createSignal(0x0D);


void createSignal(int command){
   //                                           C  T     F  F
  //             0  1  2   3  4  5  6  7     8  9  10 11 12 13 14 15
  int frame[] = {2, 3, 0, 15, 4, 4, 3, 1,    2, 0, 0, 0, 0, 0, 0, 0};

  frame[12] = command >> 4;
  frame[13] = command & 15; 

  frame[10] = 0;
  frame[9] = (15 - frame[8] + frame[12] + frame[13]) % 16; 

   
  
}





*/
