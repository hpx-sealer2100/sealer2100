////
////  JubiterSealer2100ImplAPTOS.cpp
////  JubSDK
////
////  Created by Pan Min on 2025/5/5.
////  Copyright © 2025 ZianCube. All rights reserved.
////
//
//#include "token/JubiterSealer2100Impl.h"
//
//#include "nanopb/pb.h"
//#include "nanopb/pb_common.h"
//#include "nanopb/pb_encode.h"
//#include "common/common/protob/messages-aptos.pb.h"
//
//namespace jub {
//
//JUB_RV JubiterSealer2100Impl::SelectAppletAPTOS() {
//    return JUBR_OK;
//}
//
//
//JUB_RV JubiterSealer2100Impl::SetCoinTypeAPTOS() {
//    return JUBR_OK;
//}
//
//
//JUB_RV JubiterSealer2100Impl::GetAddressAPTOSEncodePb(
//    const std::string& path,
//    const JUB_UINT16 tag,
//    std::string& msgInPb
//) {
//    try {
//        uchar_vector path_n(path.begin(), path.end());
//
//        AptosGetAddress getAddressMsg = AptosGetAddress_init_zero;
//        if (path_n.size() > sizeof(getAddressMsg.address_n)
//        ) {
//            return JUBR_HOST_MEMORY;
//        }
//
//        memcpy(getAddressMsg.address_n, path_n.data(), path_n.size());
//        getAddressMsg.address_n_count = path_n.size();
//        getAddressMsg.has_show_display = false;
//        getAddressMsg.show_display = false;
//
//        // Encode the message
//        size_t encoded_size = 0;
//        if (!pb_get_encoded_size(&encoded_size, AptosGetAddress_fields, &getAddressMsg)) {
//            return JUBR_ERROR;
//        }
//
//        uint8_t *encoded = new uint8_t[encoded_size];
//        if (nullptr == encoded) {
//            return JUBR_MEMORY_NULL_PTR;
//        }
//        pb_ostream_t stream = pb_ostream_from_buffer(encoded, encoded_size);
//
//        bool status = pb_encode(&stream, AptosGetAddress_fields, &getAddressMsg);
//        if (!status) {
//            // Handle encoding error
//            printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
//            delete [] encoded; encoded = nullptr;
//            return JUBR_ARGUMENTS_BAD;
//        }
//
//        msgInPb.assign(reinterpret_cast<const char*>(encoded), stream.bytes_written);
//        delete [] encoded; encoded = nullptr;
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_OK;
//}
//
//
//JUB_RV JubiterSealer2100Impl::GetAddressAPTOSDecodePb(
//    const std::string& msgInPb,
//    std::string& address
//) {
//    // TODO
//    try {
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV JubiterSealer2100Impl::GetHDNodeAPTOSEncodePb(
//    const JUB_BYTE format,
//    const std::string& path,
//    std::string& msgInPb
//) {
//    try {
//        uchar_vector path_n(path.begin(), path.end());
//
//        AptosGetPublicKey getPublicKey = AptosGetPublicKey_init_zero;
//        if (path_n.size() > sizeof(getPublicKey.address_n)
//        ) {
//            return JUBR_HOST_MEMORY;
//        }
//
//        memcpy(getPublicKey.address_n, path_n.data(), path_n.size());
//        getPublicKey.address_n_count = path_n.size();
//        getPublicKey.has_show_display = false;
//        getPublicKey.show_display = false;
//
//        // Encode the message
//        size_t encoded_size = 0;
//        if (!pb_get_encoded_size(&encoded_size, AptosGetPublicKey_fields, &getPublicKey)) {
//            return JUBR_ERROR;
//        }
//
//        uint8_t *encoded = new uint8_t[encoded_size];
//        if (nullptr == encoded) {
//            return JUBR_MEMORY_NULL_PTR;
//        }
//        pb_ostream_t stream = pb_ostream_from_buffer(encoded, encoded_size);
//
//        bool status = pb_encode(&stream, AptosGetPublicKey_fields, &getPublicKey);
//        if (!status) {
//            // Handle encoding error
//            printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
//            delete [] encoded; encoded = nullptr;
//            return JUBR_ARGUMENTS_BAD;
//        }
//
//        msgInPb.assign(reinterpret_cast<const char*>(encoded), stream.bytes_written);
//        delete [] encoded; encoded = nullptr;
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_OK;
//}
//
//
//JUB_RV JubiterSealer2100Impl::GetHDNodeAPTOSDecodePb(
//    const std::string& msgInPb,
//    std::string& address
//) {
//    // TODO
//    try {
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV JubiterSealer2100Impl::SignTXAPTOSEncodePb(
//    const std::vector<JUB_BYTE>& vPath,
//    const std::vector<JUB_BYTE>& vRaw,
//    std::vector<uchar_vector>& vMsgInPb
//) {
//    try {
//        AptosSignTx signTx = AptosSignTx_init_zero;
//        if ((vPath.size() > sizeof(signTx.address_n))
//        ) {
//            return JUBR_HOST_MEMORY;
//        }
//
//        memcpy(signTx.address_n, vPath.data(), vPath.size());
//        signTx.address_n_count = vPath.size();
//
//        // TODO
//
//        // Encode the message
//        size_t encoded_size = 0;
//        if (!pb_get_encoded_size(&encoded_size, AptosSignTx_fields, &signTx)) {
//            return JUBR_ERROR;
//        }
//
//        uint8_t *encoded = new uint8_t[encoded_size];
//        if (nullptr == encoded) {
//            return JUBR_MEMORY_NULL_PTR;
//        }
//        pb_ostream_t stream = pb_ostream_from_buffer(encoded, encoded_size);
//
//        bool status = pb_encode(&stream, AptosSignTx_fields, &signTx);
//        if (!status) {
//            // Handle encoding error
//            printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
//            delete [] encoded; encoded = nullptr;
//            return JUBR_ARGUMENTS_BAD;
//        }
//
//        delete [] encoded; encoded = nullptr;
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    // TODO
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//
//
//JUB_RV JubiterSealer2100Impl::SignTXAPTOSDecodePb(
//    const std::string& msgInPb,
//    std::string& address
//) {
//    // TODO
//    try {
//    }
//    catch (...) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    return JUBR_IMPL_NOT_SUPPORT;
//}
//} // namespace jub end
