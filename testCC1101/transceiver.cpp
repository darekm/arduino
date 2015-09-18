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


void Transceiver::Init()
{
  cc1101=cc;
  cc1101.Init();
  cc1101.StartReceive(RECEIVE_TO);

}

void Transceiver::StartReceive()
{
  cc1101.StartReceive(RECEIVE_TO);
}

int Transciever::GetData()
{
  if (cc1101.GetState() == CCGOTPACKET)
  {
    rCount=cc1101.GetData((uint8_t*)&RX_buffer);
  packet_t * pPacket;

  } else{
    return 0;
  }


bool Transciever::Valid()
{
        pPacket = &RX_buffer.packet;
        pHeader = &pPacket->header;

      return ((RX_buffer.len>=sizeof(header_t)) && (RX_buffer.len<=sizeof(packet_t)))
}


int Transciever::crcCheck()
{
          int cnt = pHeader->crc;
          pHeader->crc = 0;
          int crc=42;
          for(int i=0 ; i<RX_buffer.len ; i++) crc+=((uint8_t*)pPacket)[i];

          //valid packet crc
          return (crc-cnt);

}

int Transciever::Rssi()
{
            crc = pPacket->data[pHeader->len];
            rssi = crc;
            if (crc&0x80) rssi -= 256;
            rssi /= 2;
            rssi -= 74;
            return rssi;

}

//
// END OF FILE
//
