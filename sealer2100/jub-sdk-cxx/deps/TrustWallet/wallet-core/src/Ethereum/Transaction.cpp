// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Transaction.h"
#include "Address.h"
#include "Data.h"
#include "EIP712.h"
#include "HexCoding.h"
#include "RLP.h"
#include "../Hash.h"
#include <string>
#include <vector>

namespace TW::Ethereum {
static const Data EmptyListEncoded = parse_hex("c0");

// JuBiter-defined
Data Signature::serialize() const {
    Data encoded;
    if (1 == v.size() && 0x80 == v[0]) {
        append(encoded, 0x80);
    } else {
        append(encoded, RLP::encode(v));
    }
    append(encoded, RLP::encode(r));
    append(encoded, RLP::encode(s));
    return encoded;
}

// JuBiter-modified
Data TransactionBase::preHash(const Data &chainID) const {
    auto preimage = serialize(chainID);
    return (0 == preimage.size() ? Data() : Hash::keccak256(preimage));
}

// JuBiter-defined
Data TransactionBase::encoded(const Signature &signature) const {
    if (signature.empty()) {
        return {};
    }

    Data vSignature;
    std::copy(std::begin(signature.r), std::end(signature.r), std::back_inserter(vSignature));
    std::copy(std::begin(signature.s), std::end(signature.s), std::back_inserter(vSignature));
    std::copy(std::begin(signature.v), std::end(signature.v), std::back_inserter(vSignature));
    return vSignature;
}

// JuBiter-modified
Data TransactionBase::encoded(const Signature &signature, const Data &chainID) const { return encoded(signature); }

// JuBiter-defined
bool TransactionBase::decoded(const Data &encoded, Signature &signature) {
    if (0 == encoded.size()) {
        return false;
    }

    std::copy(std::begin(encoded), std::begin(encoded) + 32, std::back_inserter(signature.r));
    std::copy(std::begin(encoded) + 32, std::begin(encoded) + 64, std::back_inserter(signature.s));
    std::copy(std::begin(encoded) + 64, std::begin(encoded) + 65, std::back_inserter(signature.v));
    return true;
}

// JuBiter-defined
Data TransactionPersonal::preHash() const { return TransactionBase::preHash({}); }

// JuBiter-defined
Data TransactionPersonal::serialize(const Data &chainID) const { return serialize(); }

// JuBiter-defined
Data TransactionPersonal::serialize() const {
    auto encoded = Data();

    // encode(b : 𝔹⁸ⁿ) = "\x19Ethereum Signed Message:\n" ‖ len(b) ‖ b where len(b) is the ascii-decimal encoding of the
    // number of bytes in b.
    std::string pr = "Ethereum Signed Message:\n";

    encoded.push_back(0x19);
    std::copy(pr.begin(), pr.end(), std::back_inserter(encoded));
    auto sz = std::to_string(payload.size());
    std::copy(sz.begin(), sz.end(), std::back_inserter(encoded));
    std::copy(payload.begin(), payload.end(), std::back_inserter(encoded));

    return encoded;
}

// JuBiter-defined
bool TransactionPersonal::decoded(const Data &encoded, Signature &signature) { return false; }

// JuBiter-defined
Data TransactionTypedData::preHash() const { return TransactionBase::preHash({}); }

// JuBiter-defined
Data TransactionTypedData::serialize(const Data &chainID) const { return serialize(); }

// JuBiter-defined
Data TransactionTypedData::serialize() const {
    auto encoded = Data();
    encoded.push_back(0x19);
    encoded.push_back(0x01);

    std::string typedDataInJSON(payload.begin(), payload.end());
    nlohmann::json typedData = nlohmann::json::parse(typedDataInJSON);
    if (nlohmann::detail::value_t::object != typedData.type()) {
        return {};
    }

    if (!jub::eth::EIP712::parseJSON(typedData)) {
        jub::eth::EIP712::clearJSON();
        return {};
    }

    Data domainSeparator = jub::eth::EIP712::typed_data_envelope(
                            jub::eth::EIP712::EIP712Domain(),
                    typedData[jub::eth::EIP712::domainEnter()],
                    bMetamaskV4Compat);
    if (domainSeparator.empty()) {
        jub::eth::EIP712::clearJSON();
        return {};
    }

    Data hashStructMessage = jub::eth::EIP712::typed_data_envelope(
                    typedData[jub::eth::EIP712::primaryTypeEnter()].get<std::string>().c_str(),
                    typedData[jub::eth::EIP712::messageEnter()],
                    bMetamaskV4Compat);
    jub::eth::EIP712::clearJSON();
    if (hashStructMessage.empty()) {
        return {};
    }

    std::copy(domainSeparator.begin(), domainSeparator.end(), std::back_inserter(encoded));
    std::copy(hashStructMessage.begin(), hashStructMessage.end(), std::back_inserter(encoded));

    return encoded;
}

// JuBiter-defined
bool TransactionTypedData::decoded(const Data &encoded, Signature &signature) { return false; }
} // namespace TW::Ethereum
