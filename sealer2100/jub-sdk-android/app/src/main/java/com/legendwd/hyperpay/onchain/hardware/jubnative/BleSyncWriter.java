package com.legendwd.hyperpay.onchain.hardware.jubnative;

import android.util.Log;

import androidx.annotation.NonNull;

import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.LogUtil;
import com.polidea.multiplatformbleadapter.BleAdapter;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * BLE 同步写封装器
 * 特性：
 * - 串行 GATT 写（避免并发冲突）
 * - 超时控制（默认 3s）
 */
public class BleSyncWriter {

    public static final String TAG = "BleSyncWriter";

    private final BleAdapter bleAdapter;
    private final ExecutorService writeExecutor = Executors.newSingleThreadExecutor();

    // 同步控制
    private final LinkedBlockingQueue<Boolean> writeQueue = new LinkedBlockingQueue<>();
    private final AtomicBoolean isConnected = new AtomicBoolean(true); // 默认连接

    private final int DEFAULT_WRITE_TIMEOUT_MS = 3_000;


    public BleSyncWriter(@NonNull BleAdapter bleAdapter) {
//        if (Looper.getMainLooper().getThread() == Thread.currentThread()) {
//            throw new IllegalStateException("Do not instantiate on main thread!");
//        }
        this.bleAdapter = bleAdapter;
    }

    /**
     * 同步写入一包数据（必须在非主线程调用）
     *
     * @return true 成功；false 超时、失败或已关闭
     * @throws IllegalStateException 若在主线程调用
     */
    public boolean writeSync(String deviceIdentifier,
                             String serviceUUID,
                             String characteristicUUID,
                             String valueBase64,
                             String transactionId) {
        // 快速失败检查
        if (!isConnected.get()) {
            LogUtil.d(TAG, "writeSync: 未连接1");
            return false;
        }

        // 提交写任务到线程池
        writeExecutor.execute(() -> {
            if (!isConnected.get()) {
                writeQueue.offer(false); // 唤醒等待者
                LogUtil.d(TAG, "writeSync: 未连接2");
                return;
            }

            bleAdapter.writeCharacteristicForDevice(
                    deviceIdentifier,
                    serviceUUID,
                    characteristicUUID,
                    valueBase64,
                    false,
                    transactionId,
                    data -> {
                        handleWriteResult(true);
                    },
                    error -> {
                        Log.e(TAG, "write error: " + error);
                        handleWriteResult(false);
                    });
        });

        try {
            Boolean result = writeQueue.poll(DEFAULT_WRITE_TIMEOUT_MS, TimeUnit.MILLISECONDS);
            return result != null && result;
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            Log.e(TAG, "writeSync: InterruptedException");
            return false;
        }
    }

    private void handleWriteResult(boolean success) {
        writeQueue.offer(success);
    }

    /**
     * 通知 writer 连接已断开
     */
    public void notifyConnectionState(boolean state) {
        isConnected.set(state);
        LogUtil.d(TAG, "notifyConnectionState: " + state);
    }
}