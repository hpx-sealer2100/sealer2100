//
//  JubSDKCore+DEV_BIO.h
//  JubSDKCore
//
//  Created by Pan Min on 2020/7/6.
//  Copyright © 2020 JuBiter. All rights reserved.
//

#import "JubSDKCore.h"

#define JUBR_BIO_FINGERPRINT_MODALITY_ERROR     0x40009BE0UL
#define JUBR_BIO_SPACE_LIMITATION               0x40009BEAUL
#define JUBR_BIO_TIMEOUT                        0x40009BEEUL


NS_ASSUME_NONNULL_BEGIN


@interface JubSDKCore (DEV_BIO)


//typedef enum {
//    VIA_9GRIDS = 0x02,
//} JUB_ENUM_IDENTITY_VERIFY_MODE;
typedef NS_ENUM(NSInteger, JUB_NS_ENUM_IDENTITY_VERIFY_MODE) {
    NS_VIA_9GRIDS = 0x02,
    NS_IDENTITY_VERIFY_MODE_NS_ITEM
};


typedef struct FgptEnrollInfo {
   NSUInteger modalityID;   // Modality ID.
   NSUInteger nextIndex;    // The number of times of current fingerprint modality.
   NSUInteger times;        // Total number of times that need to enroll for current fingerprint modality.
} stFgptEnrollInfo;


/*****************************************************************************
 * @function name : JUB_IdentityVerifyPIN
 * @in  param : deviceID - device ID
 *           mode - the mode for verify identity, the following values are valid:
 *                   - JUB_ENUM_IDENTITY_VERIFY_MODE::VIA_9GRIDS
 *           pinMix: user's PIN
 * @out param : retry
 * @last change :
 *****************************************************************************/
//JUB_RV JUB_IdentityVerifyPIN(IN JUB_UINT16 deviceID,
//                             IN JUB_ENUM_IDENTITY_VERIFY_MODE mode,
//                             IN JUB_CHAR_CPTR pinMix,
//                             OUT JUB_ULONG_PTR pretry);
- (NSUInteger)JUB_IdentityVerifyPIN:(NSUInteger)deviceID
                               mode:(JUB_NS_ENUM_IDENTITY_VERIFY_MODE)mode
                             pinMix:(NSString*)pinMix;


/*****************************************************************************
 * @function name : JUB_IdentityShowNineGrids
 * @in  param : deviceID - device ID
 *           mode - the mode for verify identity
 * @out param :
 * @last change :
 *****************************************************************************/
//JUB_RV JUB_IdentityShowNineGrids(IN JUB_UINT16 deviceID);
- (void)JUB_IdentityShowNineGrids:(NSUInteger)deviceID;


/*****************************************************************************
 * @function name : JUB_IdentityCancelNineGrids
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
//JUB_RV JUB_IdentityCancelNineGrids(IN JUB_UINT16 deviceID);
- (void)JUB_IdentityCancelNineGrids:(NSUInteger)deviceID;


/*****************************************************************************
 * @function name : JUB_EnrollFingerprint
 * @in  param : deviceID - device ID
 * @inout param: fgptTimeout - timeout for fingerprint
 *             fgptIndex - The index of current fingerprint modality.
 *                    If this value is ZERO, indicate enroll a new fingerprint;
 *                      otherwise this value mast be equal to the value in
 *                      response of previous this command.
 * @out param : ptimes -  total number of times that need to enroll for current fingerprint modality.
 *            fgptID - modality ID, assigned by the device
 * @last change :
 *****************************************************************************/
//JUB_RV JUB_EnrollFingerprint(IN JUB_UINT16 deviceID,
//                             IN JUB_UINT16 fgptTimeout,
//                             INOUT JUB_BYTE_PTR fgptIndex, OUT JUB_ULONG_PTR ptimes,
//                             OUT JUB_BYTE_PTR fgptID);
- (stFgptEnrollInfo)JUB_EnrollFingerprint:(NSUInteger)deviceID
                              fgptTimeout:(NSUInteger)fgptTimeout
                                fgptIndex:(NSUInteger)fgptIndex
                                   fgptID:(NSUInteger)fgptID;

/*****************************************************************************
 * @function name : JUB_EnumFingerprint
 * @in  param : deviceID - device ID
 * @return : fingerprint list
 * @last change :
*****************************************************************************/
//JUB_RV JUB_EnumFingerprint(IN JUB_UINT16 deviceID,
//                           OUT JUB_CHAR_PTR_PTR fgptList);
- (NSString*)JUB_EnumFingerprint:(NSUInteger)deviceID;


/*****************************************************************************
 * @function name : JUB_EraseFingerprint
 * @in  param : deviceID - device ID
 *           fgptTimeout - timeout for fingerprint
 * @return :
  * @last change :
*****************************************************************************/
//JUB_RV JUB_EraseFingerprint(IN JUB_UINT16 deviceID,
//                            IN JUB_UINT16 fgptTimeout);
- (void)JUB_EraseFingerprint:(NSUInteger)deviceID
                 fgptTimeout:(NSUInteger)fgptTimeout;


/*****************************************************************************
 * @function name : JUB_DeleteFingerprint
 * @in  param : deviceID - device ID
 *           fgptTimeout - timeout for fingerprint
 *           fgptID - the modality ID of a fingerprint.
 * @return :
 * @last change :
*****************************************************************************/
//JUB_RV JUB_DeleteFingerprint(IN JUB_UINT16 deviceID,
//                             IN JUB_UINT16 fgptTimeout,
//                             IN JUB_BYTE fgptID);
- (void)JUB_DeleteFingerprint:(NSUInteger)deviceID
                  fgptTimeout:(NSUInteger)fgptTimeout
                       fgptID:(NSUInteger)fgptID;


/*****************************************************************************
 * @function name : JUB_VerifyFingerprint
 * @in  param : contextID - context ID
 * @out param : retry
 * @last change :
 *****************************************************************************/
//JUB_RV JUB_VerifyFingerprint(IN JUB_UINT16 contextID,
//                             OUT JUB_ULONG_PTR pretry);
- (NSUInteger)JUB_VerifyFingerprint:(NSUInteger)contextID;


@end

NS_ASSUME_NONNULL_END
