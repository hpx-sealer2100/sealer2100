//
//  JubSDKCore+DEV_BIO.mm
//  JubSDKCore
//
//  Created by Pan Min on 2020/7/6.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#import "JubSDKCore+DEV_BIO.h"
#import "JUB_SDK_DEV_BIO.h"


@implementation JubSDKCore (DEV_BIO)


JUB_ENUM_IDENTITY_VERIFY_MODE (^inlineIdentityVerifyMode)(JUB_NS_ENUM_IDENTITY_VERIFY_MODE) = ^(JUB_NS_ENUM_IDENTITY_VERIFY_MODE argument) {
    JUB_ENUM_IDENTITY_VERIFY_MODE mode;
    switch (argument) {
        case JUB_NS_ENUM_IDENTITY_VERIFY_MODE::NS_VIA_9GRIDS:
            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_9GRIDS;
            break;
//        case JUB_NS_ENUM_IDENTITY_VERIFY_MODE::NS_VIA_FPGT:
//            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_FPGT;
//            break;
        case JUB_NS_ENUM_IDENTITY_VERIFY_MODE::NS_IDENTITY_VERIFY_MODE_NS_ITEM:
            mode = JUB_ENUM_IDENTITY_VERIFY_MODE::NS_ITEM;
            break;
    }
    return mode;
};
//
//
////JUB_RV JUB_IdentityVerify(IN JUB_UINT16 deviceID,
////                          IN JUB_ENUM_IDENTITY_VERIFY_MODE mode,
////                          OUT JUB_ULONG_PTR pretry);
//- (NSUInteger)JUB_IdentityVerify:(NSUInteger)deviceID
//                            mode:(JUB_NS_ENUM_IDENTITY_VERIFY_MODE)mode
//{
//    self.lastError = JUBR_OK;
//
//    JUB_ULONG retry = 0;
//    JUB_RV rv = JUB_IdentityVerify(deviceID,
//                                   inlineIdentityVerifyMode(mode),
//                                   &retry);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return retry;
//    }
//
//    return retry;
//}


//JUB_RV JUB_IdentityVerifyPIN(IN JUB_UINT16 deviceID,
//                             IN JUB_ENUM_IDENTITY_VERIFY_MODE mode,
//                             IN JUB_CHAR_CPTR pinMix,
//                             OUT JUB_ULONG_PTR pretry);
- (NSUInteger)JUB_IdentityVerifyPIN:(NSUInteger)deviceID
                               mode:(JUB_NS_ENUM_IDENTITY_VERIFY_MODE)mode
                             pinMix:(NSString*)pinMix
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pPinMix = nullptr;
    if (nil != pinMix) {
        pPinMix = (JUB_CHAR_PTR)[pinMix UTF8String];
    }
    
    JUB_ULONG retry = 0;
    JUB_RV rv = JUB_IdentityVerifyPIN(deviceID,
                                      inlineIdentityVerifyMode(mode),
                                      pPinMix,
                                      &retry);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return retry;
    }
    
    return retry;
}


//JUB_RV JUB_IdentityShowNineGrids(IN JUB_UINT16 deviceID);
- (void)JUB_IdentityShowNineGrids:(NSUInteger)deviceID
{
    self.lastError = JUB_IdentityShowNineGrids(deviceID);
}


//JUB_RV JUB_IdentityCancelNineGrids(IN JUB_UINT16 deviceID);
- (void)JUB_IdentityCancelNineGrids:(NSUInteger)deviceID
{
    self.lastError = JUB_IdentityCancelNineGrids(deviceID);
}


//JUB_RV JUB_EnrollFingerprint(IN JUB_UINT16 deviceID,
//                             IN JUB_UINT16 fgptTimeout,
//                             INOUT JUB_BYTE_PTR fgptIndex, OUT JUB_ULONG_PTR ptimes,
//                             OUT JUB_BYTE_PTR fgptID);
- (stFgptEnrollInfo)JUB_EnrollFingerprint:(NSUInteger)deviceID
                            fgptTimeout:(NSUInteger)fgptTimeout
                              fgptIndex:(NSUInteger)fgptIndex
                                 fgptID:(NSUInteger)fgptID
{
    NSUInteger fgptNextIndex = fgptIndex;
    JUB_ULONG times = 0;
    NSUInteger assignedID = fgptID;
    self.lastError = JUB_EnrollFingerprint(deviceID,
                                           fgptTimeout,
                                           (JUB_BYTE_PTR)(&fgptNextIndex), &times,
                                           (JUB_BYTE_PTR)(&assignedID));
    
    return stFgptEnrollInfo{assignedID, fgptNextIndex, times};
}


//JUB_RV JUB_EnumFingerprint(IN JUB_UINT16 deviceID,
//                           OUT JUB_CHAR_PTR_PTR fgptList);
- (NSString*)JUB_EnumFingerprint:(NSUInteger)deviceID
{
    JUB_CHAR_PTR fgptList = nullptr;
    JUB_RV rv = JUB_EnumFingerprint(deviceID,
                                    &fgptList);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strFgptList = [NSString stringWithCString:fgptList
                                               encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(fgptList);
    
    return strFgptList;
}


//JUB_RV JUB_EraseFingerprint(IN JUB_UINT16 deviceID,
//                            IN JUB_UINT16 fgptTimeout);
- (void)JUB_EraseFingerprint:(NSUInteger)deviceID
                 fgptTimeout:(NSUInteger)fgptTimeout
{
    self.lastError = JUB_EraseFingerprint(deviceID, fgptTimeout);
}


//JUB_RV JUB_DeleteFingerprint(IN JUB_UINT16 deviceID,
//                             IN JUB_UINT16 fgptTimeout,
//                             IN JUB_BYTE fgptID);
- (void)JUB_DeleteFingerprint:(NSUInteger)deviceID
                  fgptTimeout:(NSUInteger)fgptTimeout
                       fgptID:(NSUInteger)fgptID
{
    self.lastError = JUB_DeleteFingerprint(deviceID, fgptTimeout, fgptID);
}


//JUB_RV JUB_VerifyFingerprint(IN JUB_UINT16 contextID,
//                             OUT JUB_ULONG_PTR pretry);
- (NSUInteger)JUB_VerifyFingerprint:(NSUInteger)contextID
{
    self.lastError = JUBR_OK;
    
    JUB_ULONG retry = 0;
    JUB_RV rv = JUB_VerifyFingerprint(contextID,
                                      &retry);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return retry;
    }
    
    return retry;
}


@end
