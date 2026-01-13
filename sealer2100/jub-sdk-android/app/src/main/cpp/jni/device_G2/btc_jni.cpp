//
// Created by feng on 2025/6/4.
//

#include <jni.h>
#include "btc_jni.h"
#include "common_jni.h"
#include "json/reader.h"
#include "utils/logUtils.h"
#include "jni_registry.h"
#include <JUB_SDK.h>


#ifdef __cplusplus
extern "C" {
#endif



JNIEXPORT jint JNICALL
native_BTCCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jboolean isMultiSig,
                        jstring jJSON,
                        jlong deviceInfo) {

#define MAIN_PATH      "main_path"
#define P2SH_SEGWIT    "p2sh_segwit"
#define M              "m"
#define N              "n"
#define MASTER_KEY     "cosigner"
#define COIN_TYPE_BTC  "coin_type"

    if (NULL == jJSON) {
        return JUBR_ARGUMENTS_BAD;
    }

    int length = env->GetStringLength(jJSON);
    if (0 == length) {
        errorCode = JUBR_ARGUMENTS_BAD;
        return JUBR_ARGUMENTS_BAD;
    }

    JUB_UINT16 *pContextID = (JUB_UINT16 *) env->GetIntArrayElements(jContextId, NULL);
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    CONTEXT_CONFIG_BTC cfg;
    CONTEXT_CONFIG_MULTISIG_BTC multiCfg;
    cfg.mainPath = (char *) root[MAIN_PATH].asCString();
    int cointype = root[COIN_TYPE_BTC].asInt();
    switch (cointype) {
        case 0x00:
            cfg.coinType = COINBTC;
            break;
        case 0x01:
            cfg.coinType = COINBCH;
            break;
        case 0x02:
            cfg.coinType = COINLTC;
            break;
        case 0x03:
            cfg.coinType = COINUSDT;
            break;
        case 0x04:
            cfg.coinType = COINDASH;
            break;
        case 0x05:
            cfg.coinType = COINQTUM;
            break;
        case 0x06:
            cfg.coinType = COINDOGE;
            break;
        default:
            cfg.coinType = COINBTC;
            break;
    }

    // 缓存是否是多重签名标记
    globalMultiSig = isMultiSig;

    JUB_RV rv = JUBR_OK;
    if (isMultiSig) {
        multiCfg.transType = p2sh_multisig;
        multiCfg.mainPath = cfg.mainPath;
        multiCfg.coinType = cfg.coinType;
        multiCfg.m = root[M].asInt64();
        multiCfg.n = root[N].asInt64();

        int keySize = root[MASTER_KEY].size();
        std::vector<std::string> masterKey;
        for (int i = 0; i < keySize; ++i) {
            std::string key = root[MASTER_KEY][i].asString();
            masterKey.push_back(key);
        }
        multiCfg.vCosignerMainXpub = masterKey;

        rv = JUB_CreateContextBTC(&multiCfg, deviceInfo, pContextID);
    } else {
        if (COINBCH == cfg.coinType) {
            cfg.transType = p2pkh;
        } else {
            cfg.transType = p2pkh;
            int itype = root[P2SH_SEGWIT].asInt();
            if(1 == itype){
                cfg.transType = p2pkh;
            } else if(2 == itype){
                cfg.transType = p2wpkh;
            } else if(3 == itype){
                cfg.transType = p2sh_p2wpkh;
            } else if(4 == itype){
                cfg.transType = SPEND_APROOT;
            }
//            if (root[P2SH_SEGWIT].asBool()) {
//                cfg.transType = p2sh_p2wpkh;
//            } else {
//                cfg.transType = p2pkh;
//            }
        }
        rv = JUB_CreateContextBTC(&cfg, deviceInfo, pContextID);
    }

    if (rv != JUBR_OK) {
        LOG_ERR("JUB_CreateContextBTC: %08x", rv);
        errorCode = rv;
    } else {
        LOG_INF("contextID: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, 0);
    return rv;
}


JNIEXPORT jstring JNICALL native_USDTTransaction(JNIEnv *env, jclass obj,
                                                 jlong contextID, jstring jJSON) {
#define INPUTS       "inputs"
#define PREHASH      "preHash"
#define PREINDEX     "preIndex"
#define BIP32_PATH   "bip32_path"
#define CHANGE       "change"
#define INDEX        "addressIndex"
#define AMOUNT       "amount"

#define OUTPUTS      "outputs"
#define ADDRESS      "address"
#define CHANGE_ADDRESS "change_address"

#define USDT_AMOUNT "USDT_amount"
#define USDT_TO     "USDT_to"


    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    std::vector<INPUT_BTC> inputs;
    std::vector<OUTPUT_BTC> outputs;

    int input_number = root[INPUTS].size();
    for (int i = 0; i < input_number; i++) {
        INPUT_BTC input;
        input.preHash = (char *) root[INPUTS][i][PREHASH].asCString();
        input.preIndex = static_cast<JUB_UINT16>(root[INPUTS][i][PREINDEX].asInt());
        input.path.change = (JUB_ENUM_BOOL) root[INPUTS][i][BIP32_PATH][CHANGE].asBool();
        input.path.addressIndex = static_cast<JUB_UINT64>(root[INPUTS][i][BIP32_PATH][INDEX].asInt());
        input.amount = static_cast<JUB_UINT64>(root[INPUTS][i][AMOUNT].asUInt64());
        inputs.push_back(input);
    }

    int output_number = root[OUTPUTS].size();
    for (int i = 0; i < output_number; i++) {
        OUTPUT_BTC output;
        output.stdOutput.address = (char *) root[OUTPUTS][i][ADDRESS].asCString();
        output.stdOutput.amount = static_cast<JUB_UINT64>(root[OUTPUTS][i][AMOUNT].asUInt64());
        output.stdOutput.changeAddress = (JUB_ENUM_BOOL) root[OUTPUTS][i][CHANGE_ADDRESS].asBool();
        if (output.stdOutput.changeAddress) {
            output.stdOutput.path.change = (JUB_ENUM_BOOL) root[OUTPUTS][i][BIP32_PATH][CHANGE].asBool();
            output.stdOutput.path.addressIndex = static_cast<JUB_UINT64>(root[OUTPUTS][i][BIP32_PATH][INDEX].asInt());
        }
        outputs.push_back(output);
    }

    OUTPUT_BTC USDT_outputs[2] = {};

    char *usdt_to = const_cast<char *>(root[USDT_TO].asCString());
    JUB_RV rv = JUB_BuildUSDTOutputs(
            static_cast<JUB_UINT16>(contextID),
            usdt_to,
            root[USDT_AMOUNT].asUInt64(),
            USDT_outputs);
    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    outputs.emplace_back(USDT_outputs[0]);
    outputs.emplace_back(USDT_outputs[1]);

    JUB_SetUnitBTC(static_cast<JUB_UINT16>(contextID), BTC);
    char *raw = NULL;
    rv = JUB_SignTransactionBTC(static_cast<JUB_UINT16>(contextID),
                                JUB_ENUM_BTC_ADDRESS_FORMAT::OWN,
                                &inputs[0],
                                (JUB_UINT16) inputs.size(),
                                &outputs[0], (JUB_UINT16) outputs.size(), 0, &raw);

    // JUBR_MULTISIG_OK 表示多重签名结果不完整，需要后续再次签名
    if (rv != JUBR_OK && rv != JUBR_MULTISIG_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    // 用于判断是否多签完成
    errorCode = rv;
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}

JNIEXPORT jstring JNICALL native_QRC20Transaction(JNIEnv *env, jclass obj,
                                                  jlong contextID, jstring jJSON) {
#define INPUTS       "inputs"
#define PREHASH      "preHash"
#define PREINDEX     "preIndex"
#define BIP32_PATH   "bip32_path"
#define CHANGE       "change"
#define INDEX        "addressIndex"
#define AMOUNT       "amount"

#define OUTPUTS      "outputs"
#define ADDRESS      "address"
#define CHANGE_ADDRESS "change_address"

#define CONTRACT_ADDRESS "QRC20_contractAddr"
#define DECIMAL "QRC20_decimal"
#define SYMBOL "QRC20_symbol"
#define GAS_LIMIT "gasLimit"
#define GAS_PRICE "gasPrice"
#define TOKEN_TO "QRC20_to"
#define TOKEN_VALUE "QRC20_amount"


    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    OUTPUT_BTC QRC20_outputs[1] = {};
    JUB_BuildQRC20Outputs(
            static_cast<JUB_UINT16>(contextID),
            const_cast<JUB_CHAR_PTR>(root[CONTRACT_ADDRESS].asCString()),
            static_cast<JUB_UINT8>(root[DECIMAL].asInt()),
            const_cast<JUB_CHAR_PTR>(root[SYMBOL].asCString()),
            static_cast<JUB_UINT64>(root[GAS_LIMIT].asInt()),
            static_cast<JUB_UINT64>(root[GAS_PRICE].asInt()),
            const_cast<JUB_CHAR_PTR>(root[TOKEN_TO].asCString()),
            const_cast<JUB_CHAR_PTR>(root[TOKEN_VALUE].asCString()),
            QRC20_outputs);

    std::vector<INPUT_BTC> inputs;
    std::vector<OUTPUT_BTC> outputs;

    int input_number = root[INPUTS].size();
    for (int i = 0; i < input_number; i++) {
        INPUT_BTC input;
        input.preHash = (char *) root[INPUTS][i][PREHASH].asCString();
        input.preIndex = static_cast<JUB_UINT16>(root[INPUTS][i][PREINDEX].asInt());
        input.path.change = (JUB_ENUM_BOOL) root[INPUTS][i][BIP32_PATH][CHANGE].asBool();
        input.path.addressIndex = static_cast<JUB_UINT64>(root[INPUTS][i][BIP32_PATH][INDEX].asUInt64());
        input.amount = static_cast<JUB_UINT64>(root[INPUTS][i][AMOUNT].asInt());
        inputs.push_back(input);
    }

    int output_number = root[OUTPUTS].size();
    for (int i = 0; i < output_number; i++) {
        OUTPUT_BTC output;
        output.stdOutput.address = (char *) root[OUTPUTS][i][ADDRESS].asCString();
        output.stdOutput.amount = static_cast<JUB_UINT64>(root[OUTPUTS][i][AMOUNT].asUInt64());
        output.stdOutput.changeAddress = (JUB_ENUM_BOOL) root[OUTPUTS][i][CHANGE_ADDRESS].asBool();
        if (output.stdOutput.changeAddress) {
            output.stdOutput.path.change = (JUB_ENUM_BOOL) root[OUTPUTS][i][BIP32_PATH][CHANGE].asBool();
            output.stdOutput.path.addressIndex = static_cast<JUB_UINT64>(root[OUTPUTS][i][BIP32_PATH][INDEX].asInt());
        }
        outputs.push_back(output);
    }
    outputs.push_back(QRC20_outputs[0]);

    JUB_SetUnitBTC(static_cast<JUB_UINT16>(contextID), BTC);
    char *raw = NULL;
    JUB_RV rv = JUB_SignTransactionBTC(static_cast<JUB_UINT16>(contextID),
                                       JUB_ENUM_BTC_ADDRESS_FORMAT::OWN,
                                       &inputs[0],
                                       (JUB_UINT16) inputs.size(),
                                       &outputs[0], (JUB_UINT16) outputs.size(), 0, &raw);

    // JUBR_MULTISIG_OK 表示多重签名结果不完整，需要后续再次签名
    if (rv != JUBR_OK && rv != JUBR_MULTISIG_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    // 用于判断是否多签完成
    errorCode = rv;
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;

}


JNIEXPORT jstring JNICALL
native_BTC_ShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                       jboolean useLegacyAddress) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::OWN;
    if (useLegacyAddress) {
        addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::LEGACY;
    }

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressBTC(static_cast<JUB_UINT16>(contextID), addrFmt, path, BOOL_TRUE,
                                  &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetAddressBTC: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL
native_BTC_SetMyAddress(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index,
                        jboolean useLegacyAddress) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(change);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::OWN;
    if (useLegacyAddress) {
        addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::LEGACY;
    }

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_SetMyAddressBTC(static_cast<JUB_UINT16>(contextID), addrFmt, path, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetMyAddressBTC: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jobjectArray JNICALL
native_BTCGetAddress(JNIEnv *env, jclass obj, jlong contextID, jboolean useLegacyAddress,
                     jstring jJSON) {

#define BIP32_PATH   "bip32_path"
#define CHANGE       "change"
#define INDEX        "addressIndex"

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, FALSE));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    int input_number = root[BIP32_PATH].size();
    jobjectArray array = env->NewObjectArray(2 * input_number, clazz, 0);
    for (int i = 0; i < input_number; i++) {
        JUB_CHAR_PTR xpub;

        BIP32_Path path;
        path.change = (JUB_ENUM_BOOL) root[BIP32_PATH][i][CHANGE].asBool();
        path.addressIndex = static_cast<JUB_UINT64>(root[BIP32_PATH][i][INDEX].asInt());

        JUB_RV rv = JUB_GetHDNodeBTC(contextID, path, &xpub);
        if (rv != JUBR_OK) {
            LOG_ERR("JUB_GetHDNodeBTC: %08x", rv);
            env->SetObjectArrayElement(array, 2 * i, NULL);
            env->SetObjectArrayElement(array, 2 * i + 1, NULL);
        } else {
            jstring jsXpub = env->NewStringUTF(xpub);

            JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::OWN;
            if (useLegacyAddress) {
                addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::LEGACY;
            }

            JUB_CHAR_PTR pAddress = NULL;
            rv = JUB_GetAddressBTC(contextID, addrFmt, path, BOOL_FALSE, &pAddress);
            if (rv != JUBR_OK) {
                LOG_ERR("JUB_GetAddressBTC: %08x", rv);
                env->SetObjectArrayElement(array, 2 * i, jsXpub);
                env->SetObjectArrayElement(array, 2 * i + 1, NULL);
            } else {
                jstring address = env->NewStringUTF(pAddress);
                env->SetObjectArrayElement(array, 2 * i, jsXpub);
                env->SetObjectArrayElement(array, 2 * i + 1, address);
            }
        }
    }
    return array;
}

JNIEXPORT jstring JNICALL native_BTCGetMainHDNode(JNIEnv *env, jclass obj, jlong contextID) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    JUB_CHAR_PTR xpub;
    JUB_RV rv = JUB_GetMainHDNodeBTC(contextID, &xpub);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetHDNodeBTC: %08x", rv);
        return NULL;
    }
    jstring mainPub = env->NewStringUTF(xpub);
    JUB_FreeMemory(xpub);
    return mainPub;
}


jstring native_ParseTransactionRaw(JNIEnv *env, jclass obj, jlong contextID, jstring jRaw) {

    JUB_CHAR_PTR pRaw = (JUB_CHAR_PTR) env->GetStringUTFChars(jRaw, NULL);

    JUB_UINT32 lockTime;
    std::vector<INPUT_BTC> btcInputVector;
    std::vector<OUTPUT_BTC> btcOutputVector;
    JUB_RV rv = JUB_ParseTransactionBTC(contextID, pRaw, btcInputVector, btcOutputVector,
                                        &lockTime);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_ParseTransactionBTC: %08x", rv);
        errorCode = rv;
        return NULL;
    }

    Json::Value root;
    Json::Value inputArray;
    for (int i = 0; i < btcInputVector.size(); ++i) {
        INPUT_BTC inputBtc = btcInputVector[i];
        Json::Value input;
        input["multisig"] = inputBtc.type;
        input["preHash"] = inputBtc.preHash;
        input["preIndex"] = inputBtc.preIndex;
#if defined(__aarch64__)
        Json::UInt64 tmstp = inputBtc.amount;
        input["amount"] = tmstp;
#else
        input["amount"] = inputBtc.amount;
#endif
        input["nSequence"] = inputBtc.nSequence;
        inputArray.append(input);
    }
    root["inputs"] = inputArray;

    Json::Value outputArray;
    for (int i = 0; i < btcOutputVector.size(); ++i) {
        OUTPUT_BTC outputBtc = btcOutputVector[i];
        Json::Value output;
        output["multisig"] = outputBtc.type;
#if defined(__aarch64__)
        Json::UInt64 tmoAmount = outputBtc.stdOutput.amount;
        output["amount"] = tmoAmount;
#else
        output["amount"] = outputBtc.stdOutput.amount;
#endif
        output["address"] =
                outputBtc.stdOutput.address == nullptr ? "" : outputBtc.stdOutput.address;
        outputArray.append(output);
    }
    root["output"] = outputArray;
    root["lock_time"] = lockTime;

    std::string rawString = root.toStyledString();
    return env->NewStringUTF(rawString.c_str());
}


JNIEXPORT jstring JNICALL
native_BTCTransaction(JNIEnv *env, jclass obj, jlong contextID, jboolean useLegacyAddress,
                      jstring jJSON) {
#define INPUTS       "inputs"
#define PREHASH      "preHash"
#define PREINDEX     "preIndex"
#define BIP32_PATH   "bip32_path"
#define CHANGE       "change"
#define INDEX        "addressIndex"
#define AMOUNT       "amount"
#define MULTISIG     "multisig"

#define OUTPUTS      "outputs"
#define ADDRESS      "address"
#define CHANGE_ADDRESS "change_address"

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    std::vector<INPUT_BTC> inputs;
    std::vector<OUTPUT_BTC> outputs;

    int input_number = root[INPUTS].size();
    for (int i = 0; i < input_number; i++) {
        INPUT_BTC input;
        // 根据全局变量赋值
        input.type = globalMultiSig ? P2SH_MULTISIG : P2PKH;
//        input.type = SCRIPT_BTC_TYPE(root[INPUTS][i][MULTISIG].asInt());
        input.preHash = (char *) root[INPUTS][i][PREHASH].asCString();
        input.preIndex = static_cast<JUB_UINT16>(root[INPUTS][i][PREINDEX].asInt());
        input.path.change = (JUB_ENUM_BOOL) root[INPUTS][i][BIP32_PATH][CHANGE].asBool();
        input.path.addressIndex = static_cast<JUB_UINT64>(root[INPUTS][i][BIP32_PATH][INDEX].asInt());
        input.amount = static_cast<JUB_UINT64>(root[INPUTS][i][AMOUNT].asUInt64());
        inputs.push_back(input);
    }

    int output_number = root[OUTPUTS].size();
    for (int i = 0; i < output_number; i++) {
        OUTPUT_BTC output;
        // 根据全局变量赋值
        output.type = globalMultiSig ? P2SH_MULTISIG : P2PKH;
//        output.type = SCRIPT_BTC_TYPE(root[OUTPUTS][i][MULTISIG].asInt());
        output.stdOutput.address = (char *) root[OUTPUTS][i][ADDRESS].asCString();
        output.stdOutput.amount = static_cast<JUB_UINT64>(root[OUTPUTS][i][AMOUNT].asUInt64());
        output.stdOutput.changeAddress = (JUB_ENUM_BOOL) root[OUTPUTS][i][CHANGE_ADDRESS].asBool();
        if (output.stdOutput.changeAddress) {
            output.stdOutput.path.change = (JUB_ENUM_BOOL) root[OUTPUTS][i][BIP32_PATH][CHANGE].asBool();
            output.stdOutput.path.addressIndex = static_cast<JUB_UINT64>(root[OUTPUTS][i][BIP32_PATH][INDEX].asInt());
        }
        outputs.push_back(output);
    }

    char *raw = NULL;
    JUB_SetUnitBTC(static_cast<JUB_UINT16>(contextID), BTC);

    JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::OWN;
    if (useLegacyAddress) {
        addrFmt = JUB_ENUM_BTC_ADDRESS_FORMAT::LEGACY;
    }

    JUB_RV rv = JUB_SignTransactionBTC(static_cast<JUB_UINT16>(contextID), addrFmt,
                                       &inputs[0], (JUB_UINT16) inputs.size(),
                                       &outputs[0], (JUB_UINT16) outputs.size(), 0, &raw);

    // JUBR_MULTISIG_OK 表示多重签名结果不完整，需要后续再次签名
    if (rv != JUBR_OK && rv != JUBR_MULTISIG_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    // 用于判断是否多签完成
    errorCode = rv;
    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}


#ifdef __cplusplus
}
#endif

// 定义 JNINativeMethod 数组
static JNINativeMethod g2BtcMethods[] = {
        {
                "nativeBTCCreateContext",
                "([IZLjava/lang/String;J)I",
                (void *) native_BTCCreateContext
        },
        {
                "nativeBTCGetAddress",
                "(JZLjava/lang/String;)[Ljava/lang/String;",
                (void *) native_BTCGetAddress
        },
        {
                "nativeBTCGetMainHDNode",
                "(J)Ljava/lang/String;",
                (void *) native_BTCGetMainHDNode
        },
        {
                "nativeBTCShowAddress",
                "(JIIZ)Ljava/lang/String;",
                (void *) native_BTC_ShowAddress
        },
        {
                "nativeBTCSetMyAddress",
                "(JIIZ)Ljava/lang/String;",
                (void *) native_BTC_SetMyAddress
        },

        {
                "nativeBTCTransaction",
                "(JZLjava/lang/String;)Ljava/lang/String;",
                (void *) native_BTCTransaction
        },
        {
                "nativeUSDTTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_USDTTransaction
        },
        {
                "nativeQRC20Transaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_QRC20Transaction
        },
        {
                "nativeParseTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ParseTransactionRaw
        },

};


#define G2_BTC_CLASS "com/legendwd/hyperpay/onchain/hardware/jubnative/NativeApiLL"

jint RegisterG2BtcNatives(JNIEnv* env) {
    LOG_INF("Register G2 BTC");
    REGISTER_NATIVES(env, G2_BTC_CLASS, g2BtcMethods, sizeof(g2BtcMethods) / sizeof(JNINativeMethod));
    return JNI_OK;
}

static JniModuleRegistrar kRegG2Btc(RegisterG2BtcNatives);