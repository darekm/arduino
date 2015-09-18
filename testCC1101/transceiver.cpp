//
//    FILE: transceiver.cpp
// VERSION: 0.1.00
// PURPOSE: DTransceiver library for Arduino
//
// DATASHEET: 
//
// HISTORY:
// 0.1.0 by Dariusz Mazur (01/09/201)
// inspired by DHT11 library
//

#include "transceiver.h"


/////////////////////////////////////////////////////
//
// PUBLIC
//


void Transceiver::Init(CC1101 & cc)
{
  cc1101=&cc;
  cc1101->Init();
  cc1101->StartReceive(RECEIVE_TO);

}

void Transceiver::StartReceive()
{
  cc1101->StartReceive(RECEIVE_TO);
}

int Transceiver::GetData()
{
  if (cc1101->GetState() == CCGOTPACKET)
  {
    rSize=cc1101->GetData((uint8_t*)&RX_buffer);
    return rSize;
//  packet_t * pPacket;

  } else{
    return 0;
  }
}

bool Transceiver::Valid()
{
        pPacket = &RX_buffer.packet;
        pHeader = &pPacket->header;

      return ((RX_buffer.len>=sizeof(header_t)) && (RX_buffer.len<=sizeof(packet_t)));
}


int Transceiver::crcCheck()
{
          unsigned short cnt = pHeader->crc;
          pHeader->crc = 0;
//          unsigned short c=42;
//          for(unsigned short i=0 ; i<RX_buffer.len ; i++) c+=((uint8_t*)pPacket)[i];

          //valid packet crc
          return (CRC(*pPacket)-cnt);

}

unsigned short Transceiver::GetLen(packet_t & p)
{
  return (sizeof(header_t)+p.header.len);
}

unsigned short Transceiver::CRC(packet_t & p)
{
    unsigned short c=42;
    for(unsigned short i=0 ; i<(sizeof(header_t)+p.header.len) ; i++)
    {
      c+=((uint8_t*)&p)[i];
    }
    return c;
 
}  


float Transceiver::Rssi()
{
            crc = pPacket->data[pHeader->len+1];
            unsigned short c = pPacket->data[pHeader->len];
            rssi = c;
            if (c&0x80) rssi -= 256;
            rssi /= 2;
            rssi -= 74;
            return rssi;

}

void Transceiver::PrepareTransmit(uint8_t src,uint8_t dst)
{
   TX_buffer.len=GetLen(TX_buffer.packet);
   
//       txHeader->src = MID;
//    txHeader->dest = TID;
   TX_buffer.packet.header.src=src;
   TX_buffer.packet.header.dest=dst;
//   sizeof(header_t)+txHeader->len;
   TX_buffer.packet.header.crc=CRC(TX_buffer.packet);
}   



unsigned char Transceiver::Transmit()
{
   return cc1101->Transmit((uint8_t*)&(TX_buffer.packet),TX_buffer.len); 
}

//
// END OF FILE
//
