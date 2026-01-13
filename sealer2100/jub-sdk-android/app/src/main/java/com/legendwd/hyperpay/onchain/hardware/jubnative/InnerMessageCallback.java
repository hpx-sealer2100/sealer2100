package com.legendwd.hyperpay.onchain.hardware.jubnative;

/**
 *
 */
public interface InnerMessageCallback {

    /**
     * 接收 messageType 通知
     * @param messageType
     */
    void onNotifyMessage(int messageType);
}
