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

import com.google.gson.Gson;
import com.jubiter.sdk.demo.ble.FTBTDevice;
import com.jubiter.sdk.demo.ble.FtBTConnectCallback;
import com.jubiter.sdk.demo.impl.DeviceType;
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

        // 初始化所有按钮点击事件
        initFingerprintManager();
        initGetDeviceInfo();
        initGetDeviceFeatures();
        initBtcGetAddress();
        initQrc20Trans();
        initBtcTrans();
        initBtcMultiSigTrans();
        initSendApdu();
        initApplet();
        initEthTrans();
        initEthTransCustom();
        initBtcShowAddress();
        initBtcSetMyAddress();
        initBtcUsdtTrans();
        initSetTimeOut();
        initSetLable();
        initEthShowAddress();
        initEthGetAddress();
        initBtcGetMainHDNode();
        initEthGetMainHDNode();
        initEthTransactionUniswapV2Router02();
        initTrxGetAddress();
        initTrxTx();
        initEnumSupportCoins();
        initHcGetAddress();
        initHcGetMainHDNode();
        initHcShowAddress();
        initHcTransaction();
        initEosTransaction();
        initXrpGetAddress();
        initXrpTransaction();
        initEthBuild();
        initEthContract();
        initEthErc20();
        initEthErc20Custom();
        initEthSignBytestring();
        initEthTypedData();
        initEthUploadNFT();
        initFilGetAddress();
        initFilTx();
        initSolGetAddress();
        initSolTx();
        initSolTokenTx();
        initSolTokenCreateTx();
        initPassphrase();
        initSelectFirmware();
        initSelectResource();
        initUpdateFirmware();
        initUpdateSource();

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
            mPermissionList.add(Manifest.permission.BLUETOOTH_SCAN);
            mPermissionList.add(Manifest.permission.BLUETOOTH_CONNECT);
        } else {
            mPermissionList.add(Manifest.permission.ACCESS_COARSE_LOCATION);
            mPermissionList.add(Manifest.permission.ACCESS_FINE_LOCATION);
        }
        return EasyPermissions.hasPermissions(this, mPermissionList.toArray(new String[0]));
    }

    private InnerDiscCallback callback = new InnerDiscCallback() {
        @Override
        public void onDisconnect(String name) {}
    };

    public void onClick(View view) {
        if (view.getId() == R.id.btn_scan) {
            switchBt();
        }
    }

    // 指纹管理按钮初始化
    private void initFingerprintManager() {
        findViewById(R.id.btn_fingerprint_manager, v -> {
            if (!isConnect) return;
            showProgress();
            mJubiter.getDeviceType(new JubCallback<String, Void>() {
                @Override
                public void onSuccess(String s, Void aVoid) {
                    dismissProgress();
                    DeviceType deviceType = new Gson().fromJson(s, DeviceType.class);
                    if (deviceType.getDEVICE() == 2) {
                        startActivity(new Intent(MainActivity.this, FingerPrintManagerActivity.class));
                        return;
                    }
                    showMsg("Device not support");
                }

                @Override
                public void onFailed(int errorCode) { dismissProgress(); }
            });
        });
    }

    // 获取设备信息按钮初始化
    private void initGetDeviceInfo() {
        findViewById(R.id.btn_get_device_info, v -> {
            if (!isConnect) return;
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
        });
    }

    // 获取设备特性按钮初始化
    private void initGetDeviceFeatures() {
        findViewById(R.id.btn_get_device_Features, v -> {
            if (!isConnect) return;
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
        });
    }

    // BTC获取地址按钮初始化
    private void initBtcGetAddress() {
        findViewById(R.id.btn_btc_getAddress, v -> {
            if (!isConnect) return;
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
        });
    }

    // QRc20转账按钮初始化
    private void initQrc20Trans() {
        findViewById(R.id.btn_qrc20_trans, v -> {
            if (!isConnect) return;
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
        });
    }

    // BTC转账按钮初始化
    private void initBtcTrans() {
        findViewById(R.id.btn_btc_trans, v -> {
            if (!isConnect) return;
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
        });
    }

    // BTC多签转账按钮初始化
    private void initBtcMultiSigTrans() {
        findViewById(R.id.btn_btc_multisig_trans, v -> {
            if (!isConnect) return;
            showProgress();
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
        });
    }

    // 发送APDU按钮初始化
    private void initSendApdu() {
        findViewById(R.id.btn_send_apdu, v -> {
            if (!isConnect) return;
            String apduStr = mEditApdu.getText().toString().trim().replace(" ", "");
            if (TextUtils.isEmpty(apduStr)) return;
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
        });
    }

    // 枚举Applet按钮初始化
    private void initApplet() {
        findViewById(R.id.btn_applet, v -> {
            if (!isConnect) return;
            showProgress();
            mJubiter.enumApplets(new JubCallback<List<String>, Void>() {
                @Override
                public void onSuccess(List<String> strings, Void aVoid) {
                    dismissProgress();
                    getAppletVersion(strings);
                }

                @Override
                public void onFailed(int errorCode) { dismissProgress(); }
            });
        });
    }

    // ETH转账按钮初始化
    private void initEthTrans() {
        findViewById(R.id.btn_eth_trans, v -> {
            if (!isConnect) return;
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
        });
    }

    // ETH转账按钮初始化-设置network信息
    private void initEthTransCustom() {
        findViewById(R.id.btn_eth_custom, v -> {
            if (!isConnect) return;
            showProgress();
            mJubiter.ethTransCustom(new JubCallback<String, Void>() {
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
        });
    }

    // BTC显示地址按钮初始化
    private void initBtcShowAddress() {
        findViewById(R.id.btn_btc_show_address, v -> {
            if (!isConnect) return;
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
        });
    }

    // BTC设置地址按钮初始化
    private void initBtcSetMyAddress() {
        findViewById(R.id.btn_btc_set_my_address, v -> {
            if (!isConnect) return;
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
        });
    }

    // BTC USDT转账按钮初始化
    private void initBtcUsdtTrans() {
        findViewById(R.id.btn_btc_usdt_trans, v -> {
            if (!isConnect) return;
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
        });
    }

    // 设置超时按钮初始化
    private void initSetTimeOut() {
        findViewById(R.id.btn_set_time_out, v -> {
            if (!isConnect) return;
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
        });
    }

    // 设置标签按钮初始化
    private void initSetLable() {
        findViewById(R.id.btn_set_Lable, v -> {
            if (!isConnect) return;
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
        });
    }

    // ETH显示地址按钮初始化
    private void initEthShowAddress() {
        findViewById(R.id.btn_eth_showAddress, v -> {
            if (!isConnect) return;
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
        });
    }

    // ETH获取地址按钮初始化
    private void initEthGetAddress() {
        findViewById(R.id.btn_eth_getAddress, v -> {
            if (!isConnect) return;
            showProgress();
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
        });
    }

    // BTC获取主HD节点按钮初始化
    private void initBtcGetMainHDNode() {
        findViewById(R.id.btn_btc_getMainHDNode, v -> {
            if (!isConnect) return;
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
        });
    }

    // ETH获取主HD节点按钮初始化
    private void initEthGetMainHDNode() {
        findViewById(R.id.btn_eth_getMainHDNode, v -> {
            if (!isConnect) return;
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
        });
    }

    // ETH Uniswap交易按钮初始化
    private void initEthTransactionUniswapV2Router02() {
        findViewById(R.id.btn_eth_transaction_UniswapV2Router02, v -> {
            if (!isConnect) return;
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
        });
    }

    // TRX获取地址按钮初始化
    private void initTrxGetAddress() {
        findViewById(R.id.btn_trx_get_address, v -> {
            if (!isConnect) return;
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
        });
    }

    // TRX转账按钮初始化
    private void initTrxTx() {
        findViewById(R.id.btn_trx_tx, v -> {
            if (!isConnect) return;
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
        });
    }

    // 枚举支持币种按钮初始化
    private void initEnumSupportCoins() {
        findViewById(R.id.btn_enum_support_coins, v -> {
            if (!isConnect) return;
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
        });
    }

    // HC获取地址按钮初始化
    private void initHcGetAddress() {
        findViewById(R.id.btn_hc_getAddress, v -> {
            if (!isConnect) return;
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
        });
    }

    // HC获取主HD节点按钮初始化
    private void initHcGetMainHDNode() {
        findViewById(R.id.btn_hc_getMainHDNode, v -> {
            if (!isConnect) return;
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
        });
    }

    // HC显示地址按钮初始化
    private void initHcShowAddress() {
        findViewById(R.id.btn_hc_showAddress, v -> {
            if (!isConnect) return;
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
        });
    }

    // HC转账按钮初始化
    private void initHcTransaction() {
        findViewById(R.id.btn_hc_transaction, v -> {
            if (!isConnect) return;
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
        });
    }

    // EOS转账按钮初始化
    private void initEosTransaction() {
        findViewById(R.id.btn_eos_transaction, v -> {
            if (!isConnect) return;
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
        });
    }

    // XRP获取地址按钮初始化
    private void initXrpGetAddress() {
        findViewById(R.id.btn_xrp_get_address, v -> {
            if (!isConnect) return;
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
        });
    }

    // XRP转账按钮初始化
    private void initXrpTransaction() {
        findViewById(R.id.btn_xrp_transaction, v -> {
            if (!isConnect) return;
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
        });
    }

    // ETH构建交易按钮初始化
    private void initEthBuild() {
        findViewById(R.id.btn_eth_build, v -> {
            if (!isConnect) return;
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
        });
    }

    // ETH合约按钮初始化
    private void initEthContract() {
        findViewById(R.id.btn_eth_contract, v -> {
            if (!isConnect) return;
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
        });
    }

    // ETH ERC20按钮初始化
    private void initEthErc20() {
        findViewById(R.id.btn_eth_erc20, v -> {
            if (!isConnect) return;
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
        });
    }

    // ETH ERC20按钮初始化-设置token信息
    private void initEthErc20Custom() {
        findViewById(R.id.btn_eth_erc20_custom, v -> {
            if (!isConnect) return;
            showProgress();
            mJubiter.ethErc20Custom(new JubCallback<String, String>() {
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
        });
    }

    private void initEthUploadNFT() {
        findViewById(R.id.btn_eth_upload_nft, v -> {
            if (!isConnect) return;
            showProgress();
            mJubiter.ethUploadNFT(new JubCallback<String, String>() {
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
        });
    }

    // ETH TypedData按钮初始化
    private void initEthSignBytestring() {
        findViewById(R.id.btn_eth_sign_Bytestring, v -> {
            if (!isConnect) return;
            showProgress();
            mJubiter.ethSignBytestring(new JubCallback<String, Void>() {
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
        });
    }

    // ETH TypedData按钮初始化
    private void initEthTypedData() {
        findViewById(R.id.btn_eth_TypedData, v -> {
            if (!isConnect) return;
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
        });
    }

    // FIL获取地址按钮初始化
    private void initFilGetAddress() {
        findViewById(R.id.btn_fil_get_address, v -> {
            if (!isConnect) return;
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
        });
    }

    // FIL转账按钮初始化
    private void initFilTx() {
        findViewById(R.id.btn_fil_tx, v -> {
            if (!isConnect) return;
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
        });
    }

    private void initSolGetAddress() {
        findViewById(R.id.btn_sol_get_address, v -> {
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
        });
    }

    private void initSolTx() {
        findViewById(R.id.btn_sol_tx, v -> {
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
        });
    }

    private void initSolTokenTx() {
        findViewById(R.id.btn_sol_token_tx, v -> {
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
        });
    }

    private void initSolTokenCreateTx() {
        findViewById(R.id.btn_sol_token_create_tx, v -> {
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
        });
    }

    // Passphrase按钮初始化
    private void initPassphrase() {
        findViewById(R.id.btn_passphrase, v -> {
            if (!isConnect) return;
            passphrase();
        });
    }

    private void initUsePassphrase() {
        findViewById(R.id.btn_use_passphrase, v -> {
            usePassphrase();
        });
    }

    // 选择固件文件按钮初始化
    private void initSelectFirmware() {
        findViewById(R.id.btn_select_firmware, v -> {
            if (!isConnect) return;
            selectFirmwareFile();
        });
    }

    // 选择资源文件按钮初始化
    private void initSelectResource() {
        findViewById(R.id.btn_select_resource, v -> {
            if (!isConnect) return;
            selectResourceFile();
        });
    }

    // 更新固件按钮初始化
    private void initUpdateFirmware() {
        findViewById(R.id.btn_update_firmware, v -> {
            if (!isConnect) return;
            updateFirmware();
        });
    }

    // 更新资源按钮初始化
    private void initUpdateSource() {
        findViewById(R.id.btn_update_source, v -> {
            if (!isConnect) return;
            updateSource();
        });
    }

    private void findViewById(int id, View.OnClickListener listener) {
        findViewById(id).setOnClickListener(listener);
    }

    private void selectFirmwareFile() {
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("application/octet-stream");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        try {
            startActivityForResult(Intent.createChooser(intent, "Select a ZIP file"), 1);
        } catch (ActivityNotFoundException e) {
            e.printStackTrace();
        }
    }

    private void selectResourceFile() {
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("application/zip");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        try {
            startActivityForResult(Intent.createChooser(intent, "Select a ZIP file"), 2);
        } catch (ActivityNotFoundException e) {
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

    private void useCertAndSignData() {
        findViewById(R.id.btn_use_getcertandsigndata, v -> {
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
        });
    }


    private void getAppletVersion(final List<String> strings) {
        mJubiter.getAppletVersion(strings, new JubCallback<String, Void>() {
            @Override
            public void onSuccess(String s, Void aVoid) {
                showMsg(s);
            }

            @Override
            public void onFailed(int errorCode) {}
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
        if (requestCode == REQUEST_PERMISSION && perms.containsAll(mPermissionList)) {
            switchBt();
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
        EasyPermissions.onRequestPermissionsResult(requestCode, permissions, grantResults, this);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 1 && resultCode == RESULT_OK && data != null && data.getData() != null) {
            firmwareUri = data.getData();
            Log.d(TAG, "BINs URI: " + firmwareUri.toString());
            ((TextView) findViewById(R.id.text_firmware_info)).setText(firmwareUri.toString());
        }
        if (requestCode == 2 && resultCode == RESULT_OK && data != null && data.getData() != null) {
            resourceUri = data.getData();
            Log.d(TAG, "ZIP URI: " + resourceUri.toString());
            ((TextView) findViewById(R.id.text_resource_info)).setText(resourceUri.toString());
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