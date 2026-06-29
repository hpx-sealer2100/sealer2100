//
// Created by feng on 2025/6/4.
//

#include <jni.h>
#include "eth_jni.h"
#include "JUB_SDK_COMM.h"
#include "utils/logUtils.h"
#include "json/reader.h"
#include "json/writer.h"
#include "common_jni.h"
#include "jni_registry.h"
#include <JUB_SDK.h>


#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jint JNICALL
native_ETHCreateContext(JNIEnv *env, jclass obj, jintArray jContextId, jstring jJSON,
                        jlong deviceInfo) {

#define MAIN_PATH  "main_path"
#define CHAIN_ID   "chainID"

    JUB_UINT16 *pContextID = (JUB_UINT16 *) env->GetIntArrayElements(jContextId, NULL);

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    CONTEXT_CONFIG_ETH cfg;
    cfg.mainPath = (char *) root[MAIN_PATH].asCString();
    cfg.chainID = root[CHAIN_ID].asUInt64();
    int rv = JUB_CreateContextETH(cfg, static_cast<JUB_UINT16>(deviceInfo), pContextID);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_CreateContextETH: %08x", rv);
    } else {
        LOG_INF("JUB_CreateContextETH: %d", *pContextID);
    }
    env->ReleaseIntArrayElements(jContextId, (jint *) pContextID, FALSE);
    return rv;
}


JNIEXPORT jstring JNICALL native_ETH_Transaction(JNIEnv *env, jclass obj, jlong contextID,
                                                 jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["ETH"]["bip32_path"]["change"].asBool();
    path.addressIndex = root["ETH"]["bip32_path"]["addressIndex"].asUInt();

    uint32_t nonce = static_cast<uint32_t>(root["ETH"]["nonce"].asDouble());
    uint32_t gasLimit = static_cast<uint32_t>(root["ETH"]["gasLimit"].asDouble());
    char *gasPriceInWei = (char *) root["ETH"]["gasPriceInWei"].asCString();
    char *valueInWei = (char *) root["ETH"]["valueInWei"].asCString();
    char *to = (char *) root["ETH"]["to"].asCString();
    char *data = (char *) root["ETH"]["data"].asCString();

    char *raw = nullptr;
    JUB_RV rv = JUB_SignTransactionETH(static_cast<JUB_UINT16>(contextID), path,
                                       nonce, gasLimit,
                                       gasPriceInWei, to, valueInWei, data, &raw);
    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}
JNIEXPORT jstring JNICALL native_ETH_SignTypedDataMAX(JNIEnv *env, jclass obj, jlong contextID,
                                                   jstring jJSON) {
    auto pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, nullptr));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    if(nullptr == pJSON) {
        errorCode = JUBR_ARGUMENTS_BAD;
        return NULL;
    }

    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["ETH"]["bip32_path"]["change"].asBool();
    path.addressIndex = root["ETH"]["bip32_path"]["addressIndex"].asUInt();

    const Json::Value& eip712 = root["EIP712"];
    Json::StreamWriterBuilder builder;
    std::string eip712_json = Json::writeString(builder, eip712);

    char *raw = nullptr;
    JUB_RV rv = JUB_SignTypedDataETH_MAX(contextID, path, (JUB_CHAR_PTR )eip712_json.data(), &raw);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}

JNIEXPORT jint JNICALL native_ETH_SetERC20Tokens(JNIEnv *env, jclass obj, jlong contextID,
                                                 jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    std::vector<ERC20_TOKEN_INFO> tokens;
    int tokensCount = root.size();
    for (int i = 0; i < tokensCount; i++) {
        ERC20_TOKEN_INFO info;
        info.tokenName = (JUB_CHAR_PTR) root[i]["tokenName"].asCString();
        info.unitDP = root[i]["dp"].asDouble();
        info.contractAddress = (JUB_CHAR_PTR) root[i]["contractAddress"].asCString();
        tokens.push_back(info);
    }


    JUB_RV rv = JUB_SetERC20TokensETH(contextID, &tokens[0], tokensCount);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetERC20TokensETH: %08x", rv);
    }
    env->ReleaseStringUTFChars(jJSON, pJSON);
    return rv;
}

JNIEXPORT jint JNICALL native_ETH_SetERC20Token(JNIEnv *env, jclass obj, jlong contextID,
                                                jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);


    char *tokenName = (char *) root["ERC20Token"]["tokenName"].asCString();
    uint16_t unitDP = root["ERC20Token"]["dp"].asInt();
    char *contractAddress = (char *) root["ERC20Token"]["contract_address"].asCString();


    JUB_RV rv = JUB_SetERC20TokenETH(contextID, tokenName, unitDP, contractAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetERC20TokenETH: %08x", rv);
    }
    env->ReleaseStringUTFChars(jJSON, pJSON);
    return rv;
}

JNIEXPORT jint JNICALL native_ETH_SetERC20TokenV2(JNIEnv *env, jclass obj, jlong contextID,
                                                  jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);
    const auto& jNetwork = root["network"];

    // network
    const auto* name = jNetwork["name"].asCString();
    const auto* symbol = jNetwork["symbol"].asCString();
    auto chain_id = jNetwork["chain_id"].asUInt64();
    auto slip44 = jNetwork["slip44"].asUInt64();
    const auto network = JUB_ETH_NETWORK_INFO{
            .name = name,
            .symbol = symbol,
            .chainID = chain_id,
            .slip44ID = slip44,
    };

    // token
    const auto& jToken = root["token"];
    if (chain_id != jToken["chain_id"].asUInt64()) {
        env->ReleaseStringUTFChars(jJSON, pJSON);
        return JUBR_ARGUMENTS_BAD;
    }
    const auto* token_name = jToken["name"].asCString();
    const auto* token_symbol = jToken["symbol"].asCString();
    auto decimals = jToken["decimals"].asUInt();
    const auto* address = jToken["address"].asCString();
    const auto token = JUB_ERC20_TOKEN_INFO{
        .chainID = chain_id,
        .name = token_name,
        .symbol = token_symbol,
        .address = address,
        .decimals = decimals,
    };

    auto rv = JUB_SetERC20TokenETHV2(contextID, network, token);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetERC20TokenETHV2: %08x", rv);
    }
    env->ReleaseStringUTFChars(jJSON, pJSON);
    return rv;
}


JNIEXPORT jint JNICALL native_ETH_SetNetworkV2(JNIEnv *env, jclass obj, jlong contextID,
                                               jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);
    const auto& jNetwork = root["network"];

    // network
    const auto* name = jNetwork["name"].asCString();
    const auto* symbol = jNetwork["symbol"].asCString();
    auto chain_id = jNetwork["chain_id"].asUInt64();
    auto slip44 = jNetwork["slip44"].asUInt64();
    const auto network = JUB_ETH_NETWORK_INFO{
        .name = name,
        .symbol = symbol,
        .chainID = chain_id,
        .slip44ID = slip44,
    };

    auto rv = JUB_SetNetworkETHV2(contextID, network);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetNetworkETHV2: %08x", rv);
    }
    env->ReleaseStringUTFChars(jJSON, pJSON);
    return rv;
}

JNIEXPORT jstring JNICALL native_ETH_BuildERC20TransferAbi(JNIEnv *env, jclass obj, jlong contextID,
                                                           jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    char *token_to = (char *) root["ERC20"]["token_to"].asCString();
    char *token_value = (char *) root["ERC20"]["token_value"].asCString();

    char *abi = nullptr;
    JUB_RV rv = JUB_BuildERC20TransferAbiETH(contextID, token_to,
                                             token_value, &abi);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_BuildERC20TransferAbiETH: %08x", rv);
        env->ReleaseStringUTFChars(jJSON, pJSON);
        return NULL;
    }
    jstring rawString = env->NewStringUTF(abi);
    JUB_FreeMemory(abi);
    return rawString;
}


JNIEXPORT jstring JNICALL native_ETH_SignContract(JNIEnv *env, jclass obj, jlong contextID,
                                                  jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["ETH"]["bip32_path"]["change"].asBool();
    path.addressIndex = root["ETH"]["bip32_path"]["addressIndex"].asUInt();

    uint32_t nonce = static_cast<uint32_t>(root["ETH"]["nonce"].asDouble());
    uint32_t gasLimit = static_cast<uint32_t>(root["ETH"]["gasLimit"].asDouble());
    char *gasPriceInWei = (char *) root["ETH"]["gasPriceInWei"].asCString();
    char *valueInWei = nullptr;
    try {
        valueInWei = (char *) root["ETH"]["valueInWei"].asCString();
    } catch (...) {}

    char *to = nullptr;
    try {
        to = (char *) root["ETH"]["contract_address"].asCString();
    } catch (...) {}

    char *data = (char *) root["ETH"]["data"].asCString();


    char *raw = nullptr;
    JUB_RV rv = JUB_SignContractETH(contextID, path, nonce, gasLimit, gasPriceInWei, to, valueInWei,
                                    data, &raw);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}


JNIEXPORT jint JNICALL native_ETH_SetContrAddr(JNIEnv *env, jclass obj, jlong contextID,
                                               jstring jContrAddr) {

    JUB_CHAR_PTR pContrAddr = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jContrAddr, NULL));
    JUB_RV rv = JUB_SetContrAddrETH(contextID, pContrAddr);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetContrAddrETH rv: %08", rv);
    }
    return rv;
}


//JNIEXPORT jstring JNICALL native_ETH_BuildContractWithAddrAbi(JNIEnv *env, jobject obj, jlong contextID,
//                                                  jstring jJSON) {
//
//    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
//
//    Json::Reader reader;
//    Json::Value root;
//    reader.parse(pJSON, root);
//
//    char *methodID = (char *) root["methodID"].asCString();
//    char *address = (char *) root["address"].asCString();
//
//    char *abi = nullptr;
//    JUB_RV rv = JUB_BuildContractWithAddrAbiETH(contextID, methodID, address, &abi);
//
//    if (rv != JUBR_OK) {
//        errorCode = static_cast<int>(rv);
//        return NULL;
//    }
//
//    jstring rawString = env->NewStringUTF(abi);
//    JUB_FreeMemory(abi);
//    return rawString;
//}


JNIEXPORT jstring JNICALL
native_ETH_BuildContractWithAddrAmtDataAbi(JNIEnv *env, jclass obj, jlong contextID,
                                           jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    char *methodID = (char *) root["methodID"].asCString();
    char *address = (char *) root["address"].asCString();
    char *amount = (char *) root["amount"].asCString();
    char *data = (char *) root["data"].asCString();

    char *abi = nullptr;
    JUB_RV rv = JUB_BuildContractWithAddrAmtDataAbiETH(contextID, methodID, address, amount, data,
                                                       &abi);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    jstring rawString = env->NewStringUTF(abi);
    JUB_FreeMemory(abi);
    return rawString;
}


JNIEXPORT jstring JNICALL
native_ETH_BuildContractWithTxIDAbi(JNIEnv *env, jclass obj, jlong contextID,
                                    jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    char *methodID = (char *) root["methodID"].asCString();
    char *transactionID = (char *) root["transactionID"].asCString();

    char *abi = nullptr;
    JUB_RV rv = JUB_BuildContractWithTxIDAbiETH(contextID, methodID, transactionID, &abi);

    if (rv != JUBR_OK) {
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    jstring rawString = env->NewStringUTF(abi);
    JUB_FreeMemory(abi);
    return rawString;
}
//
//JNIEXPORT jstring JNICALL native_ETH_BuildContractWithAmtAbi(JNIEnv *env, jobject obj, jlong contextID,
//                                                              jstring jJSON) {
//
//    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));
//
//    Json::Reader reader;
//    Json::Value root;
//    reader.parse(pJSON, root);
//
//    char *methodID = (char *) root["methodID"].asCString();
//    char *amount = (char *) root["amount"].asCString();
//
//    char *abi = nullptr;
//    JUB_RV rv = JUB_BuildContractWithAmtAbiETH(contextID, methodID, amount, &abi);
//
//    if (rv != JUBR_OK) {
//        errorCode = static_cast<int>(rv);
//        return NULL;
//    }
//
//    jstring rawString = env->NewStringUTF(abi);
//    JUB_FreeMemory(abi);
//    return rawString;
//}


JNIEXPORT jobjectArray JNICALL
native_ETHGetAddress(JNIEnv *env, jclass obj, jlong contextID, jstring jJSON) {

#define BIP32_PATH   "bip32_path"
#define CHANGE       "change"
#define INDEX        "addressIndex"

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

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

        JUB_RV rv = JUB_GetHDNodeETH(contextID, XPUB, path, &xpub);
        if (rv != JUBR_OK) {
            LOG_ERR("JUB_GetHDNodeETH: %08x", rv);
            env->SetObjectArrayElement(array, 2 * i, NULL);
            env->SetObjectArrayElement(array, 2 * i + 1, NULL);
        } else {
            jstring jsXpub = env->NewStringUTF(xpub);
            JUB_CHAR_PTR pAddress = NULL;
            rv = JUB_GetAddressETH(contextID, path, BOOL_FALSE, &pAddress);
            if (rv != JUBR_OK) {
                LOG_ERR("JUB_GetAddressETH: %08x", rv);
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

JNIEXPORT jstring JNICALL
native_ETHShowAddress(JNIEnv *env, jclass obj, jlong contextID, jint index) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(BOOL_FALSE);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_GetAddressETH(static_cast<JUB_UINT16>(contextID), path, BOOL_TRUE, &pAddress);
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
native_SetMyAddressETH(JNIEnv *env, jclass obj, jlong contextID, jint change, jint index) {

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    BIP32_Path path;
    path.change = JUB_ENUM_BOOL(BOOL_FALSE);
    path.addressIndex = static_cast<JUB_UINT64>(index);

    JUB_CHAR_PTR pAddress = NULL;
    JUB_RV rv = JUB_SetMyAddressETH(static_cast<JUB_UINT16>(contextID), path, &pAddress);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetMyAddressETH: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }
    jstring address = env->NewStringUTF(pAddress);
    JUB_FreeMemory(pAddress);
    return address;
}

JNIEXPORT jstring JNICALL native_ETHGetMainHDNode(JNIEnv *env, jclass obj, jlong contextID) {
    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == NULL) {
        LOG_ERR("clazz == NULL");
        return NULL;
    }

    JUB_CHAR_PTR xpub;
    JUB_RV rv = JUB_GetMainHDNodeETH(contextID, XPUB, &xpub);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_GetMainHDNodeETH: %08x", rv);
        return NULL;
    }
    jstring mainPub = env->NewStringUTF(xpub);
    JUB_FreeMemory(xpub);
    return mainPub;
}


JNIEXPORT jstring JNICALL native_ETH_SignBytestring(JNIEnv *env, jclass obj, jlong contextID,
                                                    jstring jJSON) {

    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["Bytestring"]["bip32_path"]["change"].asBool();
    path.addressIndex = root["Bytestring"]["bip32_path"]["addressIndex"].asUInt();

    char *data = (char *) root["Bytestring"]["data"].asCString();

    char *raw = nullptr;
    JUB_RV rv = JUB_SignBytestringETH(contextID, path, data, &raw);

    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SignBytestringETH: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}


JNIEXPORT jstring JNICALL native_ETH_SignTypedData(JNIEnv *env, jclass obj, jlong contextID,
                                                   jstring jJSON) {
    JUB_CHAR_PTR pJSON = const_cast<JUB_CHAR_PTR>(env->GetStringUTFChars(jJSON, NULL));

    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    bool enableV4 = root["EIP712"]["metamask_v4_compat"].asBool();

    BIP32_Path path;
    path.change = (JUB_ENUM_BOOL) root["EIP712"]["bip32_path"]["change"].asBool();
    path.addressIndex = root["EIP712"]["bip32_path"]["addressIndex"].asUInt();

    char *data = (char *) root["EIP712"]["data"].asCString();

    char *raw = nullptr;
    JUB_RV rv = JUB_SignTypedDataETH(contextID, path, data, enableV4, &raw);

    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SignTypedDataETH: %08x", rv);
        errorCode = static_cast<int>(rv);
        return NULL;
    }

    jstring rawString = env->NewStringUTF(raw);
    JUB_FreeMemory(raw);
    return rawString;
}

JNIEXPORT jint JNICALL
native_ETH_UploadNFT(JNIEnv *env, jclass obj, jlong contextID, jstring jMeta, jbyteArray image,
                     jbyteArray thumbnail, jbyteArray wallpaper) {
    auto pJSON = const_cast<JUB_CHAR_CPTR>(env->GetStringUTFChars(jMeta, JNI_FALSE));
    Json::Reader reader;
    Json::Value root;
    reader.parse(pJSON, root);

    const auto* id = root["id"].asCString();
    const auto* name = root["name"].asCString();
    const auto* token = root["token"].asCString();
    const auto* network = root["network"].asCString();
    const auto* owner = root["owner"].asCString();
    const auto* ext = root["extension"].asCString();
    auto* image_payload = (JUB_BYTE_CPTR )(env->GetByteArrayElements(image, JNI_FALSE));
    JUB_UINT32 image_payload_size = env->GetArrayLength(image);
    auto* thumbnail_payload = (JUB_BYTE_CPTR )(env->GetByteArrayElements(thumbnail, JNI_FALSE));
    JUB_UINT32 thumbnail_size = env->GetArrayLength(thumbnail);
    auto* wallpaper_payload = (JUB_BYTE_CPTR )(env->GetByteArrayElements(wallpaper, JNI_FALSE));
    JUB_UINT32 wallpaper_payload_size = env->GetArrayLength(wallpaper);

    auto nft = JUB_ETH_NFT_INFO {
        .id = id,
        .name = name,
        .token = token,
        .network = network,
        .owner = owner,
        .extension = ext,
        .image = {
                .payload = image_payload,
                .size = image_payload_size,
        },
        .thumbnail = {
                .payload = thumbnail_payload,
                .size = thumbnail_size,
        },
        .wallpaper = {
                .payload = wallpaper_payload,
                .size = wallpaper_payload_size
        }
    };

    auto rv = JUB_UploadNFT(contextID, nft);
    if (rv != JUBR_OK) {
        LOG_ERR("JUB_SetERC20TokenETHV2: %08x", rv);
    }
    env->ReleaseStringUTFChars(jMeta, pJSON);
    return JUBR_OK;
}


#ifdef __cplusplus
}
#endif



// 定义 JNINativeMethod 数组
static JNINativeMethod g2EthMethods[] = {
        {
                "nativeETHCreateContext",
                "([ILjava/lang/String;J)I",
                (void *) native_ETHCreateContext
        },
        {
                "nativeETHTransaction",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_Transaction
        },
        {
                "nativeETHSetERC20Tokens",
                "(JLjava/lang/String;)I",
                (void *) native_ETH_SetERC20Tokens
        },
        {
                "nativeETHSetERC20Token",
                "(JLjava/lang/String;)I",
                (void *) native_ETH_SetERC20Token
        },
        {
                "nativeETHSetERC20TokenV2",
                "(JLjava/lang/String;)I",
                (void *) native_ETH_SetERC20TokenV2
        },
        {
                "nativeETHSetNetworkV2",
                "(JLjava/lang/String;)I",
                (void *) native_ETH_SetNetworkV2
        },
        {
                "nativeETHBuildERC20TransferAbi",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_BuildERC20TransferAbi
        },
        {
                "nativeETHSetContrAddr",
                "(JLjava/lang/String;)I",
                (void *) native_ETH_SetContrAddr
        },
        {
                "nativeETHShowAddress",
                "(JI)Ljava/lang/String;",
                (void *) native_ETHShowAddress
        },
        {
                "nativeETHGetAddress",
                "(JLjava/lang/String;)[Ljava/lang/String;",
                (void *) native_ETHGetAddress
        },
        {
                "nativeETHSetMyAddress",
                "(JII)Ljava/lang/String;",
                (void *) native_SetMyAddressETH
        },
        {
                "nativeETHGetMainHDNode",
                "(J)Ljava/lang/String;",
                (void *) native_ETHGetMainHDNode
        },
        {
                "nativeETHSignContract",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_SignContract
        },
//        {
//                "nativeETHBuildContractWithAddrAbi",
//                "(JLjava/lang/String;)Ljava/lang/String;",
//                (void *) native_ETH_BuildContractWithAddrAbi
//        },
        {
                "nativeETHBuildContractWithAddrAmtDataAbi",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_BuildContractWithAddrAmtDataAbi
        },
        {
                "nativeETHBuildContractWithTxIDAbi",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_BuildContractWithTxIDAbi
        },
        {
                "nativeETHTypeDataMAX",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_SignTypedDataMAX
        },
        //        {
//                "nativeETHBuildContractWithAmtAbi",
//                "(JLjava/lang/String;)Ljava/lang/String;",
//                (void *) native_ETH_BuildContractWithAmtAbi
//        },
        {
                "nativeETHSignBytestring",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_SignBytestring
        },
        {
                "nativeETHSignTypedData",
                "(JLjava/lang/String;)Ljava/lang/String;",
                (void *) native_ETH_SignTypedData
        },
        {
            "nativeETHUploadNFT",
            "(JLjava/lang/String;[B[B[B)I",
            (void*) native_ETH_UploadNFT,
        }
};



#define G2_ETH_CLASS "com/legendwd/hyperpay/onchain/hardware/jubnative/NativeApiLL"

jint RegisterG2EthNatives(JNIEnv* env) {
    LOG_INF("Register G2 ETH");
    REGISTER_NATIVES(env, G2_ETH_CLASS, g2EthMethods, sizeof(g2EthMethods) / sizeof(JNINativeMethod));
    return JNI_OK;
}

static JniModuleRegistrar kRegG2Eth(RegisterG2EthNatives);