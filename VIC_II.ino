void send_uint8(const uint8_t *data, const size_t data_length) {
  server.send_P(200,"application/octet-stream",(const char*)data, data_length);
}



void dumpScreenRam(){
  for (int i=0; i<1000; i++){
    uint8_t petscii = RAM[i + 1024];
    if (petscii<32) petscii = petscii + 64;
    Serial.print (char(petscii));
    int col=i % 40;
    if (col==39) Serial.println();
  }
}

extern void nmi6502();

void VICIIRefresh(){
  wdt_disable();

//server.setContentLength(CONTENT_LENGTH_UNKNOWN);
//handle keypresses
  if (server.args()){
    bool restore_pressed = false;
     //Serial.println("keyDown: ");
     //code using http://commodore64.se/wiki/index.php/Commodore_64_RAM_Addresses    
     int nrkeys=0;  
     for (uint8_t i=0; i<server.args(); i++){ 
       if (nrkeys<10 && server.argName(i).equals("k")){
         int key=server.arg(i).toInt();         
         //if (key>63 && key<94) key = key - 64;
         //make all upper case
         if (key>96 && key<123) key = key - 32;
         char buff[0xff];
         sprintf(buff,"Key %d", key);
         Serial.println(buff);
         key = key & 0x7F;
         if(key == 8) {
          key = 20; // backspace -> c64 backspace 
         }
         if(key == 27) {
          key = 3; // esc -> run/stop
          restore_pressed = true;
         }
         RAM[631+nrkeys] = key ;//& 0x7F;//put key in buffer
         nrkeys++;         
/*         
         Serial.print("accept: ");
         Serial.print(server.arg(i).toInt());
         Serial.print(" > ");
         Serial.println(key);
       }else{
         //Serial.print("reject: ");
         //Serial.print(server.argName(i));              
         //Serial.print("=");
         //Serial.println(server.arg(i));
*/         
       }
     }      
     RAM[198]=nrkeys;//number keys put in buffer
     if(restore_pressed) {
       Serial.println("NMI");
       nmi6502();
     }
      

  }

//245-246 vector keyboard decode table
//655-656 vector keyboard table setup
//648 top of screen memory
//678 PAL(1)/NTSC(0)
//2024-?2047? sprite memory

  //send video ram

  int m=0;
  uint8_t VICRAM[1024];  
  VICRAM[0]=RAM[214];//line number of cursor (read/write)
  VICRAM[1]=RAM[211];//col number of cursor (read/write)
  VICRAM[2]=RAM[243];//low byte of location of color ram (RAM[244]*256+RAM[243], 55296-56319 default )
  VICRAM[3]=RAM[244];//high byte of location of color ram (RAM[244]*256+RAM[243], 55296-56319 default )
  VICRAM[4]=RAM[646];//text color code (read/write)
  VICRAM[5]=HIGHVIC[0];//53280 border color
  VICRAM[6]=HIGHVIC[1];//53281 background (0)
  VICRAM[7]=HIGHVIC[2];//53282 background 1 (MC & ECM)
  VICRAM[8]=HIGHVIC[3];//53283 background 2 (MC & ECM)
  VICRAM[9]=HIGHVIC[4];//53284 background 3 (MC & ECM)
  m=24;
  for (int i=1024;i<2023;i++){//1024-2023 char ram
    uint8_t petscii = RAM[i];
    if (petscii<32) petscii = petscii + 64;
    VICRAM[m]=petscii;
    m++;
  }
  send_uint8(VICRAM,1024); 
  //dumpScreenRam();
  wdt_enable(5000);  
} 

/*    
    "function dumpVICRAM(){\n"
    "  var msg='';\n" 
    "  for (var i=0; i<120; i++){\n"
    "    var petscii = VICRAM[i];\n"
    "    if (petscii<32) petscii = petscii + 64;\n"    
    "    msg=msg+String.fromCharCode(petscii);\n"
    "    var col=i % 40;\n"
    "    if (col==39) msg=msg+'<BR>';\n"
    "  }\n"
    "  printLog(msg);\n"
    "}\n"
*/    

//Very nice articles about graphics mode of the VIC-II
//memory requirements of different modes: http://www.filfre.net/2013/08/how-things-work-commodore-64-and-summer-games-edition/
//location of screen RAM @ 53272 (https://www.c64-wiki.com/index.php/Screen_RAM)
//how it works: http://dustlayer.com/c64-coding-tutorials/2013/4/8/episode-2-2-writing-to-the-c64-screen
