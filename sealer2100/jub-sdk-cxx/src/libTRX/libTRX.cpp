#include "JUB_SDK_TRX.h"
#include "libTRX/libTRX.hpp"

#include "utility/util.h"
#include <bigint/BigIntegerUtils.hh>
#include <Tron/Transaction.h>
#include <Tron/Serialization.h>
#include <../HexCoding.h>

namespace jub {

namespace trx {


JUB_RV SerializeContract(const JUB_CONTRACT_TRX& contract,
                         ::google::protobuf::Any& parameter) {
    try {
        switch (contract.type) {
        case JUB_ENUM_TRX_CONTRACT_TYPE::ACCT_PERM_UPDATE_CONTRACT: {
            JUB_CHECK_NULL(contract.acctPermUpdate.owner_address);

            if (!TW::Tron::Address::isValid(contract.acctPermUpdate.owner_address)) {
                return JUBR_ARGUMENTS_BAD;
            }

            std::vector<TW::Tron::Key> owner_keys;
            for (JUB_UINT16 j=0; j<contract.acctPermUpdate.owner.keyCount; ++j) {
                owner_keys.push_back(
                    TW::Tron::Key(
                        std::string(contract.acctPermUpdate.owner.keys[j].address),
                        contract.acctPermUpdate.owner.keys[j].weight
                    )
                );
            }

            std::vector<TW::Tron::Permission> actives;
            for (JUB_UINT16 i=0; i<contract.acctPermUpdate.activeCount; ++i) {
                std::vector<TW::Tron::Key> keys;
                for (JUB_UINT16 j=0; j<contract.acctPermUpdate.actives[i].keyCount; ++j) {
                    keys.push_back(
                        TW::Tron::Key(
                            std::string(contract.acctPermUpdate.actives[i].keys[j].address),
                            contract.acctPermUpdate.actives[i].keys[j].weight
                        )
                    );
                }
                actives.push_back(
                    TW::Tron::Permission(
                        (protocol::Permission_PermissionType)contract.acctPermUpdate.actives[i].type,
                        (int32_t)contract.acctPermUpdate.actives[i].identity,
                        std::string(contract.acctPermUpdate.actives[i].permission_name),
                        contract.acctPermUpdate.actives[i].threshold,
                        uchar_vector(contract.acctPermUpdate.actives[i].operations),
                        keys
                    )
                );
            }

            parameter = TW::Tron::TransactionContract::to_parameter(
                            TW::Tron::AccountPermissionUpdateContract(
                                std::string(contract.acctPermUpdate.owner_address),
                                TW::Tron::Permission(
                                    (protocol::Permission_PermissionType)contract.acctPermUpdate.owner.type,
                                    (int32_t)contract.acctPermUpdate.owner.identity,
                                    std::string(contract.acctPermUpdate.owner.permission_name),
                                    contract.acctPermUpdate.owner.threshold,
                                    owner_keys
                                ),
                                TW::Tron::Permission(),
                                actives
                            )
                        );
        } break;
        case JUB_ENUM_TRX_CONTRACT_TYPE::XFER_CONTRACT: {
            JUB_CHECK_NULL(contract.transfer.owner_address);
            JUB_CHECK_NULL(contract.transfer.to_address);

            parameter = TW::Tron::TransactionContract::to_parameter(
                            TW::Tron::TransferContract(
                                contract.transfer.owner_address,
                                contract.transfer.to_address,
                                contract.transfer.amount
                            )
                        );
        } break;
        case JUB_ENUM_TRX_CONTRACT_TYPE::XFER_ASSET_CONTRACT: {
            JUB_CHECK_NULL(contract.transferAsset.asset_name);
            JUB_CHECK_NULL(contract.transferAsset.owner_address);
            JUB_CHECK_NULL(contract.transferAsset.to_address);

            parameter = TW::Tron::TransactionContract::to_parameter(
                            TW::Tron::TransferAssetContract(
                                contract.transferAsset.asset_name,
                                contract.transferAsset.owner_address,
                                contract.transferAsset.to_address,
                                contract.transferAsset.amount
                            )
                        );
        } break;
        case JUB_ENUM_TRX_CONTRACT_TYPE::CREATE_SMART_CONTRACT: {
            JUB_CHECK_NULL(contract.createSmart.owner_address);
            JUB_CHECK_NULL(contract.createSmart.bytecode);
            ::protocol::SmartContract pb;
            TW::Data bytecode(uchar_vector(contract.createSmart.bytecode));
            TW::Tron::SmartContract::deserialize(bytecode);
            parameter = TW::Tron::TransactionContract::to_parameter(
                            TW::Tron::CreateSmartContract(
                                contract.createSmart.owner_address,
                                pb,
                                contract.createSmart.call_token_value,
                                contract.createSmart.token_id
                            )
                        );
        } break;
        case JUB_ENUM_TRX_CONTRACT_TYPE::TRIG_SMART_CONTRACT: {
            JUB_CHECK_NULL(contract.triggerSmart.owner_address);
            JUB_CHECK_NULL(contract.triggerSmart.contract_address);
            JUB_CHECK_NULL(contract.triggerSmart.data);

            parameter = TW::Tron::TransactionContract::to_parameter(
                            TW::Tron::TriggerSmartContract(
                                contract.triggerSmart.owner_address,
                                contract.triggerSmart.contract_address,
                                contract.triggerSmart.call_value,
                                TW::parse_hex(contract.triggerSmart.data),
                                contract.triggerSmart.call_token_value,
                                contract.triggerSmart.token_id
                            )
                        );
        } break;
        default:
            return JUBR_ARGUMENTS_BAD;
        }
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }

    return JUBR_OK;
}


JUB_RV PackTransactionRaw(const JUB_TX_TRX& tx,
                          std::string& packedContractInPB) {
    try {
        uchar_vector vRefBlockBytes(tx.ref_block_bytes);

        int64_t bigRefBlockNum = 0;
        if (nullptr != tx.ref_block_num) {
            bigRefBlockNum = stringToBigInteger(tx.ref_block_num).toLong();
        }

        uchar_vector vRefBlockHash(tx.ref_block_hash);

        int64_t bigExpiration = stringToBigInteger(tx.expiration).toLong();
        int64_t bigFeeLimit   = stringToBigInteger(tx.fee_limit).toLong();

        std::vector<TW::Tron::Authority> vAuthorities = {};

        uchar_vector vData;
        if (nullptr != tx.data) {
            vData = uchar_vector(tx.data);
        }

        std::vector<TW::Tron::TransactionContract> vContracts;
        for (int i=0; i<tx.contractCount; ++i) {
            TW::Tron::TransactionContract contract;
            contract.type = tx.contracts[i].type;
            JUB_VERIFY_RV(SerializeContract(tx.contracts[i], contract.parameter));
            contract.permission_id = tx.contracts[i].permissionId;
            vContracts.push_back(contract);
        }

        int64_t bigTimestamp = 0;
        if (nullptr != tx.timestamp) {
            bigTimestamp = stringToBigInteger(tx.timestamp).toLong();
        }

        TW::Tron::TransactionRaw txRaw(vRefBlockBytes, bigRefBlockNum, vRefBlockHash,
                                       bigExpiration,
                                       vAuthorities,
                                       vData,
                                       vContracts,
                                       {},
                                       bigTimestamp,
                                       bigFeeLimit);
        packedContractInPB = uchar_vector(txRaw.serialize()).getHex();
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }

    return JUBR_OK;
}


JUB_RV SerializePreimage(const std::string& packedContractInPb,
                         uchar_vector& preimageRaw) {

    try {
        preimageRaw = uchar_vector(packedContractInPb);
    }
    catch (...) {
        return JUBR_ARGUMENTS_BAD;
    }

    return JUBR_OK;
}


} // namespace trx end

} // namespace jub end
