package com.legendwd.hyperpay.onchain.hardware.jubnative;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import com.legendwd.hyperpay.onchain.hardware.jubnative.protocol.ProtocolEncoder;
import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.Base64Converter;
import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.GetContextByReflection;
import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.LogUtil;
import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.StrUtil;
import com.polidea.multiplatformbleadapter.BleAdapter;
import com.polidea.multiplatformbleadapter.BleAdapterFactory;
import com.polidea.multiplatformbleadapter.ConnectionOptions;
import com.polidea.multiplatformbleadapter.ConnectionState;
import com.polidea.multiplatformbleadapter.RefreshGattMoment;
import com.polidea.multiplatformbleadapter.utils.Constants;

import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * @author ：FS
 * @date ：Created in 2025/5/15
 * @description ：支持端到端超时（含配对）的 HyperMate 蓝牙适配器（单设备单例）
 */
public class HyperMateAdapter {
    private static final String TAG = "HyperMateAdapter";

    private static final String ID_DISCOVER_SERVICE = "discoverService";
    private static final String ID_TX_NOTIFY = "notifyCharacteristic";
    private static final String ID_WRITE_CHAR = "writeCharacteristicForDevice";

    private static final String SERVICE_UUID = "00000001-0000-1000-8000-00805f9b34fb";
    private static final String WRITE_UUID = "00000002-0000-1000-8000-00805f9b34fb";
    private static final String NOTIFY_UUID = "00000003-0000-1000-8000-00805f9b34fb";

    private static final int BLE_MTU = 517;

    // HyperMateMax
    public static final String SCAN_FILTER_MAX = "HyperMateMax";
    public static final String SCAN_FILTER_SEALER = "Sealer";
    public static final String SCAN_FILTER_G2 = "HPYG2";

    // 为了统一流程的假数据
    private static final String CONNECT_DEV_NAME = "HyperMateMax";
    private static final String CONNECT_DEV_UUID = "00000000-0000-0000-0000-000000000000";
    private static final int CONNECT_DEV_TYPE = 0;

    private static final ScheduledExecutorService GLOBAL_SCHEDULER = Executors.newScheduledThreadPool(1);

    private long mDeviceHandle = 0;
    private String mDeviceAddress;

    private volatile boolean mConnected = false;
    private volatile boolean mConnecting = false;
    private volatile boolean mDeviceBonded = false;
    private volatile boolean mBondCompleted = false;

    private NativeApiMax.NotifyCallback mNotifyCallback;
    private InnerDiscCallback discCallback;
    private BroadcastReceiver mBondReceiver;
    private final AtomicBoolean mIsBondReceiverRegistered = new AtomicBoolean(false);

    private HyperMateAdapter() {
    }

    public static HyperMateAdapter getInstance() {
        return SingletonHolder.INSTANCE;
    }

    private static class SingletonHolder {
        private static final HyperMateAdapter INSTANCE = new HyperMateAdapter();
    }

    BleAdapter bleAdapter;
    BleSyncWriter bleSyncWriter;

    /**
     * 初始化蓝牙适配器。
     * 通过反射获取应用上下文，若上下文获取成功，则创建新的蓝牙适配器客户端。
     */
    public void init() {
        Context context = GetContextByReflection.getApplicationContext();
        if (context == null) return;

        bleAdapter = BleAdapterFactory.getNewAdapter(context);
        if (LogUtil.DEBUG) {
            bleAdapter.setLogLevel(Constants.BluetoothLogLevel.VERBOSE);
        }
        bleAdapter.createClient("HyperMateAdapter",
                data -> LogUtil.d(TAG, "createClient callback1: " + data),
                data -> LogUtil.d(TAG, "createClient callback2: " + data));
        bleSyncWriter = new BleSyncWriter(bleAdapter);
    }

    /**
     * 开始扫描蓝牙设备。
     *
     * @param scanCallback 扫描结果回调接口，用于处理扫描成功和失败的结果
     */
    public void startScan(InnerScanCallback scanCallback) {
        bleAdapter.startDeviceScan(
                null,
                0,
                1,
                data -> {
                    LogUtil.d(TAG, "scan callback: " + data);
                    if (data != null && data.getDeviceName() != null) {
                        if (data.getDeviceName().startsWith(SCAN_FILTER_MAX) ||
                                data.getDeviceName().startsWith(SCAN_FILTER_SEALER)) {
                            // MAX 这条产品代码中 deviceType 没有用到
                            scanCallback.onScanResult(data.getDeviceName(), data.getDeviceId(), 0);
                        }
                    }
                }, error -> {
                    Log.e(TAG, "scan onError: " + error.getMessage());
                    scanCallback.onError(-1);
                });
    }

    /**
     * 停止扫描蓝牙设备。
     */
    public void stopScan() {
        bleAdapter.stopDeviceScan();
    }

    /**
     * 连接指定地址的蓝牙设备
     *
     * @param deviceAddress 要连接的设备地址
     * @param handle        用于存储设备句柄的数组
     */
    public int connectDevice(String deviceAddress, final long[] handle, long timeoutMs, final InnerDiscCallback discCallback) {
        if (deviceAddress == null || handle == null || handle.length == 0 || discCallback == null) {
            Log.e(TAG, "connectDevice: invalid arguments");
            return -1;
        }
        if (timeoutMs <= 0) {
            Log.e(TAG, "connectDevice: timeoutMs must be > 0, got: " + timeoutMs);
            return -1;
        }

        synchronized (HyperMateAdapter.this) {
            if (mConnecting || mConnected) {
                Log.e(TAG, "Already connecting or connected. Reject concurrent call.");
                return -1;
            }
            mConnecting = true;
        }

        mDeviceAddress = deviceAddress;
        this.discCallback = discCallback;
        mConnected = false;
        mDeviceBonded = isDeviceAlreadyBonded(deviceAddress);
        mBondCompleted = mDeviceBonded;

        bleAdapter.stopDeviceScan();

        CountDownLatch totalLatch = new CountDownLatch(1);
        AtomicBoolean timedOut = new AtomicBoolean(false);
        AtomicBoolean completed = new AtomicBoolean(false);
        CountDownLatch bondLatch;

        if (!mDeviceBonded) {
            bondLatch = new CountDownLatch(1);
            registerBleBondReceiver(totalLatch, completed);
        } else {
            bondLatch = null;
        }

        // 超时任务
        Runnable timeoutTask = () -> {
            synchronized (HyperMateAdapter.this) {
                if (!completed.getAndSet(true)) {
                    timedOut.set(true);
                    Log.e(TAG, "connectDevice: overall timeout (" + timeoutMs + " ms)");
                    if (mConnected) {
                        bleAdapter.cancelDeviceConnection(mDeviceAddress, null, null);
                        mConnected = false;
                    }
                    totalLatch.countDown();
                    if (bondLatch != null) unregisterBleBondReceiver();
                }
            }
        };
        GLOBAL_SCHEDULER.schedule(timeoutTask, timeoutMs, TimeUnit.MILLISECONDS);

        ConnectionOptions options = new ConnectionOptions(
                false,
                BLE_MTU,
                RefreshGattMoment.ON_CONNECTED,
                null,
                Constants.ConnectionPriority.HIGH,
                true);

        try {
            bleAdapter.connectToDevice(
                    deviceAddress,
                    options,
                    data -> {
                        LogUtil.d(TAG, "connectDevice - onSuccessCallback");
                        bleSyncWriter.notifyConnectionState(true);
                        boolean shouldComplete = false;
                        synchronized (HyperMateAdapter.this) {
                            mConnected = true;
                            if (mDeviceBonded) {
                                shouldComplete = !completed.getAndSet(true);
                            }
                        }
                        if (shouldComplete) {
                            totalLatch.countDown();
                        }
                    },
                    data -> {
                        LogUtil.d(TAG, "connectDevice - onEventCallback: " + data);
                        if (ConnectionState.DISCONNECTED.value.equals(data.value)) {
                            // 停止写
                            bleSyncWriter.notifyConnectionState(false);

                            synchronized (HyperMateAdapter.this) {
                                mConnected = false;
                                // 在主线程回调断开通知
                                new Handler(Looper.getMainLooper()).post(() -> discCallback.onDisconnect(deviceAddress));

                                // 如果未完成，则标记完成并释放锁
                                if (!completed.getAndSet(true)) {
                                    totalLatch.countDown();
                                }
                            }
                        }
                    },
                    error -> {
                        Log.e(TAG, "connectDevice - onErrorCallback: " + error);
                        synchronized (HyperMateAdapter.this) {
                            if (!completed.getAndSet(true)) {
                                totalLatch.countDown();
                            }
                        }
                    });

            totalLatch.await();

        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            Log.e(TAG, "connectDevice interrupted", e);
            timedOut.set(true);
            if (!completed.getAndSet(true)) {
                totalLatch.countDown();
            }
        } finally {
            if (bondLatch != null) {
                unregisterBleBondReceiver();
            }
            synchronized (HyperMateAdapter.this) {
                mConnecting = false;
            }
        }

        if (timedOut.get() || !(mConnected && mDeviceBonded)) {
            Log.e(TAG, "connectDevice failed: timedOut=" + timedOut.get() +
                    ", connected=" + mConnected + ", bonded=" + mDeviceBonded);
            return -1;
        }

        nativeConnectDevice(handle);
        LogUtil.d(TAG, "connectDevice - handle 1: " + handle[0]);

        discoverServices(deviceAddress);
        return 0;
    }

    private boolean isDeviceAlreadyBonded(String address) {
        try {
            BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
            if (adapter == null) return false;
            BluetoothDevice device = adapter.getRemoteDevice(address);
            return device.getBondState() == BluetoothDevice.BOND_BONDED;
        } catch (SecurityException | IllegalArgumentException e) {
            Log.e(TAG, "检查绑定状态失败", e);
            return false;
        }
    }

    private void nativeConnectDevice(long[] handle) {
        // 为了统一流程的假数据
        int result = NativeApiMax.nativeConnectDevice(
                CONNECT_DEV_NAME,
                CONNECT_DEV_UUID,
                CONNECT_DEV_TYPE,
                handle,
                10_000,
                (InnerDiscCallback) data -> {}
        );
        if (0 == result) {
            mDeviceHandle = handle[0];
        }
        LogUtil.d(TAG, "nativeConnectDevice: " + result + ", handle: " + handle[0]);
    }

    public void disconnect(long deviceHandle) {
        if (mDeviceAddress != null) {
            bleAdapter.cancelDeviceConnection(
                    mDeviceAddress,
                    device -> {
                        LogUtil.d(TAG, "disconnect success");
                        synchronized (HyperMateAdapter.this) {
                            mConnected = false;
                            mDeviceHandle = 0;
                        }
                    },
                    bleError -> {
                        Log.e(TAG, "disconnect error");
                        synchronized (HyperMateAdapter.this) {
                            mConnected = false;
                            mDeviceHandle = 0;
                        }
                    }
            );
        }
        bleAdapter.cancelTransaction(ID_TX_NOTIFY);
        mDeviceAddress = null;
    }

    public int deviceConnected() {
        return mConnected ? 0 : -1;
    }

    /**
     * 发现指定设备的所有服务和特征。
     * 若发现成功，则开启通知以接收设备响应数据。
     *
     * @param deviceAddress 要发现服务和特征的设备地址
     */
    private void discoverServices(String deviceAddress) {
        CountDownLatch discoveryLatch = new CountDownLatch(1);

        bleAdapter.discoverAllServicesAndCharacteristicsForDevice(
                deviceAddress,
                ID_DISCOVER_SERVICE,
                data -> {
                    LogUtil.d(TAG, "discoverServices onSuccessCallback: " + data);
                    notifyCharacteristic(deviceAddress, SERVICE_UUID, NOTIFY_UUID);

                    // 在通知设置后，延迟1秒再 countDown
                    new Handler(Looper.getMainLooper()).postDelayed(discoveryLatch::countDown, 1000);
                },
                error -> {
                    Log.e(TAG, "discoverServices error: ", error);
                    discoveryLatch.countDown();
                });

        try {
            if (!discoveryLatch.await(5, TimeUnit.SECONDS)) {
                Log.e(TAG, "discoverServices: 服务发现超时");
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            Log.e(TAG, "discoverServices: 等待服务发现被中断", e);
        }

        LogUtil.d(TAG, "discoverServices finish");
    }

    /**
     * 开启 Notify，用于接收响应数据
     *
     * @param deviceAddress      设备地址
     * @param serviceUuid        服务 UUID
     * @param characteristicUuid 特征 UUID
     */
    private void notifyCharacteristic(String deviceAddress, String serviceUuid, String characteristicUuid) {
        bleAdapter.monitorCharacteristicForDevice(
                deviceAddress,
                serviceUuid,
                characteristicUuid,
                ID_TX_NOTIFY,
                data -> {
                    LogUtil.d(TAG, () -> "[cmd] notify received <<< " + StrUtil.byteArr2HexStr(data.getValue()));
                    // 转发
                    if (null != mNotifyCallback) {
                        mNotifyCallback.onNotify(data.getValue());
                    }
                },
                unused -> {
                    LogUtil.d(TAG, "notifyCharacteristic success");
                },
                error -> Log.e(TAG, "notifyCharacteristic error: " + error)
        );
    }

    /**
     * 写入命令并等待响应，确保会话已开启
     *
     * @param cmd 要写入的命令字节数组
     * @return 成功返回 true，否则返回 false
     */
    public boolean writeWithoutResponse(byte[] cmd, NativeApiMax.NotifyCallback callback) {
        mNotifyCallback = callback;
        LogUtil.d(TAG, () -> "[cmd] Encode: " + StrUtil.byteArr2HexStr(cmd));
        try {
            boolean writeSuccess = writeCommands(cmd);
            if (!writeSuccess) {
                Log.e(TAG, "writeAndWaitForResponse: 命令写入失败");
                return false;
            } else {
                LogUtil.d(TAG, () -> "writeAndWaitForResponse: 命令写入成功");
                return true;
            }
        } catch (Exception e) {
            Log.e(TAG, "writeAndWaitForResponse 过程中出现异常", e);
            return false;
        }
    }

    /**
     * 核心写入逻辑，将命令分片并写入设备
     *
     * @param cmd 要发送的命令字节数组
     * @return 若所有命令部分都成功发送则返回 true，否则返回 false
     */
    private boolean writeCommands(byte[] cmd) {
        List<byte[]> cmdList = ProtocolEncoder.slice(cmd);
        int transactionCounter = 0;  // 事务计数器，用于生成唯一ID
        for (byte[] part : cmdList) {
            LogUtil.d(TAG, () -> "[cmd] writeCharacteristic >>> " + StrUtil.byteArr2HexStr(part));
            if (!mConnected) {
                Log.e(TAG, "writeCommands: 设备已断开，停止写入");
                return false;
            }
            String cmdBase64 = Base64Converter.encode(part);
            // 生成唯一事务ID（基础ID + 计数器）
            String transactionId = "writeCharacteristic_" + transactionCounter++;
            if (!bleSyncWriter.writeSync(mDeviceAddress, SERVICE_UUID, WRITE_UUID, cmdBase64, transactionId)) {
                Log.e(TAG, "writeCommands: 写入失败");
                return false;
            }
        }
        return true;
    }

    private void registerBleBondReceiver(CountDownLatch totalLatch, AtomicBoolean completed) {
        if (mIsBondReceiverRegistered.get()) {
            return;
        }

        mBondReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if (device == null || device.getAddress() == null) return;
                if (mDeviceAddress == null || !device.getAddress().equals(mDeviceAddress)) return;
                if (!BluetoothDevice.ACTION_BOND_STATE_CHANGED.equals(action)) return;

                int bondState = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, BluetoothDevice.ERROR);
                LogUtil.d(TAG, "蓝牙广播：设备 bond 状态改变 - " + device.getAddress() + ", 状态: " + bondState);
                boolean shouldComplete = false;
                synchronized (HyperMateAdapter.this) {
                    if (bondState == BluetoothDevice.BOND_BONDED) {
                        LogUtil.d(TAG, "设备配对成功");
                        mDeviceBonded = true;
                        mBondCompleted = true;
                        shouldComplete = !completed.getAndSet(true);
                    } else if (bondState == BluetoothDevice.BOND_NONE) {
                        Log.e(TAG, "设备配对失败或取消");
                        mDeviceBonded = false;
                        mBondCompleted = true;
                        shouldComplete = !completed.getAndSet(true);
                    }
                }
                if (shouldComplete) {
                    totalLatch.countDown();
                }
            }
        };

        Context ctx = GetContextByReflection.getApplicationContext();
        if (ctx == null) {
            if (!completed.getAndSet(true)) {
                totalLatch.countDown();
            }
            return;
        }

        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        try {
            ctx.registerReceiver(mBondReceiver, filter);
            mIsBondReceiverRegistered.set(true);
        } catch (Exception e) {
            Log.e(TAG, "注册配对广播失败", e);
            mBondReceiver = null;
            if (!completed.getAndSet(true)) {
                totalLatch.countDown();
            }
        }
    }

    private void unregisterBleBondReceiver() {
        if (!mIsBondReceiverRegistered.getAndSet(false)) {
            return;
        }

        try {
            Context context = GetContextByReflection.getApplicationContext();
            if (context != null && mBondReceiver != null) {
                context.unregisterReceiver(mBondReceiver);
            }
        } catch (IllegalArgumentException e) {
            Log.w(TAG, "unregisterBleBondReceiver: 广播未注册", e);
        } finally {
            mBondReceiver = null;
            LogUtil.d(TAG, "unregisterBleBondReceiver: 广播已注销");
        }
    }
}