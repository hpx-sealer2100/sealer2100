#include "token/JubiterBLDImpl.h"
#include "../HexCoding.h"
#include "TrustWallet/wallet-core/src/BinaryCoding.h"
#include <Solana/Transaction.h>
#include <Solana/Program.h>

namespace jub {


#define SWITCH_TO_SOL_APP                       \
do {                                            \
    JUB_VERIFY_RV(_SelectApp(kPKIAID_MISC, sizeof(kPKIAID_MISC)/sizeof(JUB_BYTE)));\
} while (0);                                    \


enum SOL_APDU_TAG : uint8_t {
    TAG_PATH = 0x08,
    TAG_TO   = 0x41,
    TAG_AMOUNT,
    TAG_RECENT_HASH,
    TAG_SOURCE,
    TAG_TOKEN_MINT,
    TAG_NEWER_TOKEN_ADDRESS,
    TAG_TOKEN_ADDRESS_OWNER,
    TAG_BUMP
};


enum SOL_PROGRAM_P1 : uint8_t { P1_SYS_PROGRAM = 0x00, P1_SPL_TOKEN, P1_ASSOCIATED_TOKEN };


/// enum tag number in `instruction`
namespace sintruction_p2 {
static uint8_t SYS_INS_TRANSFER         = 0x02;
static uint8_t SPL_INS_TRANSFER         = 0x03;
static uint8_t SPL_INS_TRANSFER_CHECKED = 0x0C;
static uint8_t ASSOCIATED_INS_CREATE    = 0x00;
} // namespace sintruction_p2


JUB_RV JubiterBLDImpl::SelectAppletSOL() {

    SWITCH_TO_SOL_APP;
    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::SetCoinTypeSOL() {

    return _SetCoinType((JUB_BYTE)JUB_ENUM_COINTYPE_MISC::COINSOL);
}


JUB_RV JubiterBLDImpl::GetAddressSOL(const std::string& path, const JUB_UINT16 tag, std::string& address) {

    uchar_vector data(path.begin(), path.end());

    APDU apdu(0x00, 0xf6, 0x00, (JUB_BYTE)tag, (JUB_ULONG)data.size(), data.data(), 0x14);
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    uchar_vector vAddress(retData, (unsigned int)ulRetDataLen);
    address = std::string(vAddress.begin(), vAddress.end());

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::GetHDNodeSOL(const JUB_BYTE format, const std::string& path, std::string& pubkey) {

    //path = "m/44'/501'/0'";
    uchar_vector vPath;
    vPath << path;
    uchar_vector apduData = ToTlv(0x08, vPath);

    APDU apdu(0x00, 0xe6, 0x00, format, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    if (JUB_ENUM_PUB_FORMAT::HEX == format) {
        uchar_vector vPubkey(retData, (unsigned int)ulRetDataLen);
        pubkey = vPubkey.getHex();
    }
    else if (JUB_ENUM_PUB_FORMAT::XPUB == format) {
        pubkey = (JUB_CHAR_PTR)retData;
    }

    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::SetTokenInfo(const std::string &name, JUB_UINT8 decimal, const std::string &tokenMint) {
    uchar_vector lvName  = Tollv(name);
    uchar_vector address = TW::Solana::Address{tokenMint}.vector();

    uchar_vector apduData;
    apduData << (uint8_t)decimal;
    apduData << (uint8_t)lvName.size();
    apduData << lvName;
    apduData << (uint8_t)address.size();
    apduData << address;

    APDU apdu(0x00, 0xC7, 0x00, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));

    return JUBR_OK;
}
JUB_RV JubiterBLDImpl::SignTransferTxSOl_2100(
        const std::string &path,
        const JUB_CHAR_CPTR recentHash,
        const JUB_CHAR_CPTR dest,
        JUB_UINT64 amount,
        std::string &raw){

    return JUBR_IMPL_NOT_SUPPORT;
}
// Trasfer `Token`
JUB_RV JubiterBLDImpl::SignTokenTransferTxSOL_2100(const std::string &path,
                                                   const JUB_CHAR_CPTR recentHash,
                                                   const JUB_CHAR_CPTR token,
                                                   const JUB_CHAR_CPTR &from,
                                                   const JUB_CHAR_CPTR &dest,
                                                   JUB_UINT64 amount,
                                                   JUB_UINT8 decimal,
                                                   std::string &raw) {
    return JUBR_IMPL_NOT_SUPPORT;
}
JUB_RV JubiterBLDImpl::SignTokenCreateAndTransferTxSOL_2100(const std::string &path,
                                                            const JUB_CHAR_CPTR recentHash,
                                                            const JUB_CHAR_CPTR token,
                                                            const JUB_CHAR_CPTR destMainAddress,
                                                            const JUB_CHAR_CPTR &from,
                                                            const JUB_CHAR_CPTR &dest,
                                                            JUB_UINT64 amount,
                                                            JUB_UINT8 decimal,
                                                            std::string &raw) {
    return JUBR_IMPL_NOT_SUPPORT;
}
// Trasfer `SOL`
JUB_RV JubiterBLDImpl::SignTransferTx(const std::string &path, const std::vector<JUB_BYTE> &recentHash,
                                      const std::vector<JUB_BYTE> &dest, JUB_UINT64 amount, std::vector<JUB_BYTE> &raw) {
    auto data = uchar_vector{};
    using TAG = SOL_APDU_TAG;
    // path
    data << ToTlv(TAG::TAG_PATH, {path});
    // to
    data << ToTlv(TAG::TAG_TO, dest);
    // recent hash
    data << ToTlv(TAG::TAG_RECENT_HASH, recentHash);
    // amount
    data << ToTlv(TAG::TAG_AMOUNT, TW::encode64LE(amount));

    // can send by one pack
    auto apdu = APDU{0x00, 0xF8, 0x00, 0x00, data.size(), data.data()};
    JUB_UINT16 ret;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (ret != 0x9000) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    using P1     = SOL_PROGRAM_P1;
    namespace P2 = sintruction_p2;

    apdu = APDU{0x00, 0x2A, P1::P1_SYS_PROGRAM, P2::SYS_INS_TRANSFER, 0};

    auto resp    = std::array<JUB_BYTE, 2>{};
    auto respLen = JUB_ULONG{2};
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, resp.data(), &respLen));
    if (0x9000 != ret) {
        JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
    }

    // retrieve transaction
    auto txLen = TW::decode16BE(resp.data());
    auto tx    = TW::Data(txLen);
    respLen    = txLen;

    apdu = APDU{0x00, 0xF9, 0x00, 0x00, 0x00, nullptr, txLen};
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, tx.data(), &respLen));
    if (ret != 0x9000) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }
    raw = tx;

    return JUBR_OK;
}


// Trasfer `Token`
JUB_RV JubiterBLDImpl::SignTokenTransferTx(const std::string &path, const std::vector<JUB_BYTE> &recentHash,
                                           const std::vector<JUB_BYTE> token, const std::vector<JUB_BYTE> &from,
                                           const std::vector<JUB_BYTE> &dest, JUB_UINT64 amount, JUB_UINT8 decimal,
                                           std::vector<JUB_BYTE> &raw) {
    auto data = uchar_vector{};
    using TAG = SOL_APDU_TAG;
    // path: signer/delegate
    data << ToTlv(TAG::TAG_PATH, {path});
    // source
    data << ToTlv(TAG::TAG_SOURCE, from);
    // to
    data << ToTlv(TAG::TAG_TO, dest);
    // token mint
    data << ToTlv(TAG::TAG_TOKEN_MINT, token);
    // recent hash
    data << ToTlv(TAG::TAG_RECENT_HASH, recentHash);
    // amount
    data << ToTlv(TAG::TAG_AMOUNT, TW::encode64LE(amount));
    // decimal
    (void)(decimal);

    // can send by one pack
    auto apdu = APDU{0x00, 0xF8, 0x00, 0x00, data.size(), data.data()};
    JUB_UINT16 ret;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (ret != 0x9000) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    using P1     = SOL_PROGRAM_P1;
    namespace P2 = sintruction_p2;

    apdu = APDU{0x00, 0x2A, P1::P1_SPL_TOKEN, P2::SPL_INS_TRANSFER_CHECKED, 0};

    auto resp    = std::array<JUB_BYTE, 2>{};
    auto respLen = JUB_ULONG{2};
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, resp.data(), &respLen));
    if (0x9000 != ret) {
        JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
    }

    // retrieve transaction
    auto txLen  = TW::decode16BE(resp.data());
    auto tx     = TW::Data(txLen);
    auto p      = tx.data();
    auto offset = JUB_UINT16{0};

    constexpr JUB_ULONG kSendOnceLen = 230;
    while (txLen != 0) {
        auto pack = std::min((JUB_ULONG)txLen, kSendOnceLen);

        apdu = APDU{0x00, 0xF9, (JUB_ULONG)(offset >> 8), (JUB_ULONG)(offset & 0xff), 0x00, nullptr, pack};
        JUB_VERIFY_RV(_SendApdu(&apdu, ret, p, &pack));
        if (ret != 0x9000) {
            return JUBR_TRANSMIT_DEVICE_ERROR;
        }
        p += pack;
        offset += pack;
        txLen -= pack;
    }

    raw = tx;

    return JUBR_OK;
}


// create token address
JUB_RV JubiterBLDImpl::SignCreateTokenAccountTx(const std::string &path, const std::vector<JUB_BYTE> &recentHash,
                                                const std::vector<JUB_BYTE> &owner, const std::vector<JUB_BYTE> &token,
                                                std::vector<JUB_BYTE> &raw) {
    using namespace TW::Solana;
    // we need address and index, see `TokenProgram::defaultTokenAddress`
    auto programId = Address(TOKEN_PROGRAM_ID_ADDRESS);

    auto seeds = std::vector<TW::Data>{owner, programId.vector(), token};

    // auto [newer, index] = TokenProgram::findProgramAddress(seeds, Address(ASSOCIATED_TOKEN_PROGRAM_ID_ADDRESS));
    auto generatedTokenAddress = TokenProgram::findProgramAddress(seeds, Address(ASSOCIATED_TOKEN_PROGRAM_ID_ADDRESS));
    auto newer = std::get<0>(generatedTokenAddress);
    auto index = std::get<1>(generatedTokenAddress);

    auto data = uchar_vector{};
    using TAG = SOL_APDU_TAG;
    // path: signer
    data << ToTlv(TAG::TAG_PATH, {path});
    // new address to be created
    data << ToTlv(TAG::TAG_NEWER_TOKEN_ADDRESS, newer.vector());
    // owner
    data << ToTlv(TAG::TAG_TOKEN_ADDRESS_OWNER, owner);
    // token mint
    data << ToTlv(TAG::TAG_TOKEN_MINT, token);
    // recent hash
    data << ToTlv(TAG::TAG_RECENT_HASH, recentHash);
    // bump
    data << ToTlv(TAG::TAG_BUMP, TW::Data({index}));

    // can send by one pack
    auto apdu = APDU{0x00, 0xF8, 0x00, 0x00, data.size(), data.data()};
    JUB_UINT16 ret;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (ret != 0x9000) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    using P1     = SOL_PROGRAM_P1;
    namespace P2 = sintruction_p2;

    apdu = APDU{0x00, 0x2A, P1::P1_ASSOCIATED_TOKEN, P2::ASSOCIATED_INS_CREATE, 0};

    auto resp    = std::array<JUB_BYTE, 2>{};
    auto respLen = JUB_ULONG{2};

    // sign tx
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, resp.data(), &respLen));
    if (0x9000 != ret) {
        JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
    }

    // retrieve transaction
    auto txLen  = TW::decode16BE(resp.data());
    auto tx     = TW::Data(txLen);
    auto p      = tx.data();
    auto offset = JUB_UINT16{0};

    constexpr JUB_ULONG kSendOnceLen = 230;
    while (txLen != 0) {
        auto pack = std::min((JUB_ULONG)txLen, kSendOnceLen);

        apdu = APDU{0x00, 0xF9, (JUB_ULONG)(offset >> 8), (JUB_ULONG)(offset & 0xff), 0x00, nullptr, pack};
        JUB_VERIFY_RV(_SendApdu(&apdu, ret, p, &pack));
        if (ret != 0x9000) {
            return JUBR_TRANSMIT_DEVICE_ERROR;
        }
        p += pack;
        offset += pack;
        txLen -= pack;
    }

    raw = tx;
    return JUBR_OK;
}


JUB_RV JubiterBLDImpl::GetAddressSOLEncodePb(
    const std::string& path,
    const JUB_UINT16 tag,
    std::string& msgInPb
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::GetAddressSOLDecodePb(
    const std::string& msgInPb,
    std::string& address
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::GetHDNodeSOLEncodePb(
    const JUB_BYTE format,
    const std::string& path,
    std::string& msgInPb
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::GetHDNodeSOLDecodePb(
    const std::string& msgInPb,
    std::string& address
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::SignTransferTxEncodePb(
    const std::string &path,
    const std::vector<JUB_BYTE> &recentHash,
    const std::vector<JUB_BYTE> &dest,
    JUB_UINT64 amount,
    std::vector<JUB_BYTE> &vMsgInPb
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::SignTransferTxDecodePb(
    const std::string& msgInPb,
    std::string& address
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::SignTokenTransferTxEncodePb(
    const std::string &path,
    const std::vector<JUB_BYTE> &recentHash,
    const std::vector<JUB_BYTE> token,
    const std::vector<JUB_BYTE> &from,
    const std::vector<JUB_BYTE> &dest,
    JUB_UINT64 amount,
    JUB_UINT8 decimal,
    std::vector<JUB_BYTE> &vMsgInPb
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::SignTokenTransferTxDecodePb(
    const std::string& msgInPb,
    std::string& address
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::SignCreateTokenAccountTxEncodePb(
    const std::string &path,
    const std::vector<JUB_BYTE> &recentHash,
    const std::vector<JUB_BYTE> &owner,
    const std::vector<JUB_BYTE> &token,
    std::vector<JUB_BYTE> &vMsgInPb
) {
    return JUBR_IMPL_NOT_SUPPORT;
}


JUB_RV JubiterBLDImpl::SignCreateTokenAccountTxDecodePb(
    const std::string& msgInPb,
    std::string& address
) {
    return JUBR_IMPL_NOT_SUPPORT;
}
} // namespace jub end
