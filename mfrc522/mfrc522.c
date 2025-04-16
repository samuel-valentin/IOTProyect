/*
 * MFRC522.c
 *
 *  Created on: Oct 15, 2015
 *      Author: B31117
 */


#include "mfrc522.h"
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/ioctl.h>
#include <stdint.h>
#include <string.h>

#include <linux/gpio.h>
// #define SPEED 10000
#define SPEED 1000000
#define BITS 8

/*Global Variables */
static int fd_spi;
static int fd_gpio_chip;
static struct gpiohandle_request req;

static int setReset(int value) {
    int ret;
    struct gpiohandle_data data;
    data.values[0] = value;
	ret = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);

    return ret;
}

static int  vfnSPIFullDuplex_Transfer(unsigned char * tx,  unsigned char * rx, int size) {
    int ret = 0;
    
    struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = size,
		.delay_usecs = 0,
		.speed_hz = SPEED,
		.bits_per_word = BITS,
	};

	ret = ioctl(fd_spi, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		perror("can't send spi message");

    return ret;

}
 
 void delay_us(short u16time)
 {
    usleep(u16time);
 }
 
 unsigned  MFRC522_Rd(unsigned char Address)  
 {
    unsigned int ucResult = 0;
    unsigned char TxBuff[2];
    unsigned char RxBuff[2];
    
    TxBuff[0] = ( (Address<<1)&0x7E) | 0x80;
    vfnSPIFullDuplex_Transfer(&TxBuff[0] , &RxBuff[0] , 2);
    
    ucResult = RxBuff[1];
    
    return ucResult;
 }
 
 void MFRC522_Wr(unsigned char Address, unsigned char value)
 {
    
        
        unsigned char TxBuff[2];
        unsigned char RxBuff[2];
        
        TxBuff[0] = ( (Address<<1)&0x7E);
        TxBuff[1] = value;
        
        vfnSPIFullDuplex_Transfer(&TxBuff[0] , &RxBuff[0] , 2);
   
 }
  void MFRC522_Clear_Bit( char addr, char mask )
 {     unsigned char  tmp =0x0; 
       tmp=MFRC522_Rd( addr ) ;
      MFRC522_Wr( addr,  tmp&~mask );    
 }
  void MFRC522_Set_Bit( char addr, char mask )
 {    unsigned char  tmp =0x0; 
       tmp=MFRC522_Rd( addr ) ;  
      MFRC522_Wr( addr, tmp|mask );
 }
 void MFRC522_Reset()
 {  
   // output_bit (MFRC522_RST, 1) ;
    setReset(1);
    delay_us (200);
    //output_bit (MFRC522_RST, 0) ;
    setReset(0);
    delay_us (200);
    //output_bit (MFRC522_RST, 1) ;
    setReset(1);
    delay_us (200);
    MFRC522_Wr( COMMANDREG, PCD_RESETPHASE ); 
    delay_us (200);
 }
 void MFRC522_AntennaOn()
 {                                               
 unsigned char stt;
 stt= MFRC522_Rd( TXCONTROLREG ) ;
 MFRC522_Set_Bit( TXCONTROLREG, 0x03 ); 
 }
 void MFRC522_AntennaOff()
 {
  MFRC522_Clear_Bit( TXCONTROLREG, 0x03 );                                           
 }
 void MFRC522_Init(char * gpio_chip, int gpio_line, char * spidev)     
 {

    uint32_t mode = SPI_MODE_0;
    uint8_t  bits = 8;
    uint32_t speed = 10000;
    uint16_t delay = 0;
                      
    fd_gpio_chip = open(gpio_chip, 0);
    fd_spi = open(spidev, O_RDWR);

    /*Init SPI */
        
    int ret = ioctl(fd_spi, SPI_IOC_WR_MODE32, &mode);

    // bits per word
    ret = ioctl(fd_spi, SPI_IOC_WR_BITS_PER_WORD, &bits);

    // max speed hz
    ret = ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    /*Configure GPIO for Reset */
    req.lineoffsets[0] = gpio_line;
	req.flags = GPIOHANDLE_REQUEST_OUTPUT;
	strcpy(req.consumer_label, "rfid_reset");
	req.lines  = 1;
	ret = ioctl(fd_gpio_chip, GPIO_GET_LINEHANDLE_IOCTL, &req);

     //output_bit(MFRC522_CS , 1);  
     //output_bit( MFRC522_RST , 1);
     setReset(1);
     
      MFRC522_Reset();        
      MFRC522_Wr( TMODEREG, 0x8D );      //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
      MFRC522_Wr( TPRESCALERREG, 0x3E ); //TModeReg[3..0] + TPrescalerReg 
      MFRC522_Wr( TRELOADREGL, 30 ); 
      MFRC522_Wr( TRELOADREGH, 0 );  
      MFRC522_Wr( TXAUTOREG, 0x40 );    //100%ASK
      MFRC522_Wr( MODEREG, 0x3D );      // CRC valor inicial de 0x6363
         
    
      MFRC522_AntennaOff() ;             
      MFRC522_AntennaOn();
 }
 char MFRC522_ToCard( char command, char *sendData, char sendLen, char *backData, unsigned *backLen )
 {
   char _status = MI_ERR;
   char irqEn = 0x00;
   char waitIRq = 0x00;                
   char lastBits;
   char n;
   unsigned i;
   
   switch (command)
   {
     case PCD_AUTHENT:       //Certification cards close
     {
       irqEn = 0x12;
       waitIRq = 0x10;
       break;
     }
     case PCD_TRANSCEIVE:    //Transmit FIFO data
     {
       irqEn = 0x77;
       waitIRq = 0x30;
       break;
     }
     default:
       break;
   }
   MFRC522_Wr( COMMIENREG, irqEn | 0x80 );  //Interrupt request
   MFRC522_Clear_Bit( COMMIRQREG, 0x80 );   //Clear all interrupt request bit
   MFRC522_Set_Bit( FIFOLEVELREG, 0x80 );   //FlushBuffer=1, FIFO Initialization
   MFRC522_Wr( COMMANDREG, PCD_IDLE );      //NO action; Cancel the current command???
   
   
   
   
   for ( i=0; i < sendLen; i++ )
   {
     MFRC522_Wr( FIFODATAREG, sendData[i] );
   }
   
   MFRC522_Wr( COMMANDREG, command );
   if (command == PCD_TRANSCEIVE )
   {
     MFRC522_Set_Bit( BITFRAMINGREG, 0x80 ); //StartSend=1,transmission of data starts  
   }
  
   i = 0xFFFF;   
   do
   {
  
     n = MFRC522_Rd( COMMIRQREG );
     i--;
   }
   while ( i && !(n & 0x01) && !( n & waitIRq ) );
   MFRC522_Clear_Bit( BITFRAMINGREG, 0x80 );    
   if (i != 0)
   {
     if( !( MFRC522_Rd( ERRORREG ) & 0x1B ) ) 
     {
       _status = MI_OK;
       if ( n & irqEn & 0x01 )
       {
         _status = MI_NOTAGERR;       
       }
       if ( command == PCD_TRANSCEIVE )
       {
         n = MFRC522_Rd( FIFOLEVELREG );
         lastBits = MFRC522_Rd( CONTROLREG ) & 0x07;
         if (lastBits)
         {
           *backLen = (n-1) * 8 + lastBits;
         }
         else
         {
           *backLen = n * 8;
         }
         if (n == 0)
         {
           n = 1;
         }
         if (n > 16)
         {
           n = 16;
         }
        
         for (i=0; i < n; i++)
         {
           backData[i] = MFRC522_Rd( FIFODATAREG );
         }
   
   backData[i] = 0;
       }
     }
     else
     {
       _status = MI_ERR;
     }
   }
  
   return _status;
 }
 char MFRC522_Request( char reqMode, char *TagType )
 {
   char _status;
   unsigned backBits;            
   MFRC522_Wr( BITFRAMINGREG, 0x07 ); 
   TagType[0] = reqMode;
   _status = MFRC522_ToCard( PCD_TRANSCEIVE, TagType, 1, TagType, &backBits );
   if ( (_status != MI_OK) || (backBits != 0x10) )
   {
     _status = MI_ERR;
   }
   return _status;
 }
 void MFRC522_CRC( char *dataIn, char length, char *dataOut )
 {
 char i, n;
     MFRC522_Clear_Bit( DIVIRQREG, 0x04 );
     MFRC522_Set_Bit( FIFOLEVELREG, 0x80 );    
     
      
     for ( i = 0; i < length; i++ )
     {   
         MFRC522_Wr( FIFODATAREG, *dataIn++ );   
     }
     
     MFRC522_Wr( COMMANDREG, PCD_CALCCRC );
         
     i = 0xFF;
   
     do 
     {
         n = MFRC522_Rd( DIVIRQREG );
         i--;
     }
     while( i && !(n & 0x04) );        //CRCIrq = 1
         
     dataOut[0] = MFRC522_Rd( CRCRESULTREGL );
     dataOut[1] = MFRC522_Rd( CRCRESULTREGM );        
 }
 char MFRC522_SelectTag( char *serNum )
 {
   char i;
   char _status;
   char size;
   unsigned recvBits;
   char buffer[9];
   
   
   
   buffer[0] = PICC_SElECTTAG;
   buffer[1] = 0x70;
   
   for ( i=2; i < 7; i++ )
   {
     buffer[i] = *serNum++;
   }
   
   MFRC522_CRC( buffer, 7, &buffer[7] );             
   
   _status = MFRC522_ToCard( PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits );
   if ( (_status == MI_OK) && (recvBits == 0x18) )
   {
     size = buffer[0];
   }
   else
   {
     size = 0;
   }
   return size;
 }
 //hibernation
 void MFRC522_Halt()
 {
   unsigned unLen;
   char buff[4];
   
   buff[0] = PICC_HALT;
   buff[1] = 0;
   MFRC522_CRC( buff, 2, &buff[2] );
   MFRC522_Clear_Bit( STATUS2REG, 0x80 );
   MFRC522_ToCard( PCD_TRANSCEIVE, buff, 4, buff, &unLen );
   MFRC522_Clear_Bit( STATUS2REG, 0x08 );
 }
 
 char MFRC522_AntiColl( char *serNum )
 {
   char _status;
   char i;
   char serNumCheck = 0;
   unsigned unLen;
   MFRC522_Wr( BITFRAMINGREG, 0x00 );                //TxLastBists = BitFramingReg[2..0]
   serNum[0] = PICC_ANTICOLL;
   serNum[1] = 0x20;
   MFRC522_Clear_Bit( STATUS2REG, 0x08 );
   _status = MFRC522_ToCard( PCD_TRANSCEIVE, serNum, 2, serNum, &unLen );
   if (_status == MI_OK)
   {
     for ( i=0; i < 4; i++ )
     {
       serNumCheck ^= serNum[i];
     }
     
     if ( serNumCheck != serNum[4] )
     {
       _status = MI_ERR;
     }
   }
   return _status;
 }
 
 char MFRC522_isCard( char *TagType ) 
 {
     if (MFRC522_Request( PICC_REQIDL, TagType ) == MI_OK)
         return 1;
     else
         return 0; 
 }
 char MFRC522_ReadCardSerial( char *str )
 {
 char _status; 
  _status = MFRC522_AntiColl( str );
  str[5] = 0;
  if (_status == MI_OK)
   return 1;
  else
   return 0;
 }
 