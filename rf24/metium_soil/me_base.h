#ifndef __ME_BASE_H__
#define __ME_BASE_H__

  #if defined( __AVR_ATtiny25__ ) || defined( __AVR_ATtiny45__ ) || defined( __AVR_ATtiny85__ )
     #define _ME_Tiny_
  #else
     #define _ME_Mega_
  
  #endif



#endif // __ME_BASE_H__
