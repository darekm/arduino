/*
 Copyright (C) 2015 Dariusz Mazur <darekm@emadar.com>

 */
 
/**
 * @file me_frame.h
 *
 * Setup necessary to direct stdout to madar library
 */

#ifndef __MEFRAME_H__
#define __MEFRAME_H__
#include "Arduino.h"

typedef struct  {                  // Structure of our payload
   byte action;
   byte address;
   word time;
   word value[3] ;

  unsigned long ms;
}MemsValue_t;

class FrameStream :  public Stream{
};
class Frame {
    private:
    Stream * _stream;
    static const int _buffer_max = 128;
    static char _buffer[_buffer_max];
    unsigned char _yp, _ym, _xm, _xp;
    public:
    void setStream( Stream & s);
    String ToFrame(MemsValue_t& t);
    void write(MemsValue_t& t);
    String ToString(MemsValue_t &t);
    int FromFrame(String& s,MemsValue_t &t);
};





#endif // __MEFRAME_H__
