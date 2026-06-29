package com.jubiter.sdk.demo;

import android.Manifest;
import android.os.Bundle;
import android.os.Handler;
import android.text.Html;
import android.view.View;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Size;
import androidx.appcompat.app.AppCompatActivity;

import com.jubiter.sdk.demo.ble.FTBTDevice;
import com.jubiter.sdk.demo.ble.FtBTConnectCallback;
import com.jubiter.sdk.demo.impl.JubCallback;
import com.jubiter.sdk.demo.impl.JubiterImpl;
import com.jubiter.sdk.demo.utils.FileUtils;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

import pub.devrel.easypermissions.EasyPermissions;

public class FidoTestActivity extends AppCompatActivity {

    private SimpleDateFormat mFormat;
    private Button mBtnScan;
    private TextView mTxtLog;
    private TextView mTxtState;
    private ScrollView mScrollView;
    private boolean isConnect = false;
    private JubiterImpl jubiter;
    private final static int REQUEST_PERMISSION = 0x1002;
    private List<String> apduList;

    public static final int LOG_SEND = 0;
    public static final int LOG_RECV = 1;
    public static final int LOG_ERROR = 2;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fido_test);
        initView();
        init();
        // 注册所有按钮点击事件
        initClearLog();
        initScanButton();
        initSendApduButton();
    }

    private void initView() {
        mBtnScan = findViewById(R.id.btn_scan);
        mTxtLog = findViewById(R.id.tv_message);
        mScrollView = findViewById(R.id.scrollView);
        mTxtState = findViewById(R.id.text_state);
    }

    private void init() {
        mFormat = new SimpleDateFormat("yyyy-MM-dd   hh:mm:ss");
        jubiter = JubiterImpl.getInstance(this);

        if (!hasPermissions()) {
            requestPermissions("Permission request", REQUEST_PERMISSION,
                    Manifest.permission.ACCESS_COARSE_LOCATION,
                    Manifest.permission.ACCESS_FINE_LOCATION,
                    Manifest.permission.READ_EXTERNAL_STORAGE);
        }
    }

    // 清除日志按钮初始化
    private void initClearLog() {
        findViewById(R.id.img_clear_log, v -> mTxtLog.setText(""));
    }

    // 扫描连接按钮初始化
    private void initScanButton() {
        findViewById(R.id.btn_scan, v -> switchBt());
    }

    // 发送APDU按钮初始化
    private void initSendApduButton() {
        findViewById(R.id.btn_send_apdu, v -> {
            if (isConnect) {
                apduList = FileUtils.getApdu();
                sendApdu();
            } else {
                Toast.makeText(FidoTestActivity.this, "未连接", Toast.LENGTH_SHORT).show();
            }
        });
    }

    // 简化的findViewById重载，用于注册点击事件
    private void findViewById(int id, View.OnClickListener listener) {
        findViewById(id).setOnClickListener(listener);
    }

    public boolean hasPermissions() {
        return EasyPermissions.hasPermissions(this,
                Manifest.permission.ACCESS_COARSE_LOCATION,
                Manifest.permission.ACCESS_FINE_LOCATION,
                Manifest.permission.READ_EXTERNAL_STORAGE);
    }

    public void requestPermissions(@NonNull String rationale,
                                   int requestCode, @Size(min = 1) @NonNull String... perms) {
        EasyPermissions.requestPermissions(this, rationale, requestCode, perms);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (jubiter != null) {
            jubiter.onDestroy();
        }
    }

    private void sendApdu() {
        if(apduList == null || apduList.size() == 0){
            return;
        }
        String apdu = apduList.remove(0);
        sendToMain(apdu, LOG_SEND);
        jubiter.sendApdu(apdu, new JubCallback<String, Void>() {
            @Override
            public void onSuccess(String result, Void aVoid) {
                sendToMain(result, LOG_RECV);
                if(apduList.size() > 0){
                    sendApdu();
                }
            }

            @Override
            public void onFailed(int errorCode) {
                sendToMain(errorCode + "", LOG_ERROR);
            }
        });
    }

    private void sendToMain(final String s, final int logType) {
        runOnUiThread(() -> {
            long currentTimeMillis = System.currentTimeMillis();
            mTxtLog.append("Time:\t" + mFormat.format(new Date(currentTimeMillis)) + "\n");
            if (logType == LOG_SEND) {
                mTxtLog.append(Html.fromHtml("<p><font color=\"#85d46f\">发送:<br>" + s + "</p>"));
            } else if (logType == LOG_RECV) {
                mTxtLog.append(Html.fromHtml("<p><font color=\"#55c0e4\">接收:<br>" + s + "</p>"));
            } else if (logType == LOG_ERROR) {
                mTxtLog.append(Html.fromHtml("<p><font color=\"#f92743\">ERROR:<br>" + s + "</p>"));
            }
            scrollBottom(mScrollView, mTxtLog);
        });
    }

    private void switchBt() {
        if (isConnect) {
            jubiter.disConnectDevice();
            mBtnScan.setText("连接");
            isConnect = false;
        } else {
            jubiter.connect(new FtBTConnectCallback() {
                @Override
                public void connected(FTBTDevice device, int code) {
                    if (code == 0) {
                        mTxtState.setText(device.getName() + "\n" + device.getMac());
                        mBtnScan.setText("断开");
                        mTxtLog.setText("");
                        isConnect = true;
                    } else {
                        mTxtLog.setText(String.format("连接失败：0x%x", code) + "\n");
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

    public void scrollBottom(final ScrollView scroll, final View inner) {
        new Handler().post(() -> {
            if (scroll == null || inner == null) return;
            int offset = inner.getMeasuredHeight() - scroll.getMeasuredHeight();
            if (offset < 0) offset = 0;
            scroll.scrollTo(0, offset);
        });
    }
}