package com.legendwd.hyperpay.onchain.hardware.jubnative.protocol;

import androidx.annotation.NonNull;

import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.StrUtil;

public class MessageResponse {

    private final int mMessageType;
    private final byte[] mData;

    public MessageResponse(int messageType, byte[] data) {
        this.mMessageType = messageType;
        this.mData = data;
    }

    public int getMessageType() {
        return mMessageType;
    }

    public byte[] getData() {
        return mData;
    }

    @NonNull
    @Override
    public String toString() {
        return "MessageResponse{" +
                "mMessageType=" + mMessageType + "(" + MessageType.fromCode(mMessageType).name() + ")" +
                ", mData=" + StrUtil.byteArr2HexStr(mData) +
                '}';
    }
}
