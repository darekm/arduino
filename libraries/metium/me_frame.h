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


struct MemsValue_t {                  // Structure of our payload
   byte action;
   byte address;
   word time;
   word value[3] ;

  unsigned long ms;
};

String ToString( MemsValue_t * t){
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
