package com.jubiter.sdk.demo.ble;

import android.app.ProgressDialog;
import android.content.Context;
import android.os.Handler;
import android.util.Log;

import com.legendwd.hyperpay.onchain.hardware.jubnative.HyperMateAdapter;
import com.legendwd.hyperpay.onchain.hardware.jubnative.InnerDiscCallback;
import com.legendwd.hyperpay.onchain.hardware.jubnative.NativeApi;


/**
 * @Date 2018/4/19  11:40
 * @Author ZJF
 * @Version 1.0
 */
public class BluetoothKeyApi {

    public enum DeviceType {
        HyperMate,
        HyperMateMax,
    }


    private FtBTConnectCallback mConnectCallback;
    private ProgressDialog mDialog;
    private Handler mHandler;
    private Context mContext;
    public static DeviceType sDeviceType;

    private InnerDiscCallback callback = new InnerDiscCallback() {
        @Override
        public void onDisconnect(String name) {
            if (mConnectCallback != null) {
                mConnectCallback.disConnected();
            }
        }
    };

    private SelectDeviceDialog.OnSelectedListener selectedListener = new SelectDeviceDialog.OnSelectedListener() {
        @Override
        public void onSelected(final FTBTDevice device) {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    dismissMsgDialog();
                    if (device == null) {
                        return;
                    }
                    long[] hld = new long[1];
                    if (device.getName().startsWith(HyperMateAdapter.SCAN_FILTER_MAX) ||
                            device.getName().startsWith(HyperMateAdapter.SCAN_FILTER_SEALER)) {
                        sDeviceType = DeviceType.HyperMateMax;
                    } else {
                        sDeviceType = DeviceType.HyperMate;
                    }

                    final int ret = NativeApi.nativeConnectDevice(device.getName(), device.getMac(), 1, hld, 30 * 1000, callback);
                    Log.d("FS", "connectDevice - handle 3: " + hld[0]);
                    if (ret != 0) {
                        mHandler.post(new Runnable() {
                            @Override
                            public void run() {
                                mConnectCallback.connected(null, ret);
                            }
                        });
                        Log.e("selectedListener", String.format("错误码：0x%x", ret));
                    } else {
                        Log.e("selectedListener", "connected handler: " + hld[0]);
                        device.setHandler(hld[0]);
                        mHandler.post(new Runnable() {
                            @Override
                            public void run() {
                                mConnectCallback.connected(device, 0);
                            }
                        });
                    }
                }
            }).start();
        }
    };


    public BluetoothKeyApi(Context context) {
        mContext = context;
        mHandler = new Handler(context.getMainLooper());
        int ret = NativeApi.nativeInitDevice();
        Log.d("ZJF_nativeInitDevice", ret + "");
    }

    public void connect(FtBTConnectCallback connectCallback) {
        showMsgDialog("正在连接...");
        mConnectCallback = connectCallback;

        SelectDeviceDialog dialog = new SelectDeviceDialog(mContext, selectedListener);
        dialog.show();
    }

    public int disConnect(long handler) {
        return NativeApi.nativeDisconnect(handler);
    }

    private void showMsgDialog(String msg) {
        if (mDialog == null) {
            mDialog = new ProgressDialog(mContext);
        }
        mDialog.setMessage(msg);
        if (!mDialog.isShowing()) {
            mDialog.show();
        }
    }

    private void dismissMsgDialog() {
        if (mDialog != null && mDialog.isShowing()) {
            mDialog.dismiss();
        }
    }
}
