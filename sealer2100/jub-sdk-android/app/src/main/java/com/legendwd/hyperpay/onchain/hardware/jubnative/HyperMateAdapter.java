package com.legendwd.hyperpay.onchain.hardware.jubnative;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

import com.legendwd.hyperpay.onchain.hardware.jubnative.protocol.MessageResponse;
import com.legendwd.hyperpay.onchain.hardware.jubnative.protocol.ProtocolDecoder;
import com.legendwd.hyperpay.onchain.hardware.jubnative.protocol.ProtocolEncoder;
import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.Base64Converter;
import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.GetContextByReflection;
import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.StrUtil;
import com.polidea.multiplatformbleadapter.BleAdapter;
import com.polidea.multiplatformbleadapter.BleAdapterFactory;
import com.polidea.multiplatformbleadapter.ConnectionOptions;
import com.polidea.multiplatformbleadapter.utils.Constants;

import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * @author ：FS
 * @date ：Created in 2025/5/15
 * @description ：
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

    // Digitshield
    // HyperMateMax
    public static final String SCAN_FILTER_MAX = "HyperMateMax";
    public static final String SCAN_FILTER_SEALER = "Sealer";
    public static final String SCAN_FILTER_G2 = "HPYG2";

    // 为了统一流程的假数据
    private static final String CONNECT_DEV_NAME = "HyperMateMax";
    private static final String CONNECT_DEV_UUID = "00000000-0000-0000-0000-000000000000";
    private static final int CONNECT_DEV_TYPE = 0;

    // open session 指令
    private static final String OPEN_SESSION_CMD_HEX = "3F232300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    private static final int RESPONSE_TIMEOUT_MS = 10_000;

    private long mDeviceHandle = 0;
    private String mDeviceAddress;

    private final Object mLock = new Object();
    private final Object mBondLock = new Object();
    private final Object mWriteLock = new Object();
    private final Object mNotifyLock = new Object();

    private volatile boolean mSessionOpened = false;
    private volatile boolean mConnected = false;
    private volatile boolean mDeviceBonded = false;
    private volatile boolean mBondCompleted = false;

    private NativeApiMax.NotifyCallback mNotifyCallback;
    private BroadcastReceiver mReceiver;
    private BroadcastReceiver mBondReceiver;

    private Timer discoveryTimer;

    private HyperMateAdapter() {
    }

    public static HyperMateAdapter getInstance() {
        return SingletonHolder.INSTANCE;
    }

    private static class SingletonHolder {
        private static final HyperMateAdapter INSTANCE = new HyperMateAdapter();
    }

    BleAdapter bleAdapter;

    /**
     * 初始化蓝牙适配器。
     * 通过反射获取应用上下文，若上下文获取成功，则创建新的蓝牙适配器客户端。
     */
    public void init() {
        Context context = GetContextByReflection.getApplicationContext();
        if (null == context) {
            return;
        }

        bleAdapter = BleAdapterFactory.getNewAdapter(context);
//        bleAdapter.setLogLevel(Constants.BluetoothLogLevel.VERBOSE);
        bleAdapter.createClient("HyperMateAdapter",
                data -> {
                    Log.d(TAG, "createClient callback1: " + data);
                },
                data -> {
                    Log.d(TAG, "createClient callback2: " + data);
                });
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
                    Log.d(TAG, "scan callback: " + data);
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
     * 连接指定地址的蓝牙设备。
     *
     * @param deviceAddress 要连接的设备地址
     * @param handle        用于存储设备句柄的数组
     */
    public int connectDevice(String deviceAddress, final long[] handle) {
        mDeviceAddress = deviceAddress;
        mDeviceBonded = isDeviceAlreadyBonded(deviceAddress);

        bleAdapter.stopDeviceScan();

        registerBleReceiver();
        if (!mDeviceBonded) {
            registerBleBondReceiver();
        }

        ConnectionOptions options = new ConnectionOptions(false, BLE_MTU, null, null, Constants.ConnectionPriority.HIGH);
        Log.d(TAG, "connectDevice - thread: " + Thread.currentThread().getName());
        bleAdapter.connectToDevice(
                deviceAddress,
                options,
                data -> {
                    Log.d(TAG, "connectDevice - onSuccessCallback - thread: " + Thread.currentThread().getName());
                    mConnected = true;
                    synchronized (mLock) {
                        mLock.notifyAll();
                    }

                }, data -> {
                    Log.d(TAG, "connectDevice - onEventCallback: " + data);
                }, error -> {
                    Log.e(TAG, "connectDevice - onErrorCallback: " + error);
                    mConnected = false;
                });
        synchronized (mLock) {
            try {
                mLock.wait(5_000);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                Log.e(TAG, "connectDevice: 等待连接被中断", e);
                return -1;
            }
        }

        nativeConnectDevice(handle);
        Log.d(TAG, "connectDevice - handle 1: " + handle[0]);

        synchronized (mBondLock) {
            long startTime = System.currentTimeMillis();
            while (!mDeviceBonded && !mBondCompleted) {
                long elapsed = System.currentTimeMillis() - startTime;
                long remaining = 30_000 - elapsed; // 总超时30秒
                if (remaining <= 0) {
                    Log.e(TAG, "蓝牙配对超时");
                    mBondCompleted = true;
                    break;
                }
                try {
                    mBondLock.wait(remaining);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    Log.e(TAG, "等待绑定被中断", e);
                    return -1;
                }
            }
        }

        // 配对成功后才进行服务发现和通知设置
        if (mDeviceBonded) {
            discoverServices(deviceAddress);
        } else {
            Log.e(TAG, "配对失败，无法发现服务");
            return -1;
        }

        return (mConnected && mDeviceBonded) ? 0 : -1;
    }


    /**
     * 调用本地方法连接设备，获取设备句柄。
     *
     * @param handle 用于存储设备句柄的数组
     */
    private void nativeConnectDevice(long[] handle) {
        // 为了统一流程的假数据
        int result = NativeApiMax.nativeConnectDevice(
                CONNECT_DEV_NAME,
                CONNECT_DEV_UUID,
                CONNECT_DEV_TYPE,
                handle,
                10_000,
                (InnerDiscCallback) data -> {

                }
        );
        if (0 == result) {
            mDeviceHandle = handle[0];
        }
        Log.d(TAG, "nativeConnectDevice: " + result + ", handle: " + handle[0]);
    }

    public void disconnect(long deviceHandle) {
        mConnected = false;
        bleAdapter.cancelDeviceConnection(
                mDeviceAddress,
                device -> {
                    Log.d(TAG, "disconnect success");
                },
                bleError -> {
                    Log.d(TAG, "disconnect error");
                });
        // 避免多次断开、连接崩溃
        bleAdapter.cancelTransaction(ID_TX_NOTIFY);
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
        if (discoveryTimer != null) {
            discoveryTimer.cancel();
        }

        bleAdapter.discoverAllServicesAndCharacteristicsForDevice(
                deviceAddress,
                ID_DISCOVER_SERVICE,
                data -> {
                    Log.d(TAG, "discoverServices onSuccessCallback: " + data);
                    notifyCharacteristic(deviceAddress, SERVICE_UUID, NOTIFY_UUID);

                    discoveryTimer = new Timer();
                    discoveryTimer.schedule(new TimerTask() {
                        @Override
                        public void run() {
                            synchronized (mLock) {
                                mLock.notifyAll();
                            }
                            discoveryTimer.cancel();
                        }
                    }, 1_000);
                },
                error -> {
                    Log.d(TAG, "discoverServices error: " + error);
                });
        synchronized (mLock) {
            try {
                mLock.wait(5_000);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                Log.e(TAG, "discoverServices: 等待服务发现被中断", e);
            }
        }
        Log.d(TAG, "discoverServices finish");
    }

    /**
     * 开启 Notify，用于接收响应数据
     *
     * @param deviceAddress
     * @param serviceUuid
     * @param characteristicUuid
     */
    private void notifyCharacteristic(String deviceAddress, String serviceUuid, String characteristicUuid) {
        bleAdapter.monitorCharacteristicForDevice(
                deviceAddress,
                serviceUuid,
                characteristicUuid,
                ID_TX_NOTIFY,
                data -> {
                    Log.d(TAG, "[cmd] notify received <<< " + StrUtil.byteArr2HexStr(data.getValue()));
                    // 转发
                    if (null != mNotifyCallback) {
                        mNotifyCallback.onNotify(data.getValue());
                    }
                },
                error -> {
                    Log.d(TAG, "notifyCharacteristic error: " + error);
                });
    }

    /**
     * 写入命令并等待响应，确保会话已开启
     *
     * @param cmd 要写入的命令字节数组
     * @return 成功返回 true，否则返回 false
     */
    public boolean writeWithoutResponse(byte[] cmd, NativeApiMax.NotifyCallback callback) {
        mNotifyCallback = callback;

        Log.d(TAG, "[cmd] Encode: " + StrUtil.byteArr2HexStr(cmd));

        try {
            boolean writeSuccess = writeCommands(cmd);
            if (!writeSuccess) {
                Log.e(TAG, "writeAndWaitForResponse: 命令写入失败");
                return false;
            } else {
                Log.d(TAG, "writeAndWaitForResponse: 命令写入成功");
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
        AtomicBoolean allCommandsSent = new AtomicBoolean(true);
        int transactionCounter = 0; // 事务计数器，用于生成唯一ID

        for (byte[] cmdPart : cmdList) {
            Log.d(TAG, "[cmd] writeCharacteristic >>> " + StrUtil.byteArr2HexStr(cmdPart));
            String cmdBase64 = Base64Converter.encode(cmdPart);

            // 当前分片写入状态
            AtomicBoolean writeCompleted = new AtomicBoolean(false);
            // 生成唯一事务ID（基础ID + 计数器）
            String transactionId = "writeCharacteristic_" + transactionCounter++;

            // 检查蓝牙状态，断开则停止写入
            if (!mConnected) {
                Log.e(TAG, "writeCommands: 设备已断开，停止写入");
                allCommandsSent.set(false);
                break;
            }

            bleAdapter.writeCharacteristicForDevice(
                    mDeviceAddress,
                    SERVICE_UUID.toString(),
                    WRITE_UUID.toString(),
                    cmdBase64,
                    false,
                    transactionId,
                    data -> {
//                        Log.d(TAG, "write onSuccessCallback: " + StrUtil.byteArr2HexStr(data.getValue()));
                        synchronized (mWriteLock) {
                            writeCompleted.set(true);
                            mWriteLock.notifyAll();
                        }
                    },
                    error -> {
                        Log.e(TAG, "write error: " + error);
                        allCommandsSent.set(false);
                        // 修复：写入失败时标记完成状态，避免死锁
                        writeCompleted.set(true);
                    });
            synchronized (mWriteLock) {
                try {
                    // 避免“通知提前发送”导致的死锁问题
                    while (!writeCompleted.get()) {
                        mWriteLock.wait(3_000);
                        if (!writeCompleted.get()) {
                            allCommandsSent.set(false);
                            writeCompleted.set(true);
                        }
                    }
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    Log.e(TAG, "writeCommands: 等待写入被中断", e);
                    allCommandsSent.set(false);
                }
            }
        }
        allCommandsSent.set(true);
        return allCommandsSent.get();
    }

    /**
     * 等待设备响应，设置超时时间
     *
     * @return 设备响应的字节数组，若超时则返回 null
     */
    private MessageResponse waitForResponse() {
        synchronized (mNotifyLock) {
            try {
                mNotifyLock.wait(RESPONSE_TIMEOUT_MS);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                Log.e(TAG, "waitForResponse: 等待响应被中断", e);
                return null;
            }
        }
        return ProtocolDecoder.decode();
    }

    /**
     * 监听设备连接状态
     */
    private void registerBleReceiver() {
        mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if (device == null || device.getAddress() == null) return;

                // 只处理当前连接的设备（通过地址匹配）
                if (!device.getAddress().equals(mDeviceAddress)) return;

                if (BluetoothDevice.ACTION_ACL_DISCONNECTED.equals(action)) {
                    Log.d(TAG, "蓝牙广播：设备已断开 - " + device.getAddress());
                    mConnected = false;
                    unregisterBleReceiver();
                }
            }
        };

        Context context = GetContextByReflection.getApplicationContext();
        if (context == null) {
            return;
        }

        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
        filter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        filter.setPriority(IntentFilter.SYSTEM_HIGH_PRIORITY);
        context.registerReceiver(mReceiver, filter);
    }

    private void unregisterBleReceiver() {
        Context context = GetContextByReflection.getApplicationContext();
        if (context == null || mReceiver == null) {
            return;
        }
        context.unregisterReceiver(mReceiver);
        Log.d(TAG, "unregisterBleReceiver: 广播已注销");
    }

    private void registerBleBondReceiver() {
        if (mBondReceiver != null) {
            unregisterBleBondReceiver();
        }
        mBondReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if (device == null || device.getAddress() == null) return;

                // 只处理当前连接的设备（通过地址匹配）
                if (mDeviceAddress == null || !device.getAddress().equals(mDeviceAddress)) return;

                if (!BluetoothDevice.ACTION_BOND_STATE_CHANGED.equals(action)) {
                    return;
                }
                int bondState = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, BluetoothDevice.ERROR);
//                Log.d(TAG, "蓝牙广播：设备 bonding 状态改变 - " + device.getAddress() + ", 状态: " + bondState);
                if (bondState == BluetoothDevice.BOND_BONDING) {
                    Log.d(TAG, "设备正在配对...");
                    mDeviceBonded = false;
                    mBondCompleted = false;
                } else if (bondState == BluetoothDevice.BOND_BONDED) {
                    Log.d(TAG, "设备已配对成功，尝试打开会话");

                    synchronized (mBondLock) {
                        mDeviceBonded = true;
                        mBondCompleted = true;
                        mBondLock.notifyAll();
                    }

                    mDeviceBonded = true;
                    unregisterBleBondReceiver();
                } else if (bondState == BluetoothDevice.BOND_NONE) {
                    Log.e(TAG, "设备配对失败或取消");
                    synchronized (mBondLock) {
                        mDeviceBonded = false;
                        mBondCompleted = true;  // 配对失败，同样标记流程结束
                        mBondLock.notifyAll();
                    }
                    unregisterBleBondReceiver();
                }
            }
        };

        Context context = GetContextByReflection.getApplicationContext();
        if (context == null) {
            return;
        }

        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        try {
            context.registerReceiver(mBondReceiver, filter);
        } catch (Exception e) {
            Log.e(TAG, "注册配对广播接收器失败", e);
            mBondReceiver = null;
        }
    }

    private void unregisterBleBondReceiver() {
        Context context = GetContextByReflection.getApplicationContext();
        if (context == null || mBondReceiver == null) {
            return;
        }
        context.unregisterReceiver(mBondReceiver);
        mBondReceiver = null;
        Log.d(TAG, "unregisterBleBondReceiver: 广播已注销");
    }

    private boolean isDeviceAlreadyBonded(String address) {
        try {
            BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            BluetoothDevice device = bluetoothAdapter.getRemoteDevice(address);
            return device.getBondState() == BluetoothDevice.BOND_BONDED;
        } catch (SecurityException e) {
            Log.e(TAG, "获取绑定状态失败：权限不足", e);
            return false;
        }
    }
}