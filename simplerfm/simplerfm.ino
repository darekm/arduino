
#include <imframe.h>
//#include <imatmega.h>





//#.define DBGLVL 2
#include "imdebug.h"



/******************************** Configuration *************************************/

#define MMAC 0x130020  // My MAC
#define ServerMAC 0xA0000  // Server  MAC
#define MDEVICE 3     //Type of device
#define MCHANNEL 2





/************************* Module specyfic functions **********************/


#include "imbuffer.h"
#include "imbufrfm69.h"


IMBuffer    buf;



#define pinLED  9
int COUNTER=0;


void SendDataFlood()
{
     static IMFrame frame;
     frame.Reset();
     COUNTER++;
     IMFrameData *data =frame.Data();
     for(byte i = 0; i < 4; i++){
        data->w[0]=100;
        data->w[2]=COUNTER;
        data->w[3]=i;
        data->w[4]=COUNTER;

     buf.TX_buffer.packet=frame;
     buf.Send();
       }
    
}


void SendData()
{
        DBGLEDON();
        static IMFrame frame;
        frame.Reset();
         COUNTER++;
         IMFrameData *data =frame.Data();
        data->w[0]=100;
        data->w[2]=COUNTER;
        data->w[3]=1;
     buf.TX_buffer.packet=frame;
     buf.Send();

  }



void ReceiveData()
{
}





void setup()
{
   resetPin();

  //pinMode(10,OUTPUT);
  //digitalWrite(10,HIGH);
  

  //setupTimer2();
  // power_timer0_enable();
  DBGLEDOFF();
      buf.Init(MCHANNEL);
//  trx.myDevice=MDEVICE;


}

void loop()
{
  delaySleep(100);
  SendData();

}
