package com.legendwd.hyperpay.onchain.hardware.jubnative.utils;

import android.util.Log;

import androidx.core.util.Supplier;

public final class LogUtil {

    public static final boolean DEBUG = false;
    private static final int CALLER_INDEX = 3;

    private LogUtil() {
        throw new AssertionError("No instances.");
    }

    public static void d(String tag, String msg) {
        log(Log.DEBUG, tag, msg);
    }

    public static void i(String tag, String msg) {
        log(Log.INFO, tag, msg);
    }

    public static void w(String tag, String msg) {
        log(Log.WARN, tag, msg);
    }

    public static void e(String tag, String msg) {
        log(Log.ERROR, tag, msg);
    }

    public static void e(String tag, Throwable tr) {
        log(Log.ERROR, tag, Log.getStackTraceString(tr));
    }

    /**
     * 延迟日志打印，仅在 DEBUG 模式下生效。
     *
     * @param tag
     * @param msgSupplier
     */

    public static void d(String tag, Supplier<String> msgSupplier) {
        lazyLog(Log.DEBUG, tag, msgSupplier);
    }

    public static void i(String tag, Supplier<String> msgSupplier) {
        lazyLog(Log.INFO, tag, msgSupplier);
    }

    public static void w(String tag, Supplier<String> msgSupplier) {
        lazyLog(Log.WARN, tag, msgSupplier);
    }

    public static void e(String tag, Supplier<String> msgSupplier) {
        lazyLog(Log.ERROR, tag, msgSupplier);
    }

    private static void lazyLog(int priority, String tag, Supplier<String> msgSupplier) {
        if (!DEBUG || msgSupplier == null) return;

        String msg = msgSupplier.get();
        log(priority, tag, msg);
    }

    private static void log(int priority, String tag, String msg) {
        if (!DEBUG || msg == null) return;

        // 自动获取调用类名作为 TAG
        if (tag == null) {
            StackTraceElement caller = Thread.currentThread().getStackTrace()[CALLER_INDEX];
            tag = caller.getClassName();
            int dot = tag.lastIndexOf('.');
            if (dot != -1) tag = tag.substring(dot + 1);
            // 可选：添加行号 → msg = "[" + caller.getLineNumber() + "] " + msg;
        }

        // 分段打印防截断（Android 单条日志上限 ~4KB）
        int max = 3000;
        for (int i = 0, len = msg.length(); i < len; i += max) {
            Log.println(priority, tag, msg.substring(i, Math.min(i + max, len)));
        }
    }

}