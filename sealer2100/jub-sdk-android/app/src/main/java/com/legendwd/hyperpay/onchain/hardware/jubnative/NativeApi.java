package com.legendwd.hyperpay.onchain.hardware.jubnative;

import android.util.Log;

import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.JUB_DEVICE_INFO;

/**
 * @author fengshuo
 * @date 2018/3/12
 * @time 13:26
 */

public class NativeApi {
    private static final String TAG = "NativeApi";


    enum DeviceType {
        HyperMateG2,
        HyperMateMax,
    }

    static DeviceType deviceType = DeviceType.HyperMateMax;

    /**
     * 当native接口的返回值不是int时，需要调用该接口获取错误码
     *
     * @return
     */
    public static int nativeGetErrorCode() {
        return NativeApiG2.nativeGetErrorCode();
    }

    //*********************************** 蓝牙接口 *****************************************

    public static int nativeInitDevice() {
        HyperMateAdapter.getInstance().init();
        return NativeApiG2.nativeInitDevice();
    }

    public static int nativeStartScan(InnerScanCallback scanCallback) {
        HyperMateAdapter.getInstance().startScan(scanCallback);
        return NativeApiG2.nativeStartScan(scanCallback);
    }

    public static int nativeStopScan() {
        HyperMateAdapter.getInstance().stopScan();
        return NativeApiG2.nativeStopScan();
    }

    public static int nativeConnectDevice(
            String devName,
            String address,
            int devType,
            long[] handle,
            int timeout,
            InnerDiscCallback discCallback) {
        Log.d(TAG, "nativeConnectDevice - deviceName: " + devName);

        HyperMateAdapter.getInstance().stopScan();
        NativeApiG2.nativeStopScan();

        // todo: HypermateMax 后续更改为正式产品名
        if (devName.startsWith(HyperMateAdapter.SCAN_FILTER_MAX) ||
                devName.startsWith(HyperMateAdapter.SCAN_FILTER_SEALER)) {
            deviceType = DeviceType.HyperMateMax;
            return HyperMateAdapter.getInstance().connectDevice(address, handle);
        }
        deviceType = DeviceType.HyperMateG2;
        return NativeApiG2.nativeConnectDevice(devName, address, devType, handle, timeout, discCallback);
    }

    public static int nativeDisconnect(long deviceHandle) {
        if (deviceType == DeviceType.HyperMateMax) {
            HyperMateAdapter.getInstance().disconnect(deviceHandle);
            return 0;
        } else {
            return NativeApiG2.nativeDisconnect(deviceHandle);
        }
    }

    public static int nativeIsConnected(long deviceHandle) {
        if (deviceType == DeviceType.HyperMateMax) {
            return HyperMateAdapter.getInstance().deviceConnected();
        }
        return NativeApiG2.nativeIsConnected(deviceHandle);
    }

    // ********************************* 功能接口 ********************************************

    public static String nativeGetDeviceType(long deviceHandle) {
        return NativeApiG2.nativeGetDeviceType(deviceHandle);
    }

    public static int nativeShowVirtualPwd(long contextID) {
        if (deviceType == DeviceType.HyperMateMax) {
            // 不支持
            return -1;
        }
        return NativeApiG2.nativeShowVirtualPwd(contextID);
    }

    public static int nativeCancelVirtualPwd(long contextID) {
        if (deviceType == DeviceType.HyperMateMax) {
            // 不支持
            return -1;
        }
        return NativeApiG2.nativeCancelVirtualPwd(contextID);
    }

    public static int nativeVerifyPIN(long contextID, byte[] pin) {
        if (deviceType == DeviceType.HyperMateMax) {
            // 不支持
            return -1;
        }
        return NativeApiG2.nativeVerifyPIN(contextID, pin);
    }

    public static int nativeGetDeviceInfo(JUB_DEVICE_INFO info, long deviceHandle) {
        return NativeApiG2.nativeGetDeviceInfo(info, deviceHandle);
    }

    public static String nativeSendApdu(long deviceHandle, String apduStr) {
        return NativeApiG2.nativeSendApdu(deviceHandle, apduStr);
    }
    public static int nativeSetLable(long deviceHandle, String Lable) {
        return NativeApiG2.nativeSetLable(deviceHandle, Lable);
    }
    public static int nativeEndSession(long deviceHandle) {
        return NativeApiG2.nativeEndSession(deviceHandle);
    }
    public static int nativeOpCancel(long deviceHandle) {
        return NativeApiG2.nativeOpCancel(deviceHandle);
    }

    public static String nativeGetDeviceCert(long deviceHandle) {
        return NativeApiG2.nativeGetDeviceCert(deviceHandle);
    }

    public static String nativeGetDeviceSignData(long deviceHandle, String hashData) {
        return NativeApiG2.nativeGetDeviceSignData(deviceHandle,hashData);
    }

    public static String nativeGetDeviceFeatures(long deviceHandle) {
        return NativeApiG2.nativeGetDeviceFeatures(deviceHandle);
    }

    public static String nativeEnumApplets(long deviceHandle) {
        return NativeApiG2.nativeEnumApplets(deviceHandle);
    }

    public static String nativeGetAppletVersion(long deviceHandle, String appID) {
        return NativeApiG2.nativeGetAppletVersion(deviceHandle, appID);
    }

    public static int nativeSetTimeOut(long contextID, int timeout) {
        return NativeApiG2.nativeSetTimeOut(contextID, timeout);
    }

    public static String nativeEnumSupportCoins(long deviceHandle) {
        return NativeApiG2.nativeEnumSupportCoins(deviceHandle);
    }

    public static int nativeClearContext(long contextID) {
        return NativeApiG2.nativeClearContext(contextID);
    }

    /**
     * 查询硬件电量
     *
     * @param deviceHandle 设备句柄
     * @param battery      出参：设备电量（十进制）
     * @return 接口是否调用成功
     */
    public static int nativeQueryBattery(long deviceHandle, int[] battery) {
        return NativeApiG2.nativeQueryBattery(deviceHandle, battery);
    }

    public static int nativeIsInitialize(long deviceHandle) {
        return NativeApiG2.nativeIsInitialize(deviceHandle);
    }

    public static int nativeIsBootLoader(long deviceHandle) {
        return NativeApiG2.nativeIsBootLoader(deviceHandle);
    }

    public static String nativeGetVersion() {
        return NativeApiG2.nativeGetVersion();
    }

    // ********************************* BTC ********************************************

    private static final boolean USE_LEGACY_ADDRESS = false;

    //// 只要不涉及多重签名，contextBTC只需配置 p2sh_segwit、main_path 两项

    /**
     * @param contextIDs
     * @param isMultiSig   是否是多重签名
     *                     false: json只需配置 p2sh_segwit、main_path
     *                     true: 全部配置，具体值根据多签的要求填充
     * @param json
     * @param deviceHandle
     * @return
     */
    public static int nativeBTCCreateContext(int[] contextIDs, boolean isMultiSig, String json, long deviceHandle) {
        return NativeApiG2.nativeBTCCreateContext(contextIDs, isMultiSig, json, deviceHandle);
    }

    /**
     * @param contextID
     * @param change           是否是找零地址；0：非找零地址，1：找零地址，2：其他
     * @param index
     * @param useLegacyAddress 地址显示为 Legacy 样式；
     * @return
     */
    public static String nativeBTCShowAddress(long contextID, int change, int index, boolean useLegacyAddress) {
        return NativeApiG2.nativeBTCShowAddress(contextID, change, index, useLegacyAddress);
    }

    public static String nativeBTCShowAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeBTCShowAddress(contextID, change, index, USE_LEGACY_ADDRESS);
    }

    /**
     * @param contextID
     * @param change    是否是找零地址；0：非找零地址，1：找零地址，2：其他
     * @param index
     * @return
     */
    public static String nativeBTCSetMyAddress(long contextID, int change, int index, boolean useLegacyAddress) {
        return NativeApiG2.nativeBTCSetMyAddress(contextID, change, index, useLegacyAddress);
    }

    public static String nativeBTCSetMyAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeBTCSetMyAddress(contextID, change, index, USE_LEGACY_ADDRESS);
    }

    public static String[] nativeBTCGetAddress(long contextID, boolean useLegacyAddress, String json) {
        return NativeApiG2.nativeBTCGetAddress(contextID, useLegacyAddress, json);
    }

    public static String[] nativeBTCGetAddress(long contextID, String json) {
        return NativeApiG2.nativeBTCGetAddress(contextID, USE_LEGACY_ADDRESS, json);
    }

    public static String nativeBTCGetMainHDNode(long contextID) {
        return NativeApiG2.nativeBTCGetMainHDNode(contextID);
    }

    /**
     * 普通签名：
     * multisig ：P2PKH(0)
     * <p>
     * 多重签名：
     * multisig ：P2SH_MULTISIG(2)
     *
     * @param contextID
     * @param json
     * @return
     */
    public static String nativeBTCTransaction(long contextID, boolean useLegacyAddress, String json) {
        return NativeApiG2.nativeBTCTransaction(contextID, useLegacyAddress, json);
    }

    public static String nativeBTCTransaction(long contextID, String json) {
        return NativeApiG2.nativeBTCTransaction(contextID, USE_LEGACY_ADDRESS, json);
    }

    /**
     * @param contextID
     * @param json
     * @return
     */
    public static String nativeQRC20Transaction(long contextID, String json) {
        return NativeApiG2.nativeQRC20Transaction(contextID, json);
    }

    /**
     * @param contextID
     * @param json
     * @return
     */
    public static String nativeUSDTTransaction(long contextID, String json) {
        return NativeApiG2.nativeUSDTTransaction(contextID, json);
    }

    /**
     * 将交易raw转换成json
     *
     * @param contextID
     * @param json      交易raw
     * @return
     */
    public static String nativeParseTransaction(long contextID, String json) {
        return NativeApiG2.nativeParseTransaction(contextID, json);
    }

    // ********************************* ETH ********************************************

    public static int nativeETHCreateContext(int[] contextIDs, String json, long deviceHandle) {
        return NativeApiG2.nativeETHCreateContext(contextIDs, json, deviceHandle);
    }

    public static String nativeETHShowAddress(long contextID, int index) {
        return NativeApiG2.nativeETHShowAddress(contextID, index);
    }

    public static String nativeETHSetMyAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeETHSetMyAddress(contextID, change, index);
    }

    public static int nativeETHSetContrAddr(long contextID, String contractAddress) {
        return NativeApiG2.nativeETHSetContrAddr(contextID, contractAddress);
    }

    public static String[] nativeETHGetAddress(long contextID, String json) {
        return NativeApiG2.nativeETHGetAddress(contextID, json);
    }

    public static String nativeETHGetMainHDNode(long contextID) {
        return NativeApiG2.nativeETHGetMainHDNode(contextID);
    }

    public static String nativeETHTransaction(long contextID, String json) {
        return NativeApiG2.nativeETHTransaction(contextID, json);
    }

    public static String nativeETHTypedData(long contextID, String json,String json_TypedData) {
        return NativeApiG2.nativeETHTypedData(contextID, json,json_TypedData);
    }

    public static int nativeETHSetERC20Tokens(long contextID, String json) {
        return NativeApiG2.nativeETHSetERC20Tokens(contextID, json);
    }

    public static int nativeETHSetERC20Token(long contextID, String json) {
        return NativeApiG2.nativeETHSetERC20Token(contextID, json);
    }

    public static String nativeETHBuildERC20TransferAbi(long contextID, String json) {
        return NativeApiG2.nativeETHBuildERC20TransferAbi(contextID, json);
    }

    public static String nativeETHSignBytestring(long contextID, String json) {
        return NativeApiG2.nativeETHSignBytestring(contextID, json);
    }

    public static String nativeETHSignTypedData(long contextID, String json) {
        return NativeApiG2.nativeETHSignTypedData(contextID, json);
    }

    //********************************* ETH multi sign ************************************

    public static String nativeETHSignContract(long contextID, String json) {
        return NativeApiG2.nativeETHSignContract(contextID, json);
    }

    //public static native String nativeETHBuildContractWithAddrAbi(long contextID, String json);

    public static String nativeETHBuildContractWithAddrAmtDataAbi(long contextID, String json) {
        return NativeApiG2.nativeETHBuildContractWithAddrAmtDataAbi(contextID, json);
    }

    public static String nativeETHBuildContractWithTxIDAbi(long contextID, String json) {
        return NativeApiG2.nativeETHBuildContractWithTxIDAbi(contextID, json);
    }

    //public static native String nativeETHBuildContractWithAmtAbi(long contextID, String json);

    //*********************************** HC **********************************************

    public static int nativeHCCreateContext(int[] contextIDs, boolean isMultiSig, String json, long deviceHandle) {
        return NativeApiG2.nativeHCCreateContext(contextIDs, isMultiSig, json, deviceHandle);
    }

    public static String[] nativeHCGetAddress(long contextID, String json) {
        return NativeApiG2.nativeHCGetAddress(contextID, json);
    }

    public static String nativeHCShowAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeHCShowAddress(contextID, change, index);
    }

    public static String nativeHCGetMainHDNode(long contextID) {
        return NativeApiG2.nativeHCGetMainHDNode(contextID);
    }

    public static String nativeHCTransaction(long contextID, String json) {
        return NativeApiG2.nativeHCTransaction(contextID, json);
    }

    //*********************************** EOS **********************************************
    public static int nativeEOSCreateContext(int[] contextIDs, String json, long deviceHandle) {
        return NativeApiG2.nativeEOSCreateContext(contextIDs, json, deviceHandle);
    }

    public static String nativeEOSShowAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeEOSShowAddress(contextID, change, index);
    }

    public static String nativeEOSGetAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeEOSGetAddress(contextID, change, index);
    }

    public static String nativeEOSSetMyAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeEOSSetMyAddress(contextID, change, index);
    }

    public static String nativeEOSTransaction(long contextID, String json) {
        return NativeApiG2.nativeEOSTransaction(contextID, json);
    }

    //*********************************** XRP **********************************************

    public static int nativeXRPCreateContext(int[] contextIDs, String json, long deviceHandle) {
        return NativeApiG2.nativeXRPCreateContext(contextIDs, json, deviceHandle);
    }

    public static String nativeXRPShowAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeXRPShowAddress(contextID, change, index);
    }

    public static String[] nativeXRPGetAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeXRPGetAddress(contextID, change, index);
    }

    public static String nativeXRPGetHDNode(long contextID, int change, int index, boolean useHex) {
        return NativeApiG2.nativeXRPGetHDNode(contextID, change, index, useHex);
    }

    public static String nativeXRPSetMyAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeXRPSetMyAddress(contextID, change, index);
    }

    public static String nativeXRPTransaction(long contextID, String json) {
        return NativeApiG2.nativeXRPTransaction(contextID, json);
    }

    //*********************************** TRX **********************************************

    public static int nativeTRXCreateContext(int[] contextIDs, String json, long deviceHandle) {
        return NativeApiG2.nativeTRXCreateContext(contextIDs, json, deviceHandle);
    }

    public static String nativeTRXShowAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeTRXShowAddress(contextID, change, index);
    }

    public static String[] nativeTRXGetAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeTRXGetAddress(contextID, change, index);
    }

    public static String nativeTRXGetHDNode(long contextID, int change, int index, boolean useHex) {
        return NativeApiG2.nativeTRXGetHDNode(contextID, change, index, useHex);
    }

    public static String nativeTRXSetMyAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeTRXSetMyAddress(contextID, change, index);
    }

    public static String nativeTRXTransaction(long contextID, String json, byte[] packedContractInPb) {
        return NativeApiG2.nativeTRXTransaction(contextID, json, packedContractInPb);
    }

    public static int nativeTRXSetTRC20Token(long contextID, String json) {
        return NativeApiG2.nativeTRXSetTRC20Token(contextID, json);
    }

    public static String nativeTRXBuildTRC20TransferAbi(long contextID, String json) {
        return NativeApiG2.nativeTRXBuildTRC20TransferAbi(contextID, json);
    }

    public static int nativeTRXSetTRC10Asset(long contextID, String json) {
        return NativeApiG2.nativeTRXSetTRC10Asset(contextID, json);
    }

    public static byte[] nativeTRXPackContract(long contextID, String json) {
        return NativeApiG2.nativeTRXPackContract(contextID, json);
    }

    //*********************************** FIL **********************************************

    public static int nativeFILCreateContext(int[] contextIDs, String json, long deviceHandle) {
        return NativeApiG2.nativeFILCreateContext(contextIDs, json, deviceHandle);
    }

    public static String nativeFILShowAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeFILShowAddress(contextID, change, index);
    }

    public static String[] nativeFILGetAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeFILGetAddress(contextID, change, index);
    }

    public static String nativeFILGetHDNode(long contextID, int change, int index, boolean useHex) {
        return NativeApiG2.nativeFILGetHDNode(contextID, change, index, useHex);
    }

    public static String nativeFILSetMyAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeFILSetMyAddress(contextID, change, index);
    }

    public static String nativeFILTransaction(long contextID, String json) {
        return NativeApiG2.nativeFILTransaction(contextID, json);
    }

    //*********************************** SOL **********************************************

    public static int nativeSOLCreateContext(int[] contextIDs, String json, long deviceHandle) {
        return NativeApiG2.nativeSOLCreateContext(contextIDs, json, deviceHandle);
    }

    public static String[] nativeSOLGetAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeSOLGetAddress(contextID, change, index);
    }

    public static String[] nativeSOLShowAddress(long contextID, int change, int index) {
        return NativeApiG2.nativeSOLShowAddress(contextID, change, index);
    }

    public static String nativeSOLGetHDNode(long contextID, int change, int index, boolean useHex) {
        return NativeApiG2.nativeSOLGetHDNode(contextID, change, index, useHex);
    }

    public static String nativeSOLGetMainHDNode(long contextID) {
        return NativeApiG2.nativeSOLGetMainHDNode(contextID);
    }

    public static String nativeSOLTransaction(long contextID, String json) {
        return NativeApiG2.nativeSOLTransaction(contextID, json);
    }

    public static String nativeSOLTokenTransaction(long contextID, String json) {
        return NativeApiG2.nativeSOLTokenTransaction(contextID, json);
    }

    public static String nativeSOLTokenCreateTransaction(long contextID, String json) {
        return NativeApiG2.nativeSOLTokenCreateTransaction(contextID, json);
    }

    //*********************************** bio **********************************************

    public static int nativeIdentityVerifyPIN(long deviceHandle, int mode, String pin, long[] retryTimes) {
        return NativeApiG2.nativeIdentityVerifyPIN(deviceHandle, mode, pin, retryTimes);
    }

    public static int nativeIdentityShowNineGrids(long deviceHandle) {
        return NativeApiG2.nativeIdentityShowNineGrids(deviceHandle);
    }

    public static int nativeIdentityCancelNineGrids(long deviceHandle) {
        return NativeApiG2.nativeIdentityCancelNineGrids(deviceHandle);
    }

    public static String nativeEnrollFingerprint(long deviceHandle, long timeOut, byte[] fgptIndex) {
        return NativeApiG2.nativeEnrollFingerprint(deviceHandle, timeOut, fgptIndex);
    }

    public static String nativeEnumFingerprint(long deviceHandle) {
        return NativeApiG2.nativeEnumFingerprint(deviceHandle);
    }

    public static int nativeEraseFingerprint(long deviceHandle, long timeOut) {
        return NativeApiG2.nativeEraseFingerprint(deviceHandle, timeOut);
    }

    public static int nativeDeleteFingerprint(long deviceHandle, long timeOut, byte fgptID) {
        return NativeApiG2.nativeDeleteFingerprint(deviceHandle, timeOut, fgptID);
    }

    public static int nativeVerifyFingerprint(long contextID, long[] retryTimes) {
        return NativeApiG2.nativeVerifyFingerprint(contextID, retryTimes);
    }

    public static void setNotifyMessageCallback(InnerMessageCallback notifyCallback) {
        NativeApiMax.setNotifyCallback(notifyCallback);
    }

    public static void setPassphraseCallback(PassphraseCallback passphraseCallback) {
        NativeApiMax.setPassphraseCallback(passphraseCallback);
    }

    public static void setUpdatePercentageCallback(UpdatePercentageCallback percentageCallback) {
        NativeApiMax.setUpdatePercentageCallback(percentageCallback);
    }

    public static int nativeRebootToBootloader(long deviceHandle) {
        return NativeApiG2.nativeRebootToBootloader(deviceHandle);
    }

    public static int nativeRebootToNormal(long deviceHandle) {
        return NativeApiG2.nativeRebootToNormal(deviceHandle);
    }

    public static int nativeUpdateFirmware(long deviceHandle, byte[] firmware, boolean rebootOnSuccess) {
        return NativeApiG2.nativeUpdateFirmware(deviceHandle, firmware, rebootOnSuccess);
    }

    public static int nativeUpdateResource(long deviceHandle, byte[] resource, boolean rebootOnSuccess) {
        return NativeApiG2.nativeUpdateResource(deviceHandle, resource, rebootOnSuccess);
    }

    public static int nativeUsePassphrase(long deviceHandle, boolean usePassphrase) {
        return NativeApiG2.nativeUsePassphrase(deviceHandle, usePassphrase);
    }
}
