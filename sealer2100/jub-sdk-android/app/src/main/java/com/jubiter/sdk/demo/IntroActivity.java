package com.jubiter.sdk.demo;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

public class IntroActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_intro);

        // 注册所有按钮点击事件
        initMainButton();
        initEleButton();
        initApduButton();
    }

    // 主界面按钮初始化
    private void initMainButton() {
        findViewById(R.id.btn_main, v ->
                startActivity(new Intent(IntroActivity.this, MainActivity.class))
        );
    }

    // 测试界面按钮初始化
    private void initEleButton() {
        findViewById(R.id.btn_ele, v ->
                startActivity(new Intent(IntroActivity.this, TestActivity.class))
        );
    }

    // APDU测试按钮初始化
    private void initApduButton() {
        findViewById(R.id.btn_apdu, v ->
                startActivity(new Intent(IntroActivity.this, FidoTestActivity.class))
        );
    }

    // 简化的findViewById重载，用于注册点击事件
    private void findViewById(int id, View.OnClickListener listener) {
        findViewById(id).setOnClickListener(listener);
    }

    // 原onClick方法保留空实现（避免布局onClick回调冲突）
    public void onClick(View view) {
        // 按钮事件已通过独立init方法注册，此处留空
    }
}