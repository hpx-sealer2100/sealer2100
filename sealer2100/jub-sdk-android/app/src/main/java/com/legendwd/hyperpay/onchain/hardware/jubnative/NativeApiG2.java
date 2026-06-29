package com.legendwd.hyperpay.onchain.hardware.jubnative;

import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.JUB_DEVICE_INFO;

public class NativeApiG2 {

    public static int nativeGetErrorCode() {
        return NativeApiLL.nativeGetErrorCode();
    }

    public static int nativeInitDevice() {
        return NativeApiLL.nativeInitDevice();
    }

    public static int nativeStartScan(InnerScanCallback scanCallback) {
        return NativeApiLL.nativeStartScan(scanCallback);
    }

    public static int nativeStopScan() {
        return NativeApiLL.nativeStopScan();
    }

    public static int nativeConnectDevice(String devName, String address, int devType, long[] handle, int timeout, InnerDiscCallback discCallback) {
        return NativeApiLL.nativeConnectDevice(devName, address, devType, handle, timeout, discCallback);
    }

    public static int nativeDisconnect(long deviceHandle) {
        return NativeApiLL.nativeDisconnect(deviceHandle);
    }

    public static int nativeIsConnected(long deviceHandle) {
        return NativeApiLL.nativeIsConnected(deviceHandle);
    }

    public static String nativeGetDeviceType(long deviceHandle) {
        return NativeApiLL.nativeGetDeviceType(deviceHandle);
    }

    public static int nativeShowVirtualPwd(long contextID) {
        return NativeApiLL.nativeShowVirtualPwd(contextID);
    }

    public static int nativeCancelVirtualPwd(long contextID) {
        return NativeApiLL.nativeCancelVirtualPwd(contextID);
    }

    public static int nativeVerifyPIN(long contextID, byte[] pin) {
        return NativeApiLL.nativeVerifyPIN(contextID, pin);
    }

    public static int nativeGetDeviceInfo(JUB_DEVICE_INFO info, long deviceHandle) {
        return NativeApiLL.nativeGetDeviceInfo(info, deviceHandle);
    }

    public static String nativeSendApdu(long deviceHandle, String apduStr) {
        return NativeApiLL.nativeSendApdu(deviceHandle, apduStr);
    }
    public static int nativeSetLable(long deviceHandle, String Lable) {
        return NativeApiLL.nativeSetLable(deviceHandle,Lable);
    }
    public static int nativeEndSession(long deviceHandle) {
        return NativeApiLL.nativeEndSession(deviceHandle);
    }
    public static int nativeOpCancel(long deviceHandle) {
        return NativeApiLL.nativeOpCancel(deviceHandle);
    }

    public static String nativeGetDeviceCert(long deviceHandle) {
        return NativeApiLL.nativeGetDeviceCert(deviceHandle);
    }

    public static String nativeGetDeviceSignData(long deviceHandle, String hashData) {
        return NativeApiLL.nativeGetDeviceSignData(deviceHandle,hashData);
    }


    public static String nativeGetDeviceFeatures(long deviceHandle) {
        return NativeApiLL.nativeGetFeatures(deviceHandle);
    }

    public static String nativeEnumApplets(long deviceHandle) {
        return NativeApiLL.nativeEnumApplets(deviceHandle);
    }

    public static String nativeGetAppletVersion(long deviceHandle, String appID) {
        return NativeApiLL.nativeGetAppletVersion(deviceHandle, appID);
    }

    public static int nativeSetTimeOut(long contextID, int timeout) {
        return NativeApiLL.nativeSetTimeOut(contextID, timeout);
    }

    public static String nativeEnumSupportCoins(long deviceHandle) {
        return NativeApiLL.nativeEnumSupportCoins(deviceHandle);
    }

    public static int nativeClearContext(long contextID) {
        return NativeApiLL.nativeClearContext(contextID);
    }

    public static int nativeQueryBattery(long deviceHandle, int[] battery) {
        return NativeApiLL.nativeQueryBattery(deviceHandle, battery);
    }

    public static int nativeIsInitialize(long deviceHandle) {
        return NativeApiLL.nativeIsInitialize(deviceHandle);
    }

    public static int nativeIsBootLoader(long deviceHandle) {
        return NativeApiLL.nativeIsBootLoader(deviceHandle);
    }

    public static String nativeGetVersion() {
        return NativeApiLL.nativeGetVersion();
    }

    public static int nativeBTCCreateContext(int[] contextIDs, boolean isMultiSig, String json, long deviceHandle) {
        return NativeApiLL.nativeBTCCreateContext(contextIDs, isMultiSig, json, deviceHandle);
    }

    public static String nativeBTCShowAddress(long contextID, int change, int index, boolean useLegacyAddress) {
        return NativeApiLL.nativeBTCShowAddress(contextID, change, index, useLegacyAddress);
    }

    public static String nativeBTCShowAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeBTCShowAddress(contextID, change, index);
    }

    public static String nativeBTCSetMyAddress(long contextID, int change, int index, boolean useLegacyAddress) {
        return NativeApiLL.nativeBTCSetMyAddress(contextID, change, index, useLegacyAddress);
    }

    public static String nativeBTCSetMyAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeBTCSetMyAddress(contextID, change, index);
    }

    public static String[] nativeBTCGetAddress(long contextID, boolean useLegacyAddress, String json) {
        return NativeApiLL.nativeBTCGetAddress(contextID, useLegacyAddress, json);
    }

    public static String[] nativeBTCGetAddress(long contextID, String json) {
        return NativeApiLL.nativeBTCGetAddress(contextID, json);
    }

    public static String nativeBTCGetMainHDNode(long contextID) {
        return NativeApiLL.nativeBTCGetMainHDNode(contextID);
    }

    public static String nativeBTCTransaction(long contextID, boolean useLegacyAddress, String json) {
        return NativeApiLL.nativeBTCTransaction(contextID, useLegacyAddress, json);
    }

    public static String nativeBTCTransaction(long contextID, String json) {
        return NativeApiLL.nativeBTCTransaction(contextID, json);
    }

    public static String nativeQRC20Transaction(long contextID, String json) {
        return NativeApiLL.nativeQRC20Transaction(contextID, json);
    }

    public static String nativeUSDTTransaction(long contextID, String json) {
        return NativeApiLL.nativeUSDTTransaction(contextID, json);
    }

    public static String nativeParseTransaction(long contextID, String json) {
        return NativeApiLL.nativeParseTransaction(contextID, json);
    }

    public static int nativeETHCreateContext(int[] contextIDs, String json, long deviceHandle) {
        return NativeApiLL.nativeETHCreateContext(contextIDs, json, deviceHandle);
    }

    public static String nativeETHShowAddress(long contextID, int index) {
        return NativeApiLL.nativeETHShowAddress(contextID, index);
    }

    public static String nativeETHSetMyAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeETHSetMyAddress(contextID, change, index);
    }

    public static int nativeETHSetContrAddr(long contextID, String contractAddress) {
        return NativeApiLL.nativeETHSetContrAddr(contextID, contractAddress);
    }

    public static String[] nativeETHGetAddress(long contextID, String json) {
        return NativeApiLL.nativeETHGetAddress(contextID, json);
    }

    public static String nativeETHGetMainHDNode(long contextID) {
        return NativeApiLL.nativeETHGetMainHDNode(contextID);
    }

    public static String nativeETHTransaction(long contextID, String json) {
        return NativeApiLL.nativeETHTransaction(contextID, json);
    }
    public static String nativeETHTypedData(long contextID, String json) {
        return NativeApiLL.nativeETHTypeDataMAX(contextID, json);
    }

    public static int nativeETHSetERC20Tokens(long contextID, String json) {
        return NativeApiLL.nativeETHSetERC20Tokens(contextID, json);
    }

    public static int nativeETHSetERC20Token(long contextID, String json) {
        return NativeApiLL.nativeETHSetERC20Token(contextID, json);
    }
    public static int nativeETHSetERC20TokenV2(long contextID, String json) {
        return NativeApiLL.nativeETHSetERC20TokenV2(contextID, json);
    }
    public static int nativeETHSetNetworkV2(long contextID, String json) {
        return NativeApiLL.nativeETHSetNetworkV2(contextID, json);
    }
    public static String nativeETHBuildERC20TransferAbi(long contextID, String json) {
        return NativeApiLL.nativeETHBuildERC20TransferAbi(contextID, json);
    }

    public static String nativeETHSignBytestring(long contextID, String json) {
        return NativeApiLL.nativeETHSignBytestring(contextID, json);
    }

    public static String nativeETHSignTypedData(long contextID, String json) {
        return NativeApiLL.nativeETHSignTypedData(contextID, json);
    }

    public static String nativeETHSignContract(long contextID, String json) {
        return NativeApiLL.nativeETHSignContract(contextID, json);
    }

    public static String nativeETHBuildContractWithAddrAmtDataAbi(long contextID, String json) {
        return NativeApiLL.nativeETHBuildContractWithAddrAmtDataAbi(contextID, json);
    }

    public static String nativeETHBuildContractWithTxIDAbi(long contextID, String json) {
        return NativeApiLL.nativeETHBuildContractWithTxIDAbi(contextID, json);
    }

    public static int nativeETHUploadNFT(long contextId, String meta, byte[] image, byte[] thumbnail, byte[] wallpaper) {
        return NativeApiLL.nativeETHUploadNFT(contextId, meta, image, thumbnail, wallpaper);
    }

    public static int nativeHCCreateContext(int[] contextIDs, boolean isMultiSig, String json, long deviceHandle) {
        return NativeApiLL.nativeHCCreateContext(contextIDs, isMultiSig, json, deviceHandle);
    }

    public static String[] nativeHCGetAddress(long contextID, String json) {
        return NativeApiLL.nativeHCGetAddress(contextID, json);
    }

    public static String nativeHCShowAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeHCShowAddress(contextID, change, index);
    }

    public static String nativeHCGetMainHDNode(long contextID) {
        return NativeApiLL.nativeHCGetMainHDNode(contextID);
    }

    public static String nativeHCTransaction(long contextID, String json) {
        return NativeApiLL.nativeHCTransaction(contextID, json);
    }

    public static int nativeEOSCreateContext(int[] contextIDs, String json, long deviceHandle) {
        return NativeApiLL.nativeEOSCreateContext(contextIDs, json, deviceHandle);
    }

    public static String nativeEOSShowAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeEOSShowAddress(contextID, change, index);
    }

    public static String nativeEOSGetAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeEOSGetAddress(contextID, change, index);
    }

    public static String nativeEOSSetMyAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeEOSSetMyAddress(contextID, change, index);
    }

    public static String nativeEOSTransaction(long contextID, String json) {
        return NativeApiLL.nativeEOSTransaction(contextID, json);
    }

    public static int nativeXRPCreateContext(int[] contextIDs, String json, long deviceHandle) {
        return NativeApiLL.nativeXRPCreateContext(contextIDs, json, deviceHandle);
    }

    public static String nativeXRPShowAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeXRPShowAddress(contextID, change, index);
    }

    public static String[] nativeXRPGetAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeXRPGetAddress(contextID, change, index);
    }

    public static String nativeXRPGetHDNode(long contextID, int change, int index, boolean useHex) {
        return NativeApiLL.nativeXRPGetHDNode(contextID, change, index, useHex);
    }

    public static String nativeXRPSetMyAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeXRPSetMyAddress(contextID, change, index);
    }

    public static String nativeXRPTransaction(long contextID, String json) {
        return NativeApiLL.nativeXRPTransaction(contextID, json);
    }

    public static int nativeTRXCreateContext(int[] contextIDs, String json, long deviceHandle) {
        return NativeApiLL.nativeTRXCreateContext(contextIDs, json, deviceHandle);
    }

    public static String nativeTRXShowAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeTRXShowAddress(contextID, change, index);
    }

    public static String[] nativeTRXGetAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeTRXGetAddress(contextID, change, index);
    }

    public static String nativeTRXGetHDNode(long contextID, int change, int index, boolean useHex) {
        return NativeApiLL.nativeTRXGetHDNode(contextID, change, index, useHex);
    }

    public static String nativeTRXSetMyAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeTRXSetMyAddress(contextID, change, index);
    }

    public static String nativeTRXTransaction(long contextID, String json, byte[] packedContractInPb) {
        return NativeApiLL.nativeTRXTransaction(contextID, json, packedContractInPb);
    }

    public static int nativeTRXSetTRC20Token(long contextID, String json) {
        return NativeApiLL.nativeTRXSetTRC20Token(contextID, json);
    }

    public static String nativeTRXBuildTRC20TransferAbi(long contextID, String json) {
        return NativeApiLL.nativeTRXBuildTRC20TransferAbi(contextID, json);
    }

    public static int nativeTRXSetTRC10Asset(long contextID, String json) {
        return NativeApiLL.nativeTRXSetTRC10Asset(contextID, json);
    }

    public static byte[] nativeTRXPackContract(long contextID, String json) {
        return NativeApiLL.nativeTRXPackContract(contextID, json);
    }

    //*********************************** FIL **********************************************

    public static int nativeFILCreateContext(int[] contextIDs, String json, long deviceHandle) {
        return NativeApiLL.nativeFILCreateContext(contextIDs, json, deviceHandle);
    }

    public static String nativeFILShowAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeFILShowAddress(contextID, change, index);
    }

    public static String[] nativeFILGetAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeFILGetAddress(contextID, change, index);
    }

    public static String nativeFILGetHDNode(long contextID, int change, int index, boolean useHex) {
        return NativeApiLL.nativeFILGetHDNode(contextID, change, index, useHex);
    }

    public static String nativeFILSetMyAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeFILSetMyAddress(contextID, change, index);
    }

    public static String nativeFILTransaction(long contextID, String json) {
        return NativeApiLL.nativeFILTransaction(contextID, json);
    }


    //*********************************** SOL **********************************************

    public static int nativeSOLCreateContext(int[] contextIDs, String json, long deviceHandle) {
        return NativeApiLL.nativeSOLCreateContext(contextIDs, json, deviceHandle);
    }

    public static String[] nativeSOLGetAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeSOLGetAddress(contextID, change, index);
    }

    public static String[] nativeSOLShowAddress(long contextID, int change, int index) {
        return NativeApiLL.nativeSOLShowAddress(contextID, change, index);
    }

    public static String nativeSOLGetHDNode(long contextID, int change, int index, boolean useHex) {
        return NativeApiLL.nativeSOLGetHDNode(contextID, change, index, useHex);
    }

    public static String nativeSOLGetMainHDNode(long contextID) {
        return NativeApiLL.nativeSOLGetMainHDNode(contextID);
    }

    public static String nativeSOLTransaction(long contextID, String json) {
        return NativeApiLL.nativeSOLTransaction(contextID, json);
    }

    public static String nativeSOLTokenTransaction(long contextID, String json) {
        return NativeApiLL.nativeSOLTokenTransaction(contextID, json);
    }

    public static String nativeSOLTokenCreateTransaction(long contextID, String json) {
        return NativeApiLL.nativeSOLTokenCreateTransaction(contextID, json);
    }

    //*************************************************************************************

    public static int nativeIdentityVerifyPIN(long deviceHandle, int mode, String pin, long[] retryTimes) {
        return NativeApiLL.nativeIdentityVerifyPIN(deviceHandle, mode, pin, retryTimes);
    }

    public static int nativeIdentityShowNineGrids(long deviceHandle) {
        return NativeApiLL.nativeIdentityShowNineGrids(deviceHandle);
    }

    public static int nativeIdentityCancelNineGrids(long deviceHandle) {
        return NativeApiLL.nativeIdentityCancelNineGrids(deviceHandle);
    }

    public static String nativeEnrollFingerprint(long deviceHandle, long timeOut, byte[] fgptIndex) {
        return NativeApiLL.nativeEnrollFingerprint(deviceHandle, timeOut, fgptIndex);
    }

    public static String nativeEnumFingerprint(long deviceHandle) {
        return NativeApiLL.nativeEnumFingerprint(deviceHandle);
    }

    public static int nativeEraseFingerprint(long deviceHandle, long timeOut) {
        return NativeApiLL.nativeEraseFingerprint(deviceHandle, timeOut);
    }

    public static int nativeDeleteFingerprint(long deviceHandle, long timeOut, byte fgptID) {
        return NativeApiLL.nativeDeleteFingerprint(deviceHandle, timeOut, fgptID);
    }

    public static int nativeVerifyFingerprint(long contextID, long[] retryTimes) {
        return NativeApiLL.nativeVerifyFingerprint(contextID, retryTimes);
    }

    public static int nativeRebootToBootloader(long deviceHandle) {
        return NativeApiLL.nativeRebootToBootloader(deviceHandle);
    }

    public static int nativeRebootToNormal(long deviceHandle) {
        return NativeApiLL.nativeRebootToNormal(deviceHandle);
    }

    public static int nativeUpdateFirmware(long deviceHandle, byte[] firmware, boolean rebootOnSuccess) {
        return NativeApiLL.nativeUpdateFirmware(deviceHandle, firmware, rebootOnSuccess);
    }

    public static int nativeUpdateResource(long deviceHandle, byte[] resource, boolean rebootOnSuccess) {
        return NativeApiLL.nativeUpdateResource(deviceHandle, resource, rebootOnSuccess);
    }

    public static int nativeUsePassphrase(long deviceHandle, boolean usePassphrase) {
        return NativeApiLL.nativeUsePassphrase(deviceHandle, usePassphrase);
    }
}
