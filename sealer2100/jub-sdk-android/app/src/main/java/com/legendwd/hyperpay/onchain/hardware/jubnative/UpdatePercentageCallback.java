package com.legendwd.hyperpay.onchain.hardware.jubnative;

/**
 * 转发进度更新
 */
public interface UpdatePercentageCallback {
    /**
     * 接收进度更新
     *
     * @param percentage 进度百分比，范围从 0 到 100
     */
    void onUpdatePercentage(int percentage);
}