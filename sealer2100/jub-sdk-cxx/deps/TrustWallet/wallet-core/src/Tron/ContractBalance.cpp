// Copyright © 2017-2020 JuBiter.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "ContractBalance.h"

#include "../HexCoding.h"


namespace TW::Tron {


void TransferContract::from_internal(const ::protocol::TransferContract& contract) {

    Contract::from_internal(contract);

    owner_address = TW::Tron::Address::fromHex(contract.owner_address());
       to_address = TW::Tron::Address::fromHex(contract.to_address());
    amount = contract.amount();

    // Calculate the offset of each item
    if (!calculateOffset()) {
        clear();
    }
}


/// Converts an external TransferContract to an internal one used for signing.
::protocol::TransferContract TransferContract::to_internal() const {

    ::protocol::TransferContract encode;

    encode.clear_owner_address();
    if (0 < owner_address.size()) {
        Data address = TW::Tron::Address::toHex(owner_address);
        if (0 < address.size()) {
            *encode.mutable_owner_address() = {address.begin(), address.end()};
        }
        else {
            *encode.mutable_owner_address() = owner_address;
        }
    }

    encode.clear_to_address();
    if (0 < to_address.size()) {
        Data address = TW::Tron::Address::toHex(to_address);
        if (0 < address.size()) {
            *encode.mutable_to_address() = {address.begin(), address.end()};
        }
        else {
            *encode.mutable_to_address() = to_address;
        }
    }

    encode.clear_amount();
    if (0 < amount) {
        encode.set_amount(amount);
    }

    return encode;
}


bool TransferContract::calculateOffset() {

    if (!Contract::calculateOffset()) {
        return false;
    }

    pb_length_delimited    pbToAddress = getToAddress();
    pb_varint pbAmount = getAmount();
    if (!pbToAddress.isValid()
        || !pbAmount.isValid()
        ) {
        return false;
    }

    size_t    szToAddress = 0;
    if (search(szOwnerAddress,
               pbToAddress.serialize(),
               toAddrIndex)
        ) {
        szToAddress = pbToAddress.size();

        toAddrSize  = pbToAddress.sizeValue();
        toAddrIndex += pbToAddress.sizeTag() + pbToAddress.sizeLength();
    }

    if (search(szOwnerAddress+szToAddress,
               pbAmount.serialize(),
               amtIndex)
        ) {
        amtSize  = pbAmount.sizeValue();
        amtIndex += pbAmount.sizeTag();
    }

    return true;
}


TW::Data TransferContract::toAddressSize() const {

    TW::Data o;
    if (!pb_basevarint::encode((uint32_t)toAddrSize, o)) {
        o.clear();
    }

    return o;
}


TW::Data TransferContract::toAddressOffset(const size_t offset) const {

    TW::Data o;
    if (!pb_basevarint::encode((uint32_t)toAddressIndex(offset), o)) {
        o.clear();
    }

    return o;
}


size_t TransferContract::toAddressIndex(const size_t offset) const {

    if (0 == toAddrIndex) {
        return toAddrIndex;
    }

    return (toAddrIndex + offset);
}


TW::Data TransferContract::amountSize() const {

    TW::Data o;
    if (!pb_basevarint::encode((uint32_t)amtSize, o)) {
        o.clear();
    }

    return o;
}


TW::Data TransferContract::amountOffset(const size_t offset) const {

    TW::Data o;
    if (!pb_basevarint::encode((uint32_t)amountIndex(offset), o)) {
        o.clear();
    }

    return o;
}


size_t TransferContract::amountIndex(const size_t offset) const {

    if (0 == amtIndex) {
        return amtIndex;
    }

    return (amtIndex + offset);
}


pb_length_delimited TransferContract::getToAddress() const {

    return pb_length_delimited(::protocol::TransferContract::kToAddressFieldNumber,
                               WireFormatLite::FieldType::TYPE_STRING,
                               TW::Tron::Address::toHex(to_address));
}


pb_varint TransferContract::getAmount() const {

    return pb_varint(::protocol::TransferContract::kAmountFieldNumber,
                     WireFormatLite::FieldType::TYPE_INT64,
                     amount);
}


} // namespace TW::Tron end
