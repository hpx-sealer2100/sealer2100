// Copyright © 2017-2020 JuBiter.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Contract.hpp"
#include "balance_contract.pb.h"


namespace TW::Tron {


//message TransferContract {
//    // Sender address.
//    string owner_address = 1;
//
//    // Recipient address.
//    string to_address = 2;
//
//    // Amount to send.
//    int64 amount = 3;
//}
class TransferContract : public Contract {

public:
    TransferContract() {}
    TransferContract(std::string owner_address,
                     std::string to_address,
                     int64_t amount)
        : Contract(owner_address)
        , to_address(to_address)
        , amount(amount)
    {}
    virtual ~TransferContract() {
        clear();
    }

    void clear() {
        Contract::clear();
        to_address = "";
        amount = 0;
    }

    void from_internal(const ::protocol::TransferContract& contract);
    ::protocol::TransferContract to_internal() const;

    virtual bool calculateOffset() override;

    virtual TW::Data toAddressSize() const;
    virtual TW::Data toAddressOffset(const size_t offset) const;

    virtual TW::Data amountSize() const;
    virtual TW::Data amountOffset(const size_t offset) const;

protected:
    virtual pb_length_delimited getToAddress() const;
    virtual pb_varint getAmount() const;

    virtual size_t toAddressIndex(const size_t offset) const;
    virtual size_t    amountIndex(const size_t offset) const;

    size_t toAddrSize = 0;
    size_t toAddrIndex = 0;

    size_t amtSize = 0;
    size_t amtIndex = 0;

public:
    // Recipient address.
    std::string to_address;
    // Amount to send.
    int64_t amount;
};  // class TransferContract end


} // namespace TW::Tron
