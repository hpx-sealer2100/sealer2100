package com.jubiter.sdk.demo.utils;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.Uri;
import android.os.Environment;
import android.provider.OpenableColumns;
import android.text.TextUtils;
import android.util.Log;


import com.google.gson.Gson;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

/**
 * @Date 2018/4/19  16:06
 * @Author ZJF
 * @Version 1.0
 */
public class FileUtils {

    public static List<String> getApdu() {
        return getApduStrList();
    }

    private static List<String> getApduStrList() {
        List<String> result = new ArrayList<>();
        String path = Environment.getExternalStorageDirectory().getPath();
        File file = new File(path + File.separator + "jubiter_apdu.txt");
        if (file.exists()) {
            BufferedReader br = null;
            try {
                br = new BufferedReader(new FileReader(file));
                String str = null;
                while ((str = br.readLine()) != null) {
                    result.add(str);
                }
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                try {
                    if (br != null) {
                        br.close();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return result;
    }


    /**
     * 从 Uri 读取文件内容为字节数组
     *
     * @param uri 文件的 Uri
     * @return 文件内容的字节数组
     * @throws IOException 读取文件时发生的异常
     */
    public static byte[] readFileFromUri(Activity activity, Uri uri) throws IOException {
        java.io.InputStream inputStream = activity.getContentResolver().openInputStream(uri);
        if (inputStream == null) {
            return null;
        }

        java.io.ByteArrayOutputStream byteBuffer = new java.io.ByteArrayOutputStream();
        int bufferSize = getFileSize(activity, uri);
        Log.d("FS", "readFileFromUri size: " + bufferSize);
        byte[] buffer = new byte[bufferSize];
        int len;
        while ((len = inputStream.read(buffer)) != -1) {
            byteBuffer.write(buffer, 0, len);
        }
        inputStream.close();
        return byteBuffer.toByteArray();
    }


    /**
     * 根据 Uri 获取文件大小
     *
     * @param uri 文件的 Uri
     * @return 文件大小，如果获取失败则返回 0
     */
    public static int getFileSize(Activity activity, Uri uri) {
        int result = 0;
        if (uri.getScheme().equals("content")) {
            try (Cursor cursor = activity.getContentResolver().query(uri, null, null, null, null)) {
                if (cursor != null && cursor.moveToFirst()) {
                    int sizeIndex = cursor.getColumnIndex(OpenableColumns.SIZE);
                    if (sizeIndex != -1) {
                        result = cursor.getInt(sizeIndex);
                    }
                }
            }
        }
        if (result == 0) {
            try (InputStream inputStream = activity.getContentResolver().openInputStream(uri)) {
                if (inputStream != null) {
                    result = inputStream.available();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return result;
    }
}
