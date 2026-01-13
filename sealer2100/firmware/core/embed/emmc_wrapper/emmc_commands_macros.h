//clang-format off

#ifndef _EMMC_COMMANDS_MACROS_H_
  #define _EMMC_COMMANDS_MACROS_H_

  #define MSG_INIT(NAME, TYPE) TYPE NAME = TYPE##_init_default

  #define MSG_ASSIGN_CALLBACK_ENCODE(NAME, FIELD, CB_ENCODE, ARGUMENT) \
        do                                                             \
        {                                                              \
            NAME.FIELD.funcs.encode = CB_ENCODE;                       \
            NAME.FIELD.arg = (void*)ARGUMENT;                          \
        }                                                              \
        while ( 0 )
  #define MSG_ASSIGN_CALLBACK_DECODE(NAME, FIELD, CB_DECODE, ARGUMENT) \
        do                                                             \
        {                                                              \
            NAME.FIELD.funcs.decode = CB_DECODE;                       \
            NAME.FIELD.arg = (void*)ARGUMENT;                          \
        }                                                              \
        while ( 0 )
  #define MSG_ASSIGN_REQUIRED_VALUE(NAME, FIELD, VALUE) \
        do                                              \
        {                                               \
            NAME.FIELD = VALUE;                         \
        }                                               \
        while ( 0 )
  #define MSG_ASSIGN_VALUE(NAME, FIELD, VALUE)             \
        do                                                 \
        {                                                  \
            NAME.has_##FIELD = true;                       \
            MSG_ASSIGN_REQUIRED_VALUE(NAME, FIELD, VALUE); \
        }                                                  \
        while ( 0 )
  #define MSG_ASSIGN_REQUIRED_STRING(NAME, FIELD, VALUE)        \
        do                                                      \
        {                                                       \
            memzero(NAME.FIELD, sizeof(NAME.FIELD));            \
            strncpy(NAME.FIELD, VALUE, sizeof(NAME.FIELD) - 1); \
        }                                                       \
        while ( 0 )
  #define MSG_ASSIGN_STRING(NAME, FIELD, VALUE)             \
        do                                                  \
        {                                                   \
            NAME.has_##FIELD = true;                        \
            MSG_ASSIGN_REQUIRED_STRING(NAME, FIELD, VALUE); \
        }                                                   \
        while ( 0 )
  #define MSG_ASSIGN_REQUIRED_BYTES(NAME, FIELD, VALUE, LEN)                     \
        do                                                                       \
        {                                                                        \
            memzero(NAME.FIELD.bytes, sizeof(NAME.FIELD.bytes));                 \
            memcpy(NAME.FIELD.bytes, VALUE, MIN(LEN, sizeof(NAME.FIELD.bytes))); \
            NAME.FIELD.size = MIN(LEN, sizeof(NAME.FIELD.bytes));                \
        }                                                                        \
        while ( 0 )
  #define MSG_ASSIGN_BYTES(NAME, FIELD, VALUE, LEN)             \
        do                                                      \
        {                                                       \
            MSG_ASSIGN_REQUIRED_BYTES(NAME, FIELD, VALUE, LEN); \
        }                                                       \
        while ( 0 )

  #define MSG_RECV_RET_ON_ERR(NAME, TYPE)                                                \
        do                                                                               \
        {                                                                                \
            if ( !recv_msg_async_parse(iface_num, msg_size, buf, TYPE##_fields, &NAME) ) \
                return -1;                                                               \
        }                                                                                \
        while ( 0 )
  #define MSG_RECV_NOCHECK(NAME, TYPE)                                            \
        do                                                                        \
        {                                                                         \
            recv_msg_async_parse(iface_num, msg_size, buf, TYPE##_fields, &NAME); \
        }                                                                         \
        while ( 0 )
  #define MSG_SEND(NAME, TYPE)                                                                \
        do                                                                                    \
        {                                                                                     \
            send_msg(iface_num, MessageType_MessageType_##TYPE, TYPE##_fields, &NAME, false); \
        }                                                                                     \
        while ( 0 )
  #define MSG_SEND_NOCHECK(NAME, TYPE)                                                       \
        do                                                                                   \
        {                                                                                    \
            send_msg(iface_num, MessageType_MessageType_##TYPE, TYPE##_fields, &NAME, true); \
        }                                                                                    \
        while ( 0 )
#endif //_EMMC_COMMANDS_MACROS_H_

//clang-format on
