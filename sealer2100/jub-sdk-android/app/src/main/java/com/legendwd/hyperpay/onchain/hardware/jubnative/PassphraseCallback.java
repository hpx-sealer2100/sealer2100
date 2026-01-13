package com.legendwd.hyperpay.onchain.hardware.jubnative;

import com.legendwd.hyperpay.onchain.hardware.jubnative.utils.InputPassphrase;

/**
 * 转发 passphrase
 */
public interface PassphraseCallback {

    void onPassphrase(InputPassphrase inputPassphrase);
}
