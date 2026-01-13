//
//  BLEUtils.m
//  FTTransmission
//
//  Created by wangan on 2025/6/6.
//

#import <Foundation/Foundation.h>

#import "GDBLEUtils.h"
static bool  g_isPrintLog = true;
#pragma -mark 计时函数
unsigned long  ADDPRE(getMilSeconds())
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
bool ADDPRE(isPrintLog())
{
    return g_isPrintLog;
}
void ADDPRE(SetPrintLog(bool print))
{
    g_isPrintLog = print;
}
void ADDPRE(i2dw(int value, unsigned char buffer[]))
{
    buffer[0] = (unsigned char) value & 0xFF;
    buffer[1] = (unsigned char) (value >> 8) & 0xFF;
    buffer[2] = (unsigned char) (value >> 16) & 0xFF;
    buffer[3] = (unsigned char) (value >> 24) & 0xFF;
} /* i2dw */

unsigned char ADDPRE(EDC(void const* p,int size))
{
    unsigned char* p1 = (unsigned char*)p;
    unsigned char ret = 0;
    for(int i=0;i<size;++i)
        ret^=p1[i];
    
    return ret;
}
void ADDPRE(HexToStr)(char *szDest, unsigned char *pbSrc, unsigned int dwLen)
{
    char	ddl,ddh;
    
    for (int i=0; i<dwLen; i++)
    {
        ddh = 48 + pbSrc[i] / 16;
        ddl = 48 + pbSrc[i] % 16;
        if (ddh > 57)  ddh = ddh + 7;
        if (ddl > 57)  ddl = ddl + 7;
        szDest[i*2] = ddh;
        szDest[i*2+1] = ddl;
    }
    
    szDest[dwLen*2] = '\0';
}
