echo should run as administrator
cd libraries
mkdir imcore
cd imcore
mklink imatmega.cpp c:\git\imwave\Arduino\imatmega.cpp
mklink imatmega.h c:\git\imwave\Arduino\imatmega.h
mklink imeprom.cpp c:\git\imwave\Arduino\imeprom.cpp
mklink imeprom.h c:\git\imwave\Arduino\imeprom.h
mklink imdebug.h c:\git\imwave\Arduino\imdebug.h
mklink imframe.h c:\git\imwave\Arduino\imframe.h
cd ..
mkdir imwave
cd imwave
mklink imbuffer.h c:\git\imwave\Arduino\imbuffer.h
mklink imrouting.h c:\git\imwave\Arduino\imrouting.h
mklink imrouting.cpp c:\git\imwave\Arduino\imrouting.cpp
mklink imtimer.cpp c:\git\imwave\Arduino\imtimer.cpp
mklink imtimer.h c:\git\imwave\Arduino\imtimer.h
mklink imtrans.cpp c:\git\imwave\Arduino\imtrans.cpp
mklink imtrans.h c:\git\imwave\Arduino\imtrans.h
mklink imtube.cpp c:\git\imwave\Arduino\imtube.cpp
mklink imtube.h c:\git\imwave\Arduino\imtube.h

cd ..
mkdir imrfm69
cd imrfm69
mklink imbufrfm69.cpp c:\git\imwave\Arduino\rfm69\imbufrfm69.cpp
mklink imbufrfm69.h c:\git\imwave\Arduino\rfm69\imbufrfm69.h
mklink imrfm69.cpp c:\git\imwave\Arduino\rfm69\imrfm69.cpp
mklink imrfm69.h c:\git\imwave\Arduino\rfm69\imrfm69.h
mklink imrfmregisters.h c:\git\imwave\Arduino\rfm69\imrfmregisters.h
cd ..


