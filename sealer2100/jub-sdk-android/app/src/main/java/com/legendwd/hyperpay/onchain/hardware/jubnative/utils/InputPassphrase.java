package com.legendwd.hyperpay.onchain.hardware.jubnative.utils;

import androidx.annotation.NonNull;

public class InputPassphrase {

    private String passphrase;
    private boolean onDevice;

    public InputPassphrase(String passphrase, boolean onDevice) {
        this.passphrase = passphrase;
        this.onDevice = onDevice;
    }

    public String getPassphrase() {
        return passphrase;
    }

    public void setPassphrase(String passphrase) {
        this.passphrase = passphrase;
    }

    public boolean getOnDevice() {
        return onDevice;
    }

    public void setOnDevice(boolean onDevice) {
        this.onDevice = onDevice;
    }

    @NonNull
    @Override
    public String toString() {
        return "InputPassphrase{" +
                "passphrase='" + passphrase + '\'' +
                ", onDevice=" + onDevice +
                '}';
    }
}
