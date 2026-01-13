#include "ProtocolDecoder.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef __ANDROID__
#include "utils.h"
#include "log_utils.h"
#endif

long ProtocolDecoder::msgDataLen = 0L;
int ProtocolDecoder::messageType = 0;
std::vector<uint8_t> ProtocolDecoder::buffer;

bool ProtocolDecoder::isHeaderChunk(const std::vector<uint8_t>& chunk) {
    if (chunk.size() < 9) {
        return false;
    }

    char magicQuestionMark = chunk[0];
    char sharp1 = chunk[1];
    char sharp2 = chunk[2];

    return magicQuestionMark == ProtocolConstants::MESSAGE_TOP_CHAR &&
           sharp1 == ProtocolConstants::MESSAGE_HEADER_BYTE &&
           sharp2 == ProtocolConstants::MESSAGE_HEADER_BYTE;
}

bool ProtocolDecoder::isEncrypedHeaderChunk(const std::vector<uint8_t> &chunk)
{
    if (chunk.size() < 9)
    {
        return false;
    }

    char magicQuestionMark = chunk[0];
    char sharp1 = chunk[1];
    char sharp2 = chunk[2];

    return magicQuestionMark == ProtocolConstants::MESSAGE_HEADER_BYTE &&
           sharp1 == ProtocolConstants::MESSAGE_TOP_CHAR &&
           sharp2 == ProtocolConstants::MESSAGE_TOP_CHAR;
}

int ProtocolDecoder::decode16BE(const std::vector<uint8_t>& src, int offset) {
    return (static_cast<int>(src[offset + 1]) & 0xFF) |
           ((static_cast<int>(src[offset + 0]) & 0xFF) << 8);
}

long ProtocolDecoder::decode32BE(const std::vector<uint8_t>& src, int offset) {
    return (static_cast<long>(src[offset + 3]) & 0xFFL) |
           ((static_cast<long>(src[offset + 2]) & 0xFFL) << 8) |
           ((static_cast<long>(src[offset + 1]) & 0xFFL) << 16) |
           ((static_cast<long>(src[offset + 0]) & 0xFFL) << 24);
}

bool ProtocolDecoder::packetCompletionCheck(const uint8_t* value, size_t length) {
    if (value == nullptr || length == 0) {
        return false;
    }

    size_t packetSize = length;
    // 将 std::vector 操作改为指针操作
    std::vector<uint8_t> chunk(value, value + length);
    if (ProtocolDecoder::isHeaderChunk(chunk)) {
        // 新指令，清空 buffer
        clear();
        // ?##<msg type><data len><data>
        messageType = ProtocolDecoder::decode16BE(chunk, 3);
        msgDataLen = ProtocolDecoder::decode32BE(chunk, 5);
        // ?##<msg type><data len>
        msgDataLen += 1 + 2 + 2 + 4;
        // 只要 payload
        buffer.insert(buffer.end(), value + 9, value + length);
    }
    else if (ProtocolDecoder::isEncrypedHeaderChunk(chunk))
    {
        clear();
        messageType = 0;
        if(chunk[3] != 0x05){
            //不是加密交易数据，是命令数据，直接全部返回
            msgDataLen = length;
            buffer.insert(buffer.end(), value, value + length);
        }else{
            uint16_t iv_len = static_cast<uint16_t>(chunk[4]);
            uint16_t mac_tag_len = static_cast<uint16_t>(chunk[4 + 1 + iv_len]);
            uint16_t encrypted_msg_len = (static_cast<uint16_t>(chunk[4 + 1 + iv_len + 1 + mac_tag_len]) << 8) |
                                         static_cast<uint16_t>(chunk[4 + 1 + iv_len + 1 + mac_tag_len + 1]);
            msgDataLen = encrypted_msg_len;
            // ?##<iv_len><iv><mac_tag_len><mac_tag><encrypted_msg_len><encrypted_msg>
            msgDataLen += 3 + 1 + iv_len + 1 + mac_tag_len + 2;
            // 密文情况 下，数据全都要
            buffer.insert(buffer.end(), value, value + length);
        }
    }else {
        buffer.insert(buffer.end(), value, value + length);
    }
    msgDataLen -= packetSize;

    return msgDataLen <= 0;
}

MessageResponse ProtocolDecoder::decode() {
#ifdef __ANDROID__
#ifdef DEBUG
    std::string hexBuffer = toHexString(buffer);
    LOG_INF("[cmd] Decode : messageType: %d, data: %s", messageType, hexBuffer.c_str());
#endif
#endif

    return MessageResponse(messageType, buffer);
}

MessageResponse ProtocolDecoder::decodeTV(const uint8_t *value, size_t length){
    uint16_t msgType = ProtocolDecoder::decode16BE(std::vector<uint8_t>(value, value + length), 0);
    return MessageResponse(msgType, std::vector<uint8_t>(value + 2, value + length));
}

void ProtocolDecoder::clear() {
    buffer.clear();
    msgDataLen = 0L;
    messageType = 0;
}

