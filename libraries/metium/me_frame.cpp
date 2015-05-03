

/*
 Copyright (C) 2015 Dariusz Mazur <darekm@emadar.com>

 */


#include "me_frame.h"
 


void Frame::setStream(Stream &s){
  _stream= &s;
}
String Frame::ToString( MemsValue_t & t){
 String m="values::" ;
   for(int i=0;i<3;i++){
      m +="   v:";
      m.concat(String(i));
      m+="=";
      m.concat(String (t.value[i]));
    }

  return m;
};


void Frame::write(MemsValue_t &t){
  _stream->write((const unsigned char*)& t,sizeof(MemsValue_t));
}
int Frame::FromFrame(String& s,MemsValue_t& t){
   unsigned char buffer;

   if (s.length()==sizeof(MemsValue_t)){
     char * p;
     p=(char *)s.c_str();
     memcpy(p, &t, sizeof(MemsValue_t)); // copy the current cube into a buffer.
     return 0;

   } else   {
     return -1;
   }



};
String Frame::ToFrame( MemsValue_t & t){
//   unsigned char buffer[2*sizeof(MemsValue_t)];
//   buffer[sizeof(MemsValue_t)]=0;
//   memcpy(&t, &buffer, sizeof(MemsValue_t)); // copy the current cube into a buffer.
//   char * p = (char *)(&buffer);
  String m;
  char *p;
   p=(char *) (&t);
   for(int i=0;i<sizeof(MemsValue_t);i++){
      m+=' ';
      m.setCharAt(i,*p);
      p++;
   }




  return m;
};



