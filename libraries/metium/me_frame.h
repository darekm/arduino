/*
 Copyright (C) 2015 J. Coliz <maniacbug@ymail.com>

 */
 
/**
 * @file me_frame.h
 *
 * Setup necessary to direct stdout to madar library
 */

#ifndef __MEFRAME_H__
#define __MEFRAME_H__


struct me_payload_t {                  // Structure of our payload
  unsigned long ms;
  unsigned long counter;
  unsigned long from;
  unsigned long value[3];
};

String ToString( me_payload_t * t){
 String m="values::" ;
   for(int i=0;i<3;i++){
      m +="   v:";
      m.concat(String(i));
      m+="=";
      m.concat(String ((*t).value[i]));
    }

  return m;
}

#endif // __MEFRAME_H__
