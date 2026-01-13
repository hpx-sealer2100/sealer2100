//
//  BLEUtils.h
//  FTTransmission
//
//  Created by wangan on 2025/6/6.
//

#ifndef GDBLEUtils_h
#define GDBLEUtils_h
#include <sys/time.h>



#define __PASTE(x,y)                           x##y
#define ADDPRE(name)                           __PASTE(GD_, name)


#pragma -mark 计时函数
unsigned long  ADDPRE(getMilSeconds());
bool ADDPRE(isPrintLog());
void ADDPRE(SetPrintLog(bool print));
void ADDPRE(i2dw(int value, unsigned char buffer[]));
unsigned char ADDPRE(EDC(void const* p,int size));
void ADDPRE(HexToStr)(char *szDest, unsigned char *pbSrc, unsigned int dwLen);

#endif /* GDBLEUtils_h */
