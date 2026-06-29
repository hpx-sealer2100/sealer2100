package com.legendwd.hyperpay.onchain.hardware.jubnative;

import android.util.Log;

import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.InputPassphrase;
import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.LogUtil;
import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.StrUtil;

public class NativeApiMax {

    private static final String TAG = "NativeApiMax";

    private static InnerMessageCallback mMessageCallback;


    public interface NotifyCallback {
        void onNotify(byte[] data);
    }

    /**
     * 蓝牙数据转发 Native
     */
    private static final NotifyCallback mNotifyCallback = data -> {
        // 转发给 Native
        NativeApiMax.sendDataToNative(data);
    };


    private static PassphraseCallback mPassphraseCallback;

    private static UpdatePercentageCallback mUpdatePercentageCallback;


    /**
     * 接收来自 Native 的数据。
     * 该方法会被 C++ 层调用。
     *
     * @param data
     */
    public static void onNativeDataReceived(byte[] data) {
        // log for debug
        // LogUtil.i(TAG, () -> "[Java] recvNativeData" + "[" + data.length + "]:" + StrUtil.byteArr2HexStr(data));
        HyperMateAdapter.getInstance().writeWithoutResponse(data, mNotifyCallback);
    }

    /**
     * 接收来自 Native 的通知，用于处理 Button 事件。
     * 该方法会被 C++ 层调用。
     *
     * 比如：提示按键
     *
     * @param messageType
     */
    public static void onNotifyMessage(int messageType) {
        // log for debug
        // LogUtil.d(TAG, () -> "[Java] onNotifyMessage: " + messageType);
        if (mMessageCallback != null) {
            mMessageCallback.onNotifyMessage(messageType);
        }
    }

    /**
     * 设置 passphrase。
     * 该方法会被 C++ 层调用
     *
     * @param passphrase
     * @param onDevice
     */
    public static void onSetPassphrase(String passphrase, boolean onDevice) {
        if (mPassphraseCallback != null) {
            InputPassphrase inputPassphrase = new InputPassphrase(passphrase, onDevice);
            mPassphraseCallback.onPassphrase(inputPassphrase);
            nativeSendPassphraseToNative(inputPassphrase.getPassphrase(), inputPassphrase.getOnDevice());
        } else {
            // log for debug
            // Log.w(TAG, "Passphrase callback is not set.");
        }
    }

    /**
     * 更新进度百分比。
     * 该方法会被 C++ 层调用。
     *
     * @param percentage
     */
    public static void onUpdatePercentage(int percentage) {
        // log for debug
        // LogUtil.d(TAG, () -> "[Java] onUpdatePercentage: " + percentage);
        if (mUpdatePercentageCallback != null) {
            mUpdatePercentageCallback.onUpdatePercentage(percentage);
        }
    }


    //******************************************************************************************


    public static int nativeConnectDevice(String devName, String address, int devType, long[] handle,
                                          int timeout, InnerDiscCallback discCallback) {
        return NativeApiLL.nativeConnectDevice(devName, address, devType, handle, timeout, discCallback);
    }

    public static void sendDataToNative(byte[] data) {
        NativeApiLL.sendDataToNative(data);
    }


    public static void nativeSendPassphraseToNative(String passphrase, boolean onDevice) {
        NativeApiLL.nativeSendPassphraseToNative(passphrase, onDevice);
    }

    public static void setNotifyCallback(InnerMessageCallback callback) {
        if (callback != null) {
            mMessageCallback = callback;
        }
    }

    public static void setPassphraseCallback(PassphraseCallback callback) {
        if (callback != null) {
            mPassphraseCallback = callback;
        }
    }

    public static void setUpdatePercentageCallback(UpdatePercentageCallback callback) {
        if (callback != null) {
            mUpdatePercentageCallback = callback;
        }
    }
}