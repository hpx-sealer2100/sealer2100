// Copyright © 2017-2022 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Address.h"
#include <string>
#include <vector>
#include <Data.h>

//#include "../uint256.h"

namespace TW::Ethereum {

/// R-S-V Signature values
struct Signature {
  public:
    //    uint256_t r, s, v;
    Data r, s, v;

    // JuBiter-defined
    bool empty() const { return (r.empty() && s.empty() && v.empty()); }

    // JuBiter-defined
    Data serialize() const;

    // JuBiter-defined
    bool isValid() const {
        if (0 == r.size() || 0 == s.size() || 0 == v.size()) {
            return false;
        }

        return true;
    }
}; // struct Signature end

/// Base class for all transactions.
/// Non-typed and various typed transactions derive from this.
class TransactionBase {
  public:
    //    uint256_t nonce;
    Data nonce;
    Data payload;

  public:
    //    TransactionBase(const uint256_t& nonce, const Data& payload): nonce(nonce), payload(payload) {}
    TransactionBase(const Data &nonce, const Data &payload) : nonce(nonce), payload(payload) {}
    virtual ~TransactionBase() {
        nonce.clear();
        payload.clear();
    }
    // JuBiter-modified
    // pre-sign hash of the tx, for signing
    //    virtual Data preHash(const uint256_t chainID) const = 0;
    virtual Data preHash(const Data &chainID) const;
    // pre-sign image of tx
    //    virtual Data serialize(const uint256_t chainID) const = 0;
    virtual Data serialize(const Data &chainID) const = 0;
    // JuBiter-defined
    virtual Data encoded(const Signature &signature) const;

  protected:
    // JuBiter-modified
    // encoded tx (signed)
    //    virtual Data encoded(const Signature& signature, const uint256_t chainID) const = 0;
    virtual Data encoded(const Signature &signature, const Data &chainID) const;

  public:
    // JuBiter-defined
    virtual bool decoded(const Data &encoded, Signature &signature);
    // Signals wether this tx type uses Eip155-style replay protection in the signature
    virtual bool usesReplayProtection() const = 0;

  protected:
    TransactionBase() {}
}; // class TransactionBase end

// JuBiter-defined
/// Original transaction format, with no explicit type, legacy as pre-EIP191
class TransactionPersonal : public TransactionBase {
  public:
    // Reuse base class payload as data to be signed (byteString)
    TransactionPersonal(const Data &bytestring) : TransactionBase({}, bytestring) {}

    virtual Data preHash() const;

  protected:
    virtual Data serialize(const Data &chainID) const override;

  public:
    virtual Data serialize() const;
    virtual bool decoded(const Data &encoded, Signature &signature) override;
    virtual bool usesReplayProtection() const override { return true; }
}; // class TransactionPersonal end

// JuBiter-defined
/// Original transaction format, with no explicit type, legacy as pre-EIP712
class TransactionTypedData : public TransactionBase {
  public:
    bool bMetamaskV4Compat;
    // Reuse base class payload as data in JSON to be signed (JSON data)
    TransactionTypedData(const std::string &dataInJSON, const bool bMetamaskV4Compat = true)
        : bMetamaskV4Compat(bMetamaskV4Compat)
        , TransactionBase({}, Data(dataInJSON.begin(), dataInJSON.end())) {}

    virtual Data preHash() const;

  protected:
    virtual Data serialize(const Data &chainID) const override;

  public:
    virtual Data serialize() const;
    virtual bool decoded(const Data &encoded, Signature &signature) override;
    virtual bool usesReplayProtection() const override { return true; }
}; // class TransactionTypedData end
} // namespace TW::Ethereum
