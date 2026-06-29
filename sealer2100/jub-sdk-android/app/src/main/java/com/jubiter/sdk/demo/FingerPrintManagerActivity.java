package com.jubiter.sdk.demo;


import android.app.ProgressDialog;
import android.content.Context;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.ScrollView;
import android.widget.TextView;


import androidx.appcompat.app.AppCompatActivity;

import com.jubiter.sdk.demo.impl.JubCallback;
import com.jubiter.sdk.demo.impl.JubiterImpl;
import com.jubiter.sdk.demo.utils.InputDialog;
import com.jubiter.sdk.demo.utils.SelectAdapter;
import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.StrUtil;

import java.util.ArrayList;
import java.util.List;

public class FingerPrintManagerActivity extends AppCompatActivity {

    private ScrollView mScrollView;
    private TextView mTxtLog;
    private Context mContext;
    private List<String> mFingerprintList;
    private SelectAdapter mSelectAdapter;
    private JubiterImpl mJubiter;
    private ProgressDialog mDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_finger_print_manager);
        mContext = this;
        mFingerprintList = new ArrayList<>();
        initView();
        mJubiter = JubiterImpl.getInstance(mContext);
        mDialog = new ProgressDialog(this);
        enumFinger();

        // 注册所有按钮点击事件
        initDeleteButton();
        initEnrollButton();
        initVerifyButton();
        initEraseAllButton();
        initTransButton();
    }

    private void initView() {
        ListView mListView = findViewById(R.id.list_finger);
        mScrollView = findViewById(R.id.scrollView);
        mTxtLog = findViewById(R.id.txt_log);

        mSelectAdapter = new SelectAdapter(mContext, mFingerprintList);
        mListView.setAdapter(mSelectAdapter);
        mListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                deleteFingerprint(new Byte(mFingerprintList.get(i)));
            }
        });
    }

    // 清除日志按钮初始化
    private void initDeleteButton() {
        findViewById(R.id.delete, v -> mTxtLog.setText(""));
    }

    // 指纹录入按钮初始化
    private void initEnrollButton() {
        findViewById(R.id.btn_enroll, v -> enroll());
    }

    // 验证按钮初始化
    private void initVerifyButton() {
        findViewById(R.id.btn_verify, v -> nineGridsVerify());
    }

    // 清除所有指纹按钮初始化
    private void initEraseAllButton() {
        findViewById(R.id.btn_erase_all, v -> eraseAll());
    }

    // 转账按钮初始化
    private void initTransButton() {
        findViewById(R.id.btn_trans, v -> transfer());
    }

    // 简化 findViewById 重载，用于注册点击事件
    private void findViewById(int id, View.OnClickListener listener) {
        findViewById(id).setOnClickListener(listener);
    }

    // 移除原有的 switch 逻辑，保留空实现避免布局 onClick 回调冲突
    public void onClick(View view) {
        // 所有按钮事件已通过独立 init 方法注册，此处留空
    }

    private void transfer() {
        mJubiter.btcTransBio(new JubCallback<String, Void>() {
            @Override
            public void onSuccess(String s, Void aVoid) {
                showLog(s);
            }

            @Override
            public void onFailed(int errorCode) {
                showLog("btcTransBio " + errorCode);
            }
        });
    }

    private void eraseAll() {
        mJubiter.eraseFingerprint(new JubCallback<Void, Void>() {
            @Override
            public void onSuccess(Void aVoid, Void aVoid2) {
                showLog("eraseFingerprint success");
                enumFinger();
            }

            @Override
            public void onFailed(int errorCode) {
                showLog("eraseFingerprint " + errorCode);
            }
        });
    }

    private void enroll() {
        mJubiter.enrollFingerprint(new JubCallback<String, Byte>() {
            @Override
            public void onSuccess(String s, Byte aByte) {
                if (TextUtils.isEmpty(s)) {
                    showLog("enrollFingerprint success");
                    enumFinger();
                } else {
                    showLog("FgpNextIndex:" + aByte + " " + s);
                }
            }

            @Override
            public void onFailed(int errorCode) {
                showLog("enrollFingerprint " + errorCode);
            }
        });
    }

    private void enumFinger() {
        mJubiter.enumFingerprint(new JubCallback<List<String>, Void>() {
            @Override
            public void onSuccess(List<String> list, Void aVoid) {
                showLog("enumFingerprint " + list);
                mSelectAdapter.clean();
                for (String item : list) {
                    mSelectAdapter.addItem(item);
                }
                mSelectAdapter.notifyDataSetChanged();
            }

            @Override
            public void onFailed(int errorCode) {
                showLog("enumFingerprint " + errorCode);
                mSelectAdapter.clean();
                mSelectAdapter.notifyDataSetChanged();
            }
        });
    }


    private void nineGridsVerify() {
        mJubiter.identityShowNineGrids(new JubCallback<Void, Void>() {
            @Override
            public void onSuccess(Void aVoid, Void aVoid2) {
                showLog("identityShowNineGrids success");
                new InputDialog(mContext, new InputDialog.callback() {
                    @Override
                    public void onClickListener(String pin) {
                        mJubiter.identityVerifyPIN(2, pin, new JubCallback<Void, Void>() {
                            @Override
                            public void onSuccess(Void aVoid, Void aVoid2) {
                                showLog("identityVerifyPIN success");
                            }

                            @Override
                            public void onFailed(int errorCode) {
                                showLog("identityVerifyPIN " + errorCode);
                            }
                        });
                    }

                    @Override
                    public void onCancel() {
                        mJubiter.identityCancelNineGrids(new JubCallback<Void, Void>() {
                            @Override
                            public void onSuccess(Void aVoid, Void aVoid2) {
                                showLog("identityCancelNineGrids success");
                            }

                            @Override
                            public void onFailed(int errorCode) {
                                showLog("identityVerifyPIN " + errorCode);
                            }
                        });
                    }
                }).show();
            }

            @Override
            public void onFailed(int errorCode) {
                showLog("identityShowNineGrids " + errorCode);
            }
        });
    }


    private void deleteFingerprint(Byte b) {
        mJubiter.deleteFingerprint(b, new JubCallback<Void, Void>() {
            @Override
            public void onSuccess(Void s, Void v) {
                showLog("deleteFingerprint success");
                enumFinger();
            }

            @Override
            public void onFailed(int errorCode) {
                showLog("deleteFingerprint " + errorCode);
            }
        });
    }


    void showProgressDialog(String msg) {
        mDialog.setMessage(msg);
        mDialog.show();
    }


    void dismissProgressDialog() {
        mDialog.dismiss();
    }


    void showLog(final String tip) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                StrUtil.parseLog(tip, mScrollView, mTxtLog);
            }
        });
    }
}