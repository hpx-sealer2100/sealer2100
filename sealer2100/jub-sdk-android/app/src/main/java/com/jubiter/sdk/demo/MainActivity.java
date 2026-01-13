package com.jubiter.sdk.demo;

import android.Manifest;
import android.app.ProgressDialog;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.jubiter.sdk.demo.ble.FTBTDevice;
import com.jubiter.sdk.demo.ble.FtBTConnectCallback;
import com.jubiter.sdk.demo.impl.JubCallback;
import com.jubiter.sdk.demo.impl.JubiterImpl;
import com.jubiter.sdk.demo.utils.TipDialog;
import com.legendwd.hyperpay.onchain.hardware.jubnative.InnerDiscCallback;
import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.JUB_DEVICE_INFO;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import pub.devrel.easypermissions.AppSettingsDialog;
import pub.devrel.easypermissions.EasyPermissions;
import pub.devrel.easypermissions.PermissionRequest;

public class MainActivity extends AppCompatActivity implements EasyPermissions.PermissionCallbacks {

    private final static String TAG = "MainActivity";

    private Button mBtnScan;
    private TextView mTxtState;
    private boolean isConnect = false;
    private JubiterImpl mJubiter;
    private ProgressDialog mDialog;
    private EditText mEditApdu;
    private final static int REQUEST_PERMISSION = 0x1001;

    private List<String> mPermissionList = new ArrayList<>();

    private Uri firmwareUri;
    private Uri resourceUri;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mJubiter = JubiterImpl.getInstance(this);

        mBtnScan = findViewById(R.id.btn_scan);
        mTxtState = findViewById(R.id.text_state);
        mDialog = new ProgressDialog(this);
        mEditApdu = findViewById(R.id.edit_apdu);

        if (!hasPermissions()) {
            EasyPermissions.requestPermissions(
                            new PermissionRequest.Builder(
                                    this,
                                    REQUEST_PERMISSION,
                                    mPermissionList.toArray(new String[0]))
                                    .setRationale("需要蓝牙权限来扫描和连接设备")
                    .setPositiveButtonText("确定")
                    .setNegativeButtonText("取消")
                    .build()
            );
        }
    }

    public boolean hasPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            // Android 版本大于等于 Android12 时
            // 只包括蓝牙这部分的权限，其余的需要什么权限自己添加
            mPermissionList.add(Manifest.permission.BLUETOOTH_SCAN);
            mPermissionList.add(Manifest.permission.BLUETOOTH_CONNECT);
        } else {
            // Android 版本小于 Android12 及以下版本
            mPermissionList.add(Manifest.permission.ACCESS_COARSE_LOCATION);
            mPermissionList.add(Manifest.permission.ACCESS_FINE_LOCATION);
        }

        return EasyPermissions.hasPermissions(this, mPermissionList.toArray(new String[0]));
    }

    private InnerDiscCallback callback = new InnerDiscCallback() {
        @Override
        public void onDisconnect(String name) {

        }
    };

    public void onClick(View view) {
        if (view.getId() == R.id.btn_scan) {
            switchBt();
            return;
        }
        if (!isConnect) {
            return;
        }
        switch (view.getId()) {
            case R.id.btn_fingerprint_manager:
                showProgress();
                mJubiter.getDeviceType(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
//                showProgress();
//                mJubiter.getDeviceType(new JubCallback<String, Void>() {
//                    @Override
//                    public void onSuccess(String s, Void aVoid) {
//                        dismissProgress();
//                        DeviceType deviceType = new Gson().fromJson(s, DeviceType.class);
//                        if (deviceType.getDEVICE() == 2) {
//                            startActivity(new Intent(MainActivity.this, FingerPrintManagerActivity.class));
//                            return;
//                        }
//                        showMsg("Device not support");
//                    }
//
//                    @Override
//                    public void onFailed(int errorCode) {
//
//                    }
//                });
                break;
            case R.id.btn_get_device_info:
                showProgress();
                mJubiter.getDeviceInfo(new JubCallback<JUB_DEVICE_INFO, Void>() {
                    @Override
                    public void onSuccess(JUB_DEVICE_INFO info, Void aVoid) {
                        dismissProgress();
                        Log.d("info", info.toString());
                        showMsg(info.toString());
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_get_device_Features:
                showProgress();
                mJubiter.getDeviceFeatures(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_use_getcertandsigndata:
                showProgress();
                mJubiter.getDeviceCert(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_btc_getAddress:
                showProgress();
                mJubiter.btcGetAddress(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_qrc20_trans:
                showProgress();
                mJubiter.QRC20Trans(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_btc_trans:
                showProgress();
                mJubiter.btcTrans(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_btc_multisig_trans:
                mJubiter.btcMultiSigTrans(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_send_apdu:
                String apduStr = mEditApdu.getText().toString().trim().replace(" ", "");
                if (TextUtils.isEmpty(apduStr)) {
                    return;
                }
                showProgress();
                mJubiter.sendApdu(apduStr, new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        showMsg("sendAPDU failed");
                    }
                });
                break;
            case R.id.btn_applet:
                showProgress();
                mJubiter.enumApplets(new JubCallback<List<String>, Void>() {
                    @Override
                    public void onSuccess(List<String> strings, Void aVoid) {
                        dismissProgress();
                        getAppletVersion(strings);
                    }

                    @Override
                    public void onFailed(int errorCode) {

                    }
                });
                break;
            case R.id.btn_eth_trans:
                showProgress();
                mJubiter.ethTrans(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s + "\n");
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_btc_show_address:
                showProgress();
                mJubiter.btcShowAddress(0, 1, new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_btc_set_my_address:
                showProgress();
                mJubiter.btcSetMyAddress(0, 1, new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_btc_usdt_trans:
                showProgress();
                mJubiter.usdtTrans(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_set_time_out:
                showProgress();
                mJubiter.setTimeOut(500, new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_set_Lable:
                showProgress();
                mJubiter.setLable("new lable", new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;


            case R.id.btn_eth_showAddress:
                showProgress();
                mJubiter.ethShowAddress(1, new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_eth_getAddress:
//                showProgress();
                mJubiter.ethGetAddress(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s);
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_btc_getMainHDNode:
                showProgress();
                mJubiter.btcGetMainHDNode(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s);
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_eth_getMainHDNode:
                showProgress();
                mJubiter.ethGetMainHDNode(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_eth_transaction_UniswapV2Router02:
                showProgress();
                mJubiter.ethTransactionUniswapV2Router02(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_eth_sign_Bytestring:
                showProgress();
                mJubiter.ethSignBytestring(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_eth_sign_TypedData:
                showProgress();
                mJubiter.ethSignTypedData(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_trx_get_address:
                showProgress();
                mJubiter.trxGetAddress(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_trx_tx:
                showProgress();
                mJubiter.trxTransaction(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_enum_support_coins:
                showProgress();
                mJubiter.enumSupportCoins(new JubCallback<String[], String>() {
                    @Override
                    public void onSuccess(String[] s, String s2) {
                        showMsg(Arrays.toString(s));
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_hc_getAddress:
                showProgress();
                mJubiter.hcGetAddress(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_hc_getMainHDNode:
                showProgress();
                mJubiter.hcGetMainHDNode(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_hc_showAddress:
                showProgress();
                mJubiter.hcShowAddress(0, 1, new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_hc_transaction:
                showProgress();
                mJubiter.hcTransaction(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_eos_transaction:
                showProgress();
                mJubiter.eosTransaction(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_xrp_get_address:
                showProgress();
                mJubiter.xrpGetAddress(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_xrp_transaction:
                showProgress();
                mJubiter.xrpTransaction(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_eth_build:
                showProgress();
                mJubiter.ethBuild(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_eth_contract:
                showProgress();
                mJubiter.ethContract(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_eth_erc20:
                showProgress();
                mJubiter.ethErc20(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_eth_TypedData:
                showProgress();
                mJubiter.ethTypedData(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_fil_get_address:
                showProgress();
                mJubiter.filGetAddress(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;
            case R.id.btn_fil_tx:
                showProgress();
                mJubiter.filTrans(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_sol_get_address:
                showProgress();
                mJubiter.solGetAddress(new JubCallback<String, String>() {
                    @Override
                    public void onSuccess(String s, String s2) {
                        showMsg(s + "\t");
                        dismissProgress();
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_sol_tx:
                showProgress();
                mJubiter.solTrans(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_sol_token_tx:
                showProgress();
                mJubiter.solTokenTrans(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_sol_token_create_tx:
                showProgress();
                mJubiter.solTokenCreateTrans(new JubCallback<String, Void>() {
                    @Override
                    public void onSuccess(String s, Void aVoid) {
                        dismissProgress();
                        showMsg(s);
                    }

                    @Override
                    public void onFailed(int errorCode) {
                        dismissProgress();
                        Log.e("ret:", errorCode + "");
                        showMsg("errorCode:" + errorCode);
                    }
                });
                break;

            case R.id.btn_passphrase:
                passphrase();
                break;

            case R.id.btn_select_firmware:
                selectFirmwareFile();
                break;

            case R.id.btn_select_resource:
                selectResourceFile();
                break;

            case R.id.btn_update_firmware:
                updateFirmware();
                break;

            case R.id.btn_update_source:
                updateSource();
                break;
            case R.id.btn_use_passphrase:
                usePassphrase();
                break;
        }
    }

    private void selectFirmwareFile() {
        // 创建一个选择文件的意图
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        // 设置只显示 ZIP 文件
        intent.setType("application/octet-stream");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        try {
            // 启动文件选择器并等待结果
            startActivityForResult(Intent.createChooser(intent, "Select a ZIP file"), 1);
        } catch (ActivityNotFoundException e) {
            // 处理没有文件选择器应用的异常
            e.printStackTrace();
        }
    }

    private void selectResourceFile() {
        // 创建一个选择文件的意图
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        // 设置只显示 ZIP 文件
        intent.setType("application/zip");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        try {
            // 启动文件选择器并等待结果
            startActivityForResult(Intent.createChooser(intent, "Select a ZIP file"), 2);
        } catch (ActivityNotFoundException e) {
            // 处理没有文件选择器应用的异常
            e.printStackTrace();
        }
    }

    private void updateFirmware() {
        if (firmwareUri == null) {
            showMsg("请先选择固件文件");
            return;
        }

        mJubiter.updateFirmware(this, firmwareUri);
    }

    private void passphrase() {
        mJubiter.passphrase();
    }

    private void updateSource() {
        if (resourceUri == null) {
            showMsg("请先选择资源文件");
            return;
        }
        mJubiter.updateResource(this, resourceUri);
    }

    private void usePassphrase() {
        mJubiter.usePassphrase();
    }


    private void getAppletVersion(final List<String> strings) {
        mJubiter.getAppletVersion(strings, new JubCallback<String, Void>() {
            @Override
            public void onSuccess(String s, Void aVoid) {
                showMsg(s);
            }

            @Override
            public void onFailed(int errorCode) {

            }
        });
    }

    private void switchBt() {
        if (isConnect) {
            mJubiter.disConnectDevice();
            mBtnScan.setText("连接");
            isConnect = false;
        } else {
            mJubiter.connect(new FtBTConnectCallback() {
                @Override
                public void connected(FTBTDevice device, int code) {
                    if (code == 0) {
                        mTxtState.setText(device.getName() + "\n" + device.getMac());
                        mBtnScan.setText("断开");
                        isConnect = true;
                    } else {
                        mTxtState.setText(String.format("连接失败：0x%x", code) + "\n");
                    }
                }

                @Override
                public void disConnected() {
                    mTxtState.setText("");
                    mBtnScan.setText("连接");
                    isConnect = false;
                }
            });
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mJubiter.onDestroy();
    }

    @Override
    public void onPermissionsGranted(int requestCode, @NonNull List<String> perms) {
        String result = String.join(", ", perms);
        Log.d(TAG, ">>> onPermissionsGranted：" + result);
        if (requestCode == REQUEST_PERMISSION) {
            if (perms.containsAll(mPermissionList)) {
                // 所有蓝牙权限都已授予，开始扫描
                switchBt();
            }
        }
    }

    @Override
    public void onPermissionsDenied(int requestCode, @NonNull List<String> perms) {
        if (EasyPermissions.somePermissionPermanentlyDenied(this, perms)) {
            new AppSettingsDialog.Builder(this).build().show();
        }
        finish();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        String result = String.join(", ", permissions);
        Log.d(TAG, ">>> onRequestPermissionsResult: " + result);
        EasyPermissions.onRequestPermissionsResult(requestCode, permissions, grantResults, this);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        Log.d(TAG, "onActivityResult: " + resultCode);

        if (requestCode == 1 && resultCode == RESULT_OK) {
            if (data != null && data.getData() != null) {
                // 获取用户选择的 ZIP 文件的 Uri
                firmwareUri = data.getData();
                // 在这里可以对选择的文件进行处理
                Log.d(TAG, "BINs URI: " + firmwareUri.toString());
                ((TextView)findViewById(R.id.text_firmware_info)).setText(firmwareUri.toString());
            }
        }

        if (requestCode == 2 && resultCode == RESULT_OK) {
            if (data != null && data.getData() != null) {
                // 获取用户选择的 ZIP 文件的 Uri
                resourceUri = data.getData();
                // 在这里可以对选择的文件进行处理
                Log.d(TAG, "ZIP URI: " + resourceUri.toString());
                ((TextView)findViewById(R.id.text_resource_info)).setText(resourceUri.toString());
            }
        }

    }

    public void showMsg(String msg) {
        new TipDialog().init(this)
                .setTitle("Result")
                .setMsg(msg)
                .setPositiveButton("OK", null)
                .show();
    }

    public void showProgress() {
        if (mDialog != null) {
            mDialog.setMessage("通讯中....");
            mDialog.show();
        }
    }

    public void dismissProgress() {
        if (mDialog != null) {
            mDialog.dismiss();
        }
    }
}
