#include "librust_c.h"
#include "py/obj.h"
#include "py/runtime.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "device.h"
#ifndef EMULATOR
  #include "image.h"
#endif
#include "log.h"
#define MODULE "modairgapped"

/// package: ur_parser

/// class ResultContext:
///     """The context of the parsed result."""

/// class EncodeResultContext:
///     """The context of the encode result."""

/// class DecoderContext:
///     """The context of the decoder."""

/// class EncoderContext:
///     """The context of the encoder."""

// decoder handle class
typedef struct _mp_obj_decoder_handle_t {
    mp_obj_base_t base;
    void* decoder;
} mp_obj_decoder_handle_t;

STATIC const mp_obj_type_t decoder_handle_type;

STATIC mp_obj_t decoder_handle_new(void* decoder_ptr) {
    if (decoder_ptr == NULL) {
        return mp_const_none;
    }

    mp_obj_decoder_handle_t* o = m_new_obj(mp_obj_decoder_handle_t);
    o->base.type = &decoder_handle_type;
    o->decoder = decoder_ptr;
    return MP_OBJ_FROM_PTR(o);
}

STATIC void* decoder_from_obj(mp_obj_t obj) {
    if (obj == mp_const_none) {
        return NULL;
    }

    if (!mp_obj_is_type(obj, &decoder_handle_type)) {
        mp_raise_TypeError(MP_ERROR_TEXT("expected DecoderHandle"));
    }

    mp_obj_decoder_handle_t* o = MP_OBJ_TO_PTR(obj);
    return o->decoder;
}

STATIC const mp_obj_type_t decoder_handle_type = {
    {&mp_type_type},
    .name = MP_QSTR_DecoderHandle,
};

// data handle class
typedef struct _mp_obj_data_handle_t {
    mp_obj_base_t base;
    void* data;
} mp_obj_data_handle_t;

STATIC const mp_obj_type_t data_handle_type;

STATIC mp_obj_t ur_data_handle_new(void* data_ptr) {
    if (data_ptr == NULL) {
        return mp_const_none;
    }

    mp_obj_data_handle_t* o = m_new_obj(mp_obj_data_handle_t);
    o->base.type = &data_handle_type;
    o->data = data_ptr;
    return MP_OBJ_FROM_PTR(o);
}

STATIC void* ur_data_from_obj(mp_obj_t obj) {
    if (obj == mp_const_none) {
        return NULL;
    }

    if (!mp_obj_is_type(obj, &data_handle_type)) {
        mp_raise_TypeError(MP_ERROR_TEXT("expected DataHandle"));
    }

    mp_obj_data_handle_t* o = MP_OBJ_TO_PTR(obj);
    return o->data;
}

STATIC const mp_obj_type_t data_handle_type = {
    {&mp_type_type},
    .name = MP_QSTR_DataHandle,
};

// URParseResult wrapper class

typedef struct _mp_obj_ur_parse_result_t {
    mp_obj_base_t base;
    URParseResult* c_res;
} mp_obj_ur_parse_result_t;

STATIC mp_obj_t ur_parse_result_close(mp_obj_t self_in) {
    LOG_DEBUG(MODULE, "ur_parse_result_close called");
    mp_obj_ur_parse_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res != NULL) {
        free_ur_parse_result(self->c_res);
        self->c_res = NULL;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_result_close_obj, ur_parse_result_close);

STATIC mp_obj_t ur_parse_result_is_multi_part(mp_obj_t self_in) {
    mp_obj_ur_parse_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseResult has been freed"));
    }
    return mp_obj_new_bool(self->c_res->is_multi_part);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_result_is_multi_part_obj, ur_parse_result_is_multi_part);

STATIC mp_obj_t ur_parse_result_progress(mp_obj_t self_in) {
    mp_obj_ur_parse_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseResult has been freed"));
    }
    return mp_obj_new_int_from_uint(self->c_res->progress);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_result_progress_obj, ur_parse_result_progress);

STATIC mp_obj_t ur_parse_result_t(mp_obj_t self_in) {
    mp_obj_ur_parse_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseResult has been freed"));
    }
    return mp_obj_new_int(self->c_res->t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_result_t_obj, ur_parse_result_t);

STATIC mp_obj_t ur_parse_result_ur_type(mp_obj_t self_in) {
    mp_obj_ur_parse_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseResult has been freed"));
    }
    return mp_obj_new_int(self->c_res->ur_type);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_result_ur_type_obj, ur_parse_result_ur_type);

STATIC mp_obj_t ur_parse_result_data(mp_obj_t self_in) {
    mp_obj_ur_parse_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseResult has been freed"));
    }
    return ur_data_handle_new(self->c_res->data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_result_data_obj, ur_parse_result_data);

STATIC mp_obj_t ur_parse_result_decoder(mp_obj_t self_in) {
    mp_obj_ur_parse_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseResult has been freed"));
    }
    return decoder_handle_new(self->c_res->decoder);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_result_decoder_obj, ur_parse_result_decoder);

STATIC mp_obj_t ur_parse_result_error_code(mp_obj_t self_in) {
    mp_obj_ur_parse_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseResult has been freed"));
    }
    return mp_obj_new_int_from_uint(self->c_res->error_code);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_result_error_code_obj, ur_parse_result_error_code);

STATIC mp_obj_t ur_parse_result_error_message(mp_obj_t self_in) {
    mp_obj_ur_parse_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseResult has been freed"));
    }
    if (self->c_res->error_message == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->c_res->error_message, strlen(self->c_res->error_message));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_result_error_message_obj, ur_parse_result_error_message);

/// class URParseResult:
///     def close(self) -> None:
///         """Close the URParseResult object and free the memory."""
///         ...
///     def free(self) -> None:
///         """Close the URParseResult object and free the memory."""
///         ...
///     def is_multi_part(self) -> bool:
///         """Return True if the URParseResult object is multi-part."""
///         ...
///     def progress(self) -> int:
///         """Return multi-part progress in [0, 100]"""
///         ...
///     def t(self) -> int:
///         """The `ViewType` of the URParseResult object."""
///         ...
///     def ur_type(self) -> int:
///         """The `QRCodeType` of the URParseResult object."""
///         ...
///     def data(self) -> 'ResultContext':
///         """The parsed result.
///
///         """
///     def decoder(self) -> 'DecoderContext':
///         """The decoder used to parse the UR."""
///         ...
///     def error_code(self) -> int:
///         """The error code of the URParseResult object."""
///         ...
///     def error_message(self) -> str:
///         """The error message of the URParseResult object."""
///         ...

STATIC const mp_rom_map_elem_t ur_parse_result_locals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&ur_parse_result_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_free), MP_ROM_PTR(&ur_parse_result_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_is_multi_part), MP_ROM_PTR(&ur_parse_result_is_multi_part_obj)},
    {MP_ROM_QSTR(MP_QSTR_progress), MP_ROM_PTR(&ur_parse_result_progress_obj)},
    {MP_ROM_QSTR(MP_QSTR_t), MP_ROM_PTR(&ur_parse_result_t_obj)},
    {MP_ROM_QSTR(MP_QSTR_ur_type), MP_ROM_PTR(&ur_parse_result_ur_type_obj)},
    {MP_ROM_QSTR(MP_QSTR_data), MP_ROM_PTR(&ur_parse_result_data_obj)},
    {MP_ROM_QSTR(MP_QSTR_decoder), MP_ROM_PTR(&ur_parse_result_decoder_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_code), MP_ROM_PTR(&ur_parse_result_error_code_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_message), MP_ROM_PTR(&ur_parse_result_error_message_obj)},
};
STATIC MP_DEFINE_CONST_DICT(ur_parse_result_locals_dict, ur_parse_result_locals_table);

STATIC const mp_obj_type_t ur_parse_result_type = {
    {&mp_type_type},
    .name = MP_QSTR_URParseResult,
    .locals_dict = (mp_obj_dict_t*)&ur_parse_result_locals_dict,
};

// URParseMultiResult wrapper class

typedef struct _mp_obj_ur_parse_multi_result_t {
    mp_obj_base_t base;
    PtrT_URParseMultiResult c_res;
} mp_obj_ur_parse_multi_result_t;

STATIC mp_obj_t ur_parse_multi_result_close(mp_obj_t self_in) {
    LOG_DEBUG(MODULE, "ur_parse_multi_result_close called");
    mp_obj_ur_parse_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res != NULL) {
        free_ur_parse_multi_result(self->c_res);
        self->c_res = NULL;
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_multi_result_close_obj, ur_parse_multi_result_close);

STATIC mp_obj_t ur_parse_multi_result_is_complete(mp_obj_t self_in) {
    mp_obj_ur_parse_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseMultiResult freed"));
    }
    return mp_obj_new_bool(self->c_res->is_complete);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_multi_result_is_complete_obj, ur_parse_multi_result_is_complete);

STATIC mp_obj_t ur_parse_multi_result_t(mp_obj_t self_in) {
    mp_obj_ur_parse_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseMultiResult freed"));
    }
    return mp_obj_new_int(self->c_res->t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_multi_result_t_obj, ur_parse_multi_result_t);

STATIC mp_obj_t ur_parse_multi_result_ur_type(mp_obj_t self_in) {
    mp_obj_ur_parse_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseMultiResult freed"));
    }
    return mp_obj_new_int(self->c_res->ur_type);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_multi_result_ur_type_obj, ur_parse_multi_result_ur_type);

STATIC mp_obj_t ur_parse_multi_result_progress(mp_obj_t self_in) {
    mp_obj_ur_parse_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseMultiResult freed"));
    }
    return mp_obj_new_int_from_uint(self->c_res->progress);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_multi_result_progress_obj, ur_parse_multi_result_progress);

STATIC mp_obj_t ur_parse_multi_result_data(mp_obj_t self_in) {
    mp_obj_ur_parse_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseMultiResult freed"));
    }
    return ur_data_handle_new(self->c_res->data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_multi_result_data_obj, ur_parse_multi_result_data);

STATIC mp_obj_t ur_parse_multi_result_error_code(mp_obj_t self_in) {
    mp_obj_ur_parse_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseMultiResult freed"));
    }
    return mp_obj_new_int_from_uint(self->c_res->error_code);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_multi_result_error_code_obj, ur_parse_multi_result_error_code);

STATIC mp_obj_t ur_parse_multi_result_error_message(mp_obj_t self_in) {
    mp_obj_ur_parse_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("URParseMultiResult freed"));
    }
    if (self->c_res->error_message == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->c_res->error_message, strlen(self->c_res->error_message));
}
STATIC
MP_DEFINE_CONST_FUN_OBJ_1(ur_parse_multi_result_error_message_obj, ur_parse_multi_result_error_message);

/// class URParseMultiResult:
///     def close(self) -> None:
///         """Close the URParseMultiResult object and free the memory."""
///         ...
///     def free(self) -> None:
///         """Close the URParseMultiResult object and free the memory."""
///         ...
///     def is_complete(self) -> bool:
///         """Return True if the URParseMultiResult object is complete."""
///         ...
///     def t(self) -> int:
///         """The `ViewType` of the URParseMultiResult object."""
///         ...
///     def ur_type(self) -> int:
///         """The `QRCodeType` of the URParseMultiResult object."""
///         ...
///     def progress(self) -> int:
///         """Return multi-part progress in [0, 100]"""
///         ...
///     def data(self) -> 'ResultContext':
///         """The parsed result."""
///         ...
///     def error_code(self) -> int:
///         """The error code of the parsed result."""
///         ...
///     def error_message(self) -> str:
///         """The error message of the parsed result."""
///         ...

STATIC const mp_rom_map_elem_t ur_parse_multi_result_locals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&ur_parse_multi_result_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_free), MP_ROM_PTR(&ur_parse_multi_result_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_is_complete), MP_ROM_PTR(&ur_parse_multi_result_is_complete_obj)},
    {MP_ROM_QSTR(MP_QSTR_t), MP_ROM_PTR(&ur_parse_multi_result_t_obj)},
    {MP_ROM_QSTR(MP_QSTR_ur_type), MP_ROM_PTR(&ur_parse_multi_result_ur_type_obj)},
    {MP_ROM_QSTR(MP_QSTR_progress), MP_ROM_PTR(&ur_parse_multi_result_progress_obj)},
    {MP_ROM_QSTR(MP_QSTR_data), MP_ROM_PTR(&ur_parse_multi_result_data_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_code), MP_ROM_PTR(&ur_parse_multi_result_error_code_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_message), MP_ROM_PTR(&ur_parse_multi_result_error_message_obj)},
};
STATIC MP_DEFINE_CONST_DICT(ur_parse_multi_result_locals_dict, ur_parse_multi_result_locals_table);

STATIC const mp_obj_type_t ur_parse_multi_result_type = {
    {&mp_type_type},
    .name = MP_QSTR_URParseMultiResult,
    .locals_dict = (mp_obj_dict_t*)&ur_parse_multi_result_locals_dict,
};

// interfaces
/// mock:global
/// def parse_ur(ur: str) -> URParseResult:
///     """Parse the UR code."""
///     ...
STATIC mp_obj_t mp_parse_ur(mp_obj_t ur_in) {
    LOG_DEBUG(MODULE, "mp_parse_ur called");
    const char* ur_str = mp_obj_str_get_str(ur_in);

    LOG_DEBUG(MODULE, "parse_ur %s", ur_str);

    URParseResult* c_res = parse_ur((char*)ur_str);
    LOG_DEBUG(MODULE, "parse_ur result: %s", c_res->error_code == 0 ? "success" : c_res->error_message);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("parse_ur returned NULL"));
    }

    mp_obj_ur_parse_result_t* py_res = m_new_obj(mp_obj_ur_parse_result_t);
    py_res->base.type = &ur_parse_result_type;
    py_res->c_res = c_res;

    return MP_OBJ_FROM_PTR(py_res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_parse_ur_obj, mp_parse_ur);

/// mock:global
/// def receive(ur: str, decoder: DecoderContext) -> URParseMultiResult:
///     """Receive multi-part UR code."""
///     ...
STATIC mp_obj_t mp_receive(mp_obj_t ur_in, mp_obj_t decoder_in) {
    size_t ur_len;
    const char* ur_str = mp_obj_str_get_data(ur_in, &ur_len);

    PtrDecoder decoder_ptr = decoder_from_obj(decoder_in);

    PtrT_URParseMultiResult c_res = receive((char*)ur_str, decoder_ptr);
    mp_obj_ur_parse_multi_result_t* py_res = m_new_obj(mp_obj_ur_parse_multi_result_t);
    py_res->base.type = &ur_parse_multi_result_type;
    py_res->c_res = c_res;

    return MP_OBJ_FROM_PTR(py_res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_receive_obj, mp_receive);

typedef struct _mp_obj_encoder_handle_t {
    mp_obj_base_t base;
    PtrEncoder encoder;
} mp_obj_encoder_handle_t;

STATIC const mp_obj_type_t encoder_handle_type;

STATIC mp_obj_t encoder_handle_new(void* encoder_ptr) {
    if (encoder_ptr == NULL) {
        return mp_const_none;
    }

    mp_obj_encoder_handle_t* o = m_new_obj(mp_obj_encoder_handle_t);
    o->base.type = &encoder_handle_type;
    o->encoder = encoder_ptr;
    return MP_OBJ_FROM_PTR(o);
}

STATIC void* encoder_from_obj(mp_obj_t obj) {
    if (obj == mp_const_none) {
        return NULL;
    }

    if (!mp_obj_is_type(obj, &encoder_handle_type)) {
        mp_raise_TypeError(MP_ERROR_TEXT("expected EncoderHandle"));
    }

    mp_obj_encoder_handle_t* o = MP_OBJ_TO_PTR(obj);
    return o->encoder;
}

STATIC const mp_obj_type_t encoder_handle_type = {
    {&mp_type_type},
    .name = MP_QSTR_EncoderHandle,
};

typedef struct _mp_obj_ur_encode_result_t {
    mp_obj_base_t base;
    PtrT_UREncodeResult c_res;
} mp_obj_ur_encode_result_t;

STATIC mp_obj_t ur_encode_result_close(mp_obj_t self_in) {
    mp_obj_ur_encode_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res != NULL) {
        free_ur_encode_result(self->c_res);
        self->c_res = NULL;
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_encode_result_close_obj, ur_encode_result_close);

STATIC mp_obj_t ur_encode_result_is_multi_part(mp_obj_t self_in) {
    mp_obj_ur_encode_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("UREncodeResult has been freed"));
    }
    return mp_obj_new_bool(self->c_res->is_multi_part);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_encode_result_is_multi_part_obj, ur_encode_result_is_multi_part);

STATIC mp_obj_t ur_encode_result_data(mp_obj_t self_in) {
    mp_obj_ur_encode_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("UREncodeResult has been freed"));
    }
    if (self->c_res->data != NULL) {
        return mp_obj_new_str(self->c_res->data, strlen(self->c_res->data));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_encode_result_data_obj, ur_encode_result_data);

STATIC mp_obj_t ur_encode_result_encoder(mp_obj_t self_in) {
    mp_obj_ur_encode_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("UREncodeResult has been freed"));
    }
    return encoder_handle_new(self->c_res->encoder);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_encode_result_encoder_obj, ur_encode_result_encoder);

STATIC mp_obj_t ur_encode_result_error_code(mp_obj_t self_in) {
    mp_obj_ur_encode_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("UREncodeResult has been freed"));
    }
    return mp_obj_new_int_from_uint(self->c_res->error_code);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_encode_result_error_code_obj, ur_encode_result_error_code);

STATIC mp_obj_t ur_encode_result_error_message(mp_obj_t self_in) {
    mp_obj_ur_encode_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("UREncodeResult has been freed"));
    }
    if (self->c_res->error_message != NULL) {
        return mp_obj_new_str(self->c_res->error_message, strlen(self->c_res->error_message));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_encode_result_error_message_obj, ur_encode_result_error_message);

/// class UREncodeResult:
///     """The result of encoding UR code."""
///
///     def close(self) -> None:
///         """Close the encode result and free the memory."""
///         ...
///
///     def free(self) -> None:
///         """Free the memory of the encode result."""
///         ...
///     def is_multi_part(self) -> bool:
///         """Check if the encode result is multi-part."""
///         ...
///     def data(self) -> EncodeResultContext:
///         """The encoder context of the encode result."""
///         ...
///     def encoder(self) -> EncoderContext:
///         """The encoder context of the encode result."""
///         ...
///     def error_code(self) -> int:
///         """The error code of the encode result."""
///         ...
///     def error_message(self) -> str:
///         """The error message of the encode result."""
///         ...

STATIC const mp_rom_map_elem_t ur_encode_result_locals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&ur_encode_result_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_free), MP_ROM_PTR(&ur_encode_result_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_is_multi_part), MP_ROM_PTR(&ur_encode_result_is_multi_part_obj)},
    {MP_ROM_QSTR(MP_QSTR_data), MP_ROM_PTR(&ur_encode_result_data_obj)},
    {MP_ROM_QSTR(MP_QSTR_encoder), MP_ROM_PTR(&ur_encode_result_encoder_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_code), MP_ROM_PTR(&ur_encode_result_error_code_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_message), MP_ROM_PTR(&ur_encode_result_error_message_obj)},
};

STATIC MP_DEFINE_CONST_DICT(ur_encode_result_locals_dict, ur_encode_result_locals_table);

STATIC const mp_obj_type_t ur_encode_result_type = {
    {&mp_type_type},
    .name = MP_QSTR_UREncodeResult,
    .locals_dict = (mp_obj_dict_t*)&ur_encode_result_locals_dict,
};

typedef struct _mp_obj_ur_encode_multi_result_t {
    mp_obj_base_t base;
    PtrT_UREncodeMultiResult c_res;
} mp_obj_ur_encode_multi_result_t;

STATIC mp_obj_t ur_encode_multi_result_close(mp_obj_t self_in) {
    mp_obj_ur_encode_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res != NULL) {
        free_ur_encode_muilt_result(self->c_res);
        self->c_res = NULL;
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_encode_multi_result_close_obj, ur_encode_multi_result_close);

STATIC mp_obj_t ur_encode_multi_result_data(mp_obj_t self_in) {
    mp_obj_ur_encode_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("UREncodeMultiResult has been freed"));
    }
    if (self->c_res->data != NULL) {
        return mp_obj_new_str(self->c_res->data, strlen(self->c_res->data));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_encode_multi_result_data_obj, ur_encode_multi_result_data);

STATIC mp_obj_t ur_encode_multi_result_error_code(mp_obj_t self_in) {
    mp_obj_ur_encode_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("UREncodeMultiResult has been freed"));
    }
    return mp_obj_new_int_from_uint(self->c_res->error_code);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ur_encode_multi_result_error_code_obj, ur_encode_multi_result_error_code);

STATIC mp_obj_t ur_encode_multi_result_error_message(mp_obj_t self_in) {
    mp_obj_ur_encode_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("UREncodeMultiResult has been freed"));
    }
    if (self->c_res->error_message != NULL) {
        return mp_obj_new_str(self->c_res->error_message, strlen(self->c_res->error_message));
    }
    return mp_const_none;
}
STATIC
MP_DEFINE_CONST_FUN_OBJ_1(ur_encode_multi_result_error_message_obj, ur_encode_multi_result_error_message);

/// class UREncodeMultiResult:
///     """The result of encoding UR code."""
///     def close(self) -> None:
///         """Close the encode result and free the memory."""
///         ...
///     def free(self) -> None:
///         """Free the memory of the encode result."""
///         ...
///     def data(self) -> EncodeResultContext:
///         """The data of the encode result."""
///         ...
///     def error_code(self) -> int:
///         """The error code of the encode result."""
///         ...
///     def error_message(self) -> str:
///         """The error message of the encode result."""
///         ...
STATIC const mp_rom_map_elem_t ur_encode_multi_result_locals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&ur_encode_multi_result_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_free), MP_ROM_PTR(&ur_encode_multi_result_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_data), MP_ROM_PTR(&ur_encode_multi_result_data_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_code), MP_ROM_PTR(&ur_encode_multi_result_error_code_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_message), MP_ROM_PTR(&ur_encode_multi_result_error_message_obj)},
};
STATIC MP_DEFINE_CONST_DICT(ur_encode_multi_result_locals_dict, ur_encode_multi_result_locals_table);

STATIC const mp_obj_type_t ur_encode_multi_result_type = {
    {&mp_type_type},
    .name = MP_QSTR_UREncodeMultiResult,
    .locals_dict = (mp_obj_dict_t*)&ur_encode_multi_result_locals_dict,
};

/// mock:global
/// def get_next_part(self) -> UREncodeMultiResult:
///     """Get the next part of the encode result."""
///     ...
STATIC mp_obj_t ur_encode_multi_result_get_next_part(mp_obj_t self_in) {
    mp_obj_ur_encode_multi_result_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("UREncodeMultiResult freed"));
    }

    PtrEncoder encoder_ptr = encoder_from_obj(self_in);
    if (encoder_ptr == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("encoder is NULL"));
    }

    PtrT_UREncodeMultiResult c_res = get_next_part(encoder_ptr);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("get_next_part returned NULL"));
    }

    mp_obj_ur_encode_multi_result_t* py_res = m_new_obj(mp_obj_ur_encode_multi_result_t);
    py_res->base.type = &ur_encode_multi_result_type;
    py_res->c_res = c_res;

    return MP_OBJ_FROM_PTR(py_res);
}
STATIC
MP_DEFINE_CONST_FUN_OBJ_1(ur_encode_multi_result_get_next_part_obj, ur_encode_multi_result_get_next_part);

/// mock:global
/// def get_connect_metamask_ur(mfp: int, public_keys: list, origin: str) -> UREncodeMultiResult:
///     """Get the connect metamask UR code."""
///     ...
STATIC mp_obj_t mp_get_connect_metamask_ur(mp_obj_t mfp_in, mp_obj_t public_keys_in, mp_obj_t origin) {
    uint32_t mfp_int = mp_obj_get_int_truncated(mfp_in);
    if (mfp_int < 0 || mfp_int > 0xFFFFFFFFU) {
        mp_raise_ValueError(MP_ERROR_TEXT("mfp value out of range for uint32_t"));
    }
    uint8_t mfp_bytes[4] = {0};
    mfp_bytes[0] = (mfp_int >> 24) & 0xFF;
    mfp_bytes[1] = (mfp_int >> 16) & 0xFF;
    mfp_bytes[2] = (mfp_int >> 8) & 0xFF;
    mfp_bytes[3] = mfp_int & 0xFF;

    if (!mp_obj_is_type(public_keys_in, &mp_type_list)) {
        mp_raise_ValueError(MP_ERROR_TEXT("public_keys must be a list"));
    }
    size_t num_public_keys = MP_OBJ_SMALL_INT_VALUE(mp_obj_len(public_keys_in));
    ExtendedPublicKey* c_public_keys = m_new(ExtendedPublicKey, num_public_keys);
    mp_obj_t* items;
    mp_obj_list_get(public_keys_in, &num_public_keys, &items);
    for (size_t i = 0; i < num_public_keys; i++) {
        mp_obj_t item = items[i];
        mp_obj_t path_obj, xpub_obj;
        mp_obj_tuple_get(item, NULL, NULL);
        size_t tuple_len;
        mp_obj_t* tuple_items;
        mp_obj_tuple_get(item, &tuple_len, &tuple_items);
        if (tuple_len != 2) {
            mp_raise_ValueError(MP_ERROR_TEXT("each public_key must be a tuple of (path, xpub)"));
        }
        path_obj = tuple_items[0];
        xpub_obj = tuple_items[1];
        c_public_keys[i].path = (PtrString)mp_obj_str_get_str(path_obj);
        c_public_keys[i].xpub = (PtrString)mp_obj_str_get_str(xpub_obj);
    }
    CSliceFFI_ExtendedPublicKey public_keys_slice;
    public_keys_slice.data = c_public_keys;
    public_keys_slice.size = num_public_keys;

    const char* wallet_name_str = mp_obj_str_get_str(origin);

    PtrT_UREncodeResult c_res = get_connect_metamask_ur(
        (PtrBytes)mfp_bytes, 4, Bip44Standard, &public_keys_slice, (PtrString)wallet_name_str
    );
    m_free(c_public_keys);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("get_connect_metamask_ur returned NULL"));
    }

    mp_obj_ur_encode_result_t* py_res = m_new_obj(mp_obj_ur_encode_result_t);
    py_res->base.type = &ur_encode_result_type;
    py_res->c_res = c_res;
    return MP_OBJ_FROM_PTR(py_res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mp_get_connect_metamask_ur_obj, mp_get_connect_metamask_ur);

/// mock:global
/// def get_connect_hpx_ur(mfp: int, public_keys: list, origin: str, *, device_id: str, label: str, sn: str, passphrase: bool) -> UREncodeMultiResult:
///     """Get the connect HPX UR code."""
///     ...
STATIC mp_obj_t mp_get_connect_hpx_ur(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_device_id, ARG_label, ARG_sn, ARG_passphrase };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_device_id, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_label, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_sn, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_passphrase, MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_obj_t mfp_obj         = pos_args[0];
    mp_obj_t public_keys_obj = pos_args[1];
    mp_obj_t origin_obj      = pos_args[2];

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(
        n_args - 3,
        0,
        kw_args,
        MP_ARRAY_SIZE(allowed_args),
        allowed_args,
        args
    );

    mp_obj_t device_id_obj   = args[ARG_device_id].u_obj;
    mp_obj_t label_obj       = args[ARG_label].u_obj;
    mp_obj_t sn_obj          = args[ARG_sn].u_obj;
    bool passphrase_enabled  = args[ARG_passphrase].u_bool;


    uint32_t mfp_int = mp_obj_get_int_truncated(mfp_obj);
    if (mfp_int < 0 || mfp_int > 0xFFFFFFFFU) {
        mp_raise_ValueError(MP_ERROR_TEXT("mfp value out of range for uint32_t"));
    }
    uint8_t mfp_bytes[4] = {0};
    mfp_bytes[0] = (mfp_int >> 24) & 0xFF;
    mfp_bytes[1] = (mfp_int >> 16) & 0xFF;
    mfp_bytes[2] = (mfp_int >> 8) & 0xFF;
    mfp_bytes[3] = mfp_int & 0xFF;

    if (!mp_obj_is_type(public_keys_obj, &mp_type_list)) {
        mp_raise_ValueError(MP_ERROR_TEXT("public_keys must be a list"));
    }
    size_t num_public_keys = MP_OBJ_SMALL_INT_VALUE(mp_obj_len(public_keys_obj));
    ExtendedPublicKey* c_public_keys = m_new(ExtendedPublicKey, num_public_keys);
    mp_obj_t* items;
    mp_obj_list_get(public_keys_obj, &num_public_keys, &items);
    for (size_t i = 0; i < num_public_keys; i++) {
        mp_obj_t item = items[i];
        mp_obj_t path_obj, xpub_obj;
        mp_obj_tuple_get(item, NULL, NULL);
        size_t tuple_len;
        mp_obj_t* tuple_items;
        mp_obj_tuple_get(item, &tuple_len, &tuple_items);
        if (tuple_len != 2) {
            mp_raise_ValueError(MP_ERROR_TEXT("each public_key must be a tuple of (path, xpub)"));
        }
        path_obj = tuple_items[0];
        xpub_obj = tuple_items[1];
        c_public_keys[i].path = (PtrString)mp_obj_str_get_str(path_obj);
        c_public_keys[i].xpub = (PtrString)mp_obj_str_get_str(xpub_obj);
        LOG_DEBUG(MODULE, "Public key %d: path=%s, xpub=%s", i, c_public_keys[i].path, c_public_keys[i].xpub);
    }
    CSliceFFI_ExtendedPublicKey public_keys_slice;
    public_keys_slice.data = c_public_keys;
    public_keys_slice.size = num_public_keys;

    const char* wallet_name_str = mp_obj_str_get_str(origin_obj);
    const char* device_id_str = mp_obj_str_get_str(device_id_obj);
    const char* device_label = mp_obj_str_get_str(label_obj);
    const char* device_sn = mp_obj_str_get_str(sn_obj);
    LOG_DEBUG(MODULE, "device_id: %s", device_id_str);
    LOG_DEBUG(MODULE, "label: %s", device_label);
    LOG_DEBUG(MODULE, "sn: %s", device_sn);
    LOG_DEBUG(MODULE, "passphrase: %s", passphrase_enabled ? "enabled" : "disabled");

    char version[32] = {0};
#ifndef EMULATOR
    image_version_format(FIRMWARE->header.version, version);
#endif

    PtrT_UREncodeResult c_res = get_hpx_wallet_ur(
        (PtrBytes)mfp_bytes, 4, (PtrString)device_id_str, &public_keys_slice, (PtrString)wallet_name_str, version, (PtrString)device_label, (PtrString)device_sn, passphrase_enabled
    );
    m_free(c_public_keys);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("get_connect_metamask_ur returned NULL"));
    }

    mp_obj_ur_encode_result_t* py_res = m_new_obj(mp_obj_ur_encode_result_t);
    py_res->base.type = &ur_encode_result_type;
    py_res->c_res = c_res;
    return MP_OBJ_FROM_PTR(py_res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_get_connect_hpx_ur_obj, 3, mp_get_connect_hpx_ur);

//////////////////////////////////////////////////EVM functions//////////////////////////////////
typedef struct _mp_obj_result_eth_parsed_raw_t {
    mp_obj_base_t base;
    PtrT_TransactionParseResult_CParsedEthereumTransaction c_res;
} mp_obj_result_eth_parsed_raw_t;

STATIC mp_obj_t eth_parsed_raw_close(mp_obj_t self_in) {
    mp_obj_result_eth_parsed_raw_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res != NULL) {
        free_TransactionParseResult_CParsedEthereumTransaction(self->c_res);
        self->c_res = NULL;
    }
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_close_obj, eth_parsed_raw_close);

STATIC PtrT_TransactionParseResult_CParsedEthereumTransaction get_eth_parsed_raw(mp_obj_t self_in) {
    mp_obj_result_eth_parsed_raw_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("CParsedEthereumTransaction freed"));
    }
    return self->c_res;
}

STATIC mp_obj_t eth_parsed_raw_value(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_str(c->data->value, strlen(c->data->value));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_value_obj, eth_parsed_raw_value);

STATIC mp_obj_t eth_parsed_raw_max_txn_fee(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_str(c->data->max_txn_fee, strlen(c->data->max_txn_fee));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_max_txn_fee_obj, eth_parsed_raw_max_txn_fee);

STATIC mp_obj_t eth_parsed_raw_gas_price(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_str(c->data->gas_price, strlen(c->data->gas_price));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_gas_price_obj, eth_parsed_raw_gas_price);

STATIC mp_obj_t eth_parsed_raw_gas_limit(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_str(c->data->gas_limit, strlen(c->data->gas_limit));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_gas_limit_obj, eth_parsed_raw_gas_limit);

STATIC mp_obj_t eth_parsed_raw_from(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_str(c->data->from, strlen(c->data->from));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_from_obj, eth_parsed_raw_from);

STATIC mp_obj_t eth_parsed_raw_to(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_str(c->data->to, strlen(c->data->to));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_to_obj, eth_parsed_raw_to);

STATIC mp_obj_t eth_parsed_raw_nonce(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_int_from_uint(c->data->nonce);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_nonce_obj, eth_parsed_raw_nonce);

STATIC mp_obj_t eth_parsed_raw_input(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_str(c->data->input, strlen(c->data->input));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_input_obj, eth_parsed_raw_input);

STATIC mp_obj_t eth_parsed_raw_max_fee(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_str(c->data->max_fee, strlen(c->data->max_fee));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_max_fee_obj, eth_parsed_raw_max_fee);

STATIC mp_obj_t eth_parsed_raw_max_priority(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_str(c->data->max_priority, strlen(c->data->max_priority));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_max_priority_obj, eth_parsed_raw_max_priority);

STATIC mp_obj_t eth_parsed_raw_max_fee_per_gas(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_str(c->data->max_fee_per_gas, strlen(c->data->max_fee_per_gas));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_max_fee_per_gas_obj, eth_parsed_raw_max_fee_per_gas);

STATIC mp_obj_t eth_parsed_raw_max_priority_fee_per_gas(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_str(c->data->max_priority_fee_per_gas, strlen(c->data->max_priority_fee_per_gas));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(
    eth_parsed_raw_max_priority_fee_per_gas_obj, eth_parsed_raw_max_priority_fee_per_gas
);

STATIC mp_obj_t eth_parsed_raw_chain_id(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_int_from_uint(c->data->chain_id);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_chain_id_obj, eth_parsed_raw_chain_id);

STATIC mp_obj_t eth_parsed_raw_tx_type(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_int_from_uint(c->data->tx_type);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_tx_type_obj, eth_parsed_raw_tx_type);

STATIC mp_obj_t eth_parsed_raw_error_code(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    return mp_obj_new_int_from_uint(c->error_code);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_error_code_obj, eth_parsed_raw_error_code);

STATIC mp_obj_t eth_parsed_raw_error_message(mp_obj_t self_in) {
    PtrT_TransactionParseResult_CParsedEthereumTransaction c = get_eth_parsed_raw(self_in);
    if (c->error_message == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(c->error_message, strlen(c->error_message));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_raw_error_message_obj, eth_parsed_raw_error_message);

/// class EthParsedRaw:
///     """The parsed Ethereum transaction."""
///     def close(self):
///         """Close the parsed Ethereum transaction and free the memory."""
///         ...
///     def free(self):
///         """Free the parsed Ethereum transaction and close the memory."""
///         ...
///     def value(self) -> int:
///         """The value of the transaction."""
///         ...
///     def max_txn_fee(self) -> int:
///         """The maximum transaction fee of the transaction."""
///         ...
///     def gas_price(self) -> int:
///         """The gas price of the transaction."""
///         ...
///     def gas_limit(self) -> int:
///         """The gas limit of the transaction."""
///         ...
///     def to(self) -> str:
///         """The receiver of the transaction."""
///         ...
///     def nonce(self) -> int:
///         """The nonce of the transaction."""
///         ...
///     def input(self) -> str:
///         """The input of the transaction."""
///         ...
///     def max_fee(self) -> str:
///         """The maximum transaction fee of the transaction."""
///         ...
///     def max_priority(self) -> str:
///         """The maximum priority of the transaction."""
///         ...
///     def max_fee_per_gas(self) -> str:
///         """The maximum fee per gas of the transaction."""
///         ...
///     def max_priority_fee_per_gas(self) -> str:
///         """The maximum priority fee per gas of the transaction."""
///         ...
///     def chain_id(self) -> int:
///         """The chain ID of the transaction."""
///         ...
///     def tx_type(self) -> int:
///         """The transaction type of the transaction."""
///         ...
///     def error_code(self) -> int:
///         """The error code of the transaction."""
///         ...
///     def error_message(self) -> str:
///         """The error message of the transaction."""
///         ...
STATIC const mp_rom_map_elem_t eth_parsed_raw_locals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&eth_parsed_raw_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_free), MP_ROM_PTR(&eth_parsed_raw_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&eth_parsed_raw_value_obj)},
    {MP_ROM_QSTR(MP_QSTR_max_txn_fee), MP_ROM_PTR(&eth_parsed_raw_max_txn_fee_obj)},
    {MP_ROM_QSTR(MP_QSTR_gas_price), MP_ROM_PTR(&eth_parsed_raw_gas_price_obj)},
    {MP_ROM_QSTR(MP_QSTR_gas_limit), MP_ROM_PTR(&eth_parsed_raw_gas_limit_obj)},
    {MP_ROM_QSTR(MP_QSTR_from), MP_ROM_PTR(&eth_parsed_raw_from_obj)},
    {MP_ROM_QSTR(MP_QSTR_to), MP_ROM_PTR(&eth_parsed_raw_to_obj)},
    {MP_ROM_QSTR(MP_QSTR_nonce), MP_ROM_PTR(&eth_parsed_raw_nonce_obj)},
    {MP_ROM_QSTR(MP_QSTR_input), MP_ROM_PTR(&eth_parsed_raw_input_obj)},
    {MP_ROM_QSTR(MP_QSTR_max_fee), MP_ROM_PTR(&eth_parsed_raw_max_fee_obj)},
    {MP_ROM_QSTR(MP_QSTR_max_priority), MP_ROM_PTR(&eth_parsed_raw_max_priority_obj)},
    {MP_ROM_QSTR(MP_QSTR_max_fee_per_gas), MP_ROM_PTR(&eth_parsed_raw_max_fee_per_gas_obj)},
    {MP_ROM_QSTR(MP_QSTR_max_priority_fee_per_gas), MP_ROM_PTR(&eth_parsed_raw_max_priority_fee_per_gas_obj)},
    {MP_ROM_QSTR(MP_QSTR_chain_id), MP_ROM_PTR(&eth_parsed_raw_chain_id_obj)},
    {MP_ROM_QSTR(MP_QSTR_tx_type), MP_ROM_PTR(&eth_parsed_raw_tx_type_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_code), MP_ROM_PTR(&eth_parsed_raw_error_code_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_message), MP_ROM_PTR(&eth_parsed_raw_error_message_obj)},
};
STATIC MP_DEFINE_CONST_DICT(eth_parsed_raw_locals_dict, eth_parsed_raw_locals_table);

STATIC const mp_obj_type_t eth_parsed_raw_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_EthParsedRaw,
    .locals_dict = (mp_obj_dict_t*)&eth_parsed_raw_locals_dict,
};

/// mock:global
/// def eth_parse_raw(ur: ResultContext, xpub: str) -> EthParsedRaw:
///     """Parse the Ethereum transaction."""
///     ...
STATIC mp_obj_t mp_eth_parse_raw(mp_obj_t ur_in, mp_obj_t xpub_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    const char* xpub_str = mp_obj_str_get_str(xpub_in);

    PtrT_TransactionParseResult_CParsedEthereumTransaction c_res =
        eth_parse_raw((PtrUR)ur_data, (PtrString)xpub_str);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("eth_parse_raw returned NULL"));
    }

    mp_obj_result_eth_parsed_raw_t* py_res = m_new_obj(mp_obj_result_eth_parsed_raw_t);
    py_res->base.type = &eth_parsed_raw_type;
    py_res->c_res = c_res;

    return MP_OBJ_FROM_PTR(py_res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_eth_parse_raw_obj, mp_eth_parse_raw);

/// mock:global
/// def eth_check(ur: ResultContext, mfp: int) -> int:
///     """Check the Ethereum transaction."""
///     ...
STATIC mp_obj_t mp_eth_check(mp_obj_t ur_in, mp_obj_t mfp_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    uint32_t mfp_int = mp_obj_get_int_truncated(mfp_in);
    if (mfp_int < 0 || mfp_int > 0xFFFFFFFFU) {
        mp_raise_ValueError(MP_ERROR_TEXT("mfp value out of range for uint32_t"));
    }
    uint8_t mfp_bytes[4] = {0};
    mfp_bytes[0] = (mfp_int >> 24) & 0xFF;
    mfp_bytes[1] = (mfp_int >> 16) & 0xFF;
    mfp_bytes[2] = (mfp_int >> 8) & 0xFF;
    mfp_bytes[3] = mfp_int & 0xFF;

    PtrT_TransactionCheckResult c_res = eth_check((PtrUR)ur_data, (PtrBytes)mfp_bytes, 4);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("eth_check returned NULL"));
    }

    uint32_t error_code = c_res->error_code;
    free_TransactionCheckResult(c_res);
    return mp_obj_new_int_from_uint(error_code);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_eth_check_obj, mp_eth_check);

typedef struct _mp_obj_eth_parsed_personal_message_t {
    mp_obj_base_t base;
    PtrT_TransactionParseResult_DisplayETHPersonalMessage c_res;
} mp_obj_eth_parsed_personal_message_t;

STATIC mp_obj_t eth_parsed_personal_message_close(mp_obj_t self_in) {
    mp_obj_eth_parsed_personal_message_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res != NULL) {
        free_TransactionParseResult_DisplayETHPersonalMessage(self->c_res);
        self->c_res = NULL;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_personal_message_close_obj, eth_parsed_personal_message_close);

STATIC PtrT_TransactionParseResult_DisplayETHPersonalMessage
get_eth_parsed_personal_message(mp_obj_t self_in) {
    mp_obj_eth_parsed_personal_message_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("DisplayETHPersonalMessage freed"));
    }
    return self->c_res;
}

STATIC mp_obj_t eth_parsed_personal_message_raw_message(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHPersonalMessage c = get_eth_parsed_personal_message(self_in);
    return mp_obj_new_str(c->data->raw_message, strlen(c->data->raw_message));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(
    eth_parsed_personal_message_raw_message_obj, eth_parsed_personal_message_raw_message
);

STATIC mp_obj_t eth_parsed_personal_message_utf8_message(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHPersonalMessage c = get_eth_parsed_personal_message(self_in);
    return mp_obj_new_str(c->data->utf8_message, strlen(c->data->utf8_message));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(
    eth_parsed_personal_message_utf8_message_obj, eth_parsed_personal_message_utf8_message
);

STATIC mp_obj_t eth_parsed_personal_message_from(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHPersonalMessage c = get_eth_parsed_personal_message(self_in);
    return mp_obj_new_str(c->data->from, strlen(c->data->from));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_personal_message_from_obj, eth_parsed_personal_message_from);

STATIC mp_obj_t eth_parsed_personal_message_error_code(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHPersonalMessage c = get_eth_parsed_personal_message(self_in);
    return mp_obj_new_int_from_uint(c->error_code);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(
    eth_parsed_personal_message_error_code_obj, eth_parsed_personal_message_error_code
);

STATIC mp_obj_t eth_parsed_personal_message_error_message(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHPersonalMessage c = get_eth_parsed_personal_message(self_in);
    if (c->error_message == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(c->error_message, strlen(c->error_message));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(
    eth_parsed_personal_message_error_message_obj, eth_parsed_personal_message_error_message
);

/// class EthParsedPersonalMessage:
///     """The parsed Ethereum personal message."""
///     def close(self) -> None:
///         """Close the parsed Ethereum personal message and free the memory."""
///         ...
///     def free(self) -> None:
///         """Free the parsed Ethereum personal message."""
///         ...
///     def raw_message(self) -> str:
///         """Get the raw message."""
///         ...
///     def utf8_message(self) -> str:
///         """Get the UTF-8 message."""
///         ...
///     def error_code(self) -> int:
///         """Get the error code."""
///         ...
///     def error_message(self) -> str:
///         """Get the error message."""
///         ...
STATIC const mp_rom_map_elem_t eth_parsed_personal_message_locals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&eth_parsed_personal_message_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_free), MP_ROM_PTR(&eth_parsed_personal_message_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_raw_message), MP_ROM_PTR(&eth_parsed_personal_message_raw_message_obj)},
    {MP_ROM_QSTR(MP_QSTR_utf8_message), MP_ROM_PTR(&eth_parsed_personal_message_utf8_message_obj)},
    {MP_ROM_QSTR(MP_QSTR_from), MP_ROM_PTR(&eth_parsed_personal_message_from_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_code), MP_ROM_PTR(&eth_parsed_personal_message_error_code_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_message), MP_ROM_PTR(&eth_parsed_personal_message_error_message_obj)},
};
STATIC
MP_DEFINE_CONST_DICT(eth_parsed_personal_message_locals_dict, eth_parsed_personal_message_locals_table);

STATIC const mp_obj_type_t eth_parsed_personal_message_type = {
    {&mp_type_type},
    .name = MP_QSTR_EthParsedPersonalMessage,
    .locals_dict = (mp_obj_dict_t*)&eth_parsed_personal_message_locals_dict,
};

/// mock:global
/// def eth_parse_personal_message(ur: ResultContext, xpub: str) -> EthParsedPersonalMessage:
///     """Parse the Ethereum personal message."""
///     ...
STATIC mp_obj_t mp_eth_parse_personal_message(mp_obj_t ur_in, mp_obj_t xpub_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    const char* xpub_str = mp_obj_str_get_str(xpub_in);

    PtrT_TransactionParseResult_DisplayETHPersonalMessage c_res =
        eth_parse_personal_message((PtrUR)ur_data, (PtrString)xpub_str);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("eth_parse_personal_message returned NULL"));
    }

    mp_obj_eth_parsed_personal_message_t* py_res = m_new_obj(mp_obj_eth_parsed_personal_message_t);
    py_res->base.type = &eth_parsed_personal_message_type;
    py_res->c_res = c_res;

    return MP_OBJ_FROM_PTR(py_res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_eth_parse_personal_message_obj, mp_eth_parse_personal_message);

typedef struct _mp_obj_eth_parsed_typed_data_t {
    mp_obj_base_t base;
    PtrT_TransactionParseResult_DisplayETHTypedData c_res;
} mp_obj_eth_parsed_typed_data_t;

STATIC mp_obj_t eth_parsed_typed_data_close(mp_obj_t self_in) {
    mp_obj_eth_parsed_typed_data_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res != NULL) {
        free_TransactionParseResult_DisplayETHTypedData(self->c_res);
        self->c_res = NULL;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_close_obj, eth_parsed_typed_data_close);

STATIC PtrT_TransactionParseResult_DisplayETHTypedData get_eth_parsed_typed_data(mp_obj_t self_in) {
    mp_obj_eth_parsed_typed_data_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("DisplayETHTypedData freed"));
    }
    return self->c_res;
}

STATIC mp_obj_t eth_parsed_typed_data_name(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_str(c->data->name, strlen(c->data->name));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_name_obj, eth_parsed_typed_data_name);

STATIC mp_obj_t eth_parsed_typed_data_version(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_str(c->data->version, strlen(c->data->version));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_version_obj, eth_parsed_typed_data_version);

STATIC mp_obj_t eth_parsed_typed_data_chain_id(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_str(c->data->chain_id, strlen(c->data->chain_id));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_chain_id_obj, eth_parsed_typed_data_chain_id);

STATIC mp_obj_t eth_parsed_typed_data_verifying_contract(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_str(c->data->verifying_contract, strlen(c->data->verifying_contract));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(
    eth_parsed_typed_data_verifying_contract_obj, eth_parsed_typed_data_verifying_contract
);

STATIC mp_obj_t eth_parsed_typed_data_salt(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_str(c->data->salt, strlen(c->data->salt));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_salt_obj, eth_parsed_typed_data_salt);

STATIC mp_obj_t eth_parsed_typed_data_primary_type(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_str(c->data->primary_type, strlen(c->data->primary_type));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_primary_type_obj, eth_parsed_typed_data_primary_type);

STATIC mp_obj_t eth_parsed_typed_data_message(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_str(c->data->message, strlen(c->data->message));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_message_obj, eth_parsed_typed_data_message);

STATIC mp_obj_t eth_parsed_typed_data_from(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_str(c->data->from, strlen(c->data->from));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_from_obj, eth_parsed_typed_data_from);

STATIC mp_obj_t eth_parsed_typed_data_domain_hash(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_str(c->data->domain_hash, strlen(c->data->domain_hash));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_domain_hash_obj, eth_parsed_typed_data_domain_hash);

STATIC mp_obj_t eth_parsed_typed_data_message_hash(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_str(c->data->message_hash, strlen(c->data->message_hash));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_message_hash_obj, eth_parsed_typed_data_message_hash);

STATIC mp_obj_t eth_parsed_typed_data_safe_tx_hash(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_str(c->data->safe_tx_hash, strlen(c->data->safe_tx_hash));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_safe_tx_hash_obj, eth_parsed_typed_data_safe_tx_hash);

STATIC mp_obj_t eth_parsed_typed_data_error_code(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_int_from_uint(c->error_code);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_error_code_obj, eth_parsed_typed_data_error_code);

STATIC mp_obj_t eth_parsed_typed_data_error_message(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    if (c->error_message == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(c->error_message, strlen(c->error_message));
}
STATIC
MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_error_message_obj, eth_parsed_typed_data_error_message);

STATIC mp_obj_t eth_parsed_typed_data_raw_message(mp_obj_t self_in) {
    PtrT_TransactionParseResult_DisplayETHTypedData c = get_eth_parsed_typed_data(self_in);
    return mp_obj_new_str(c->data->raw_message, strlen(c->data->raw_message));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(eth_parsed_typed_data_raw_message_obj, eth_parsed_typed_data_raw_message);

/// class EthParsedTypedData:
///     """The parsed Ethereum typed data."""
///     def close(self) -> None:
///         """Close the parsed Ethereum typed data and free the memory."""
///         ...
///     def free(self) -> None:
///         """Free the parsed Ethereum typed data."""
///         ...
///     def name(self) -> str:
///         """Get the name of the typed data."""
///         ...
///     def version(self) -> str:
///         """Get the version of the typed data."""
///         ...
///     def chain_id(self) -> int:
///         """Get the chain ID of the typed data."""
///         ...
///     def verifying_contract(self) -> str:
///         """Get the verifying contract of the typed data."""
///         ...
///     def salt(self) -> str:
///         """Get the salt of the typed data."""
///         ...
///     def primary_type(self) -> str:
///         """Get the primary type of the typed data."""
///         ...
///     def message(self) -> str:
///         """Get the UTF-8 message of the typed data."""
///         ...
///     def domain_hash(self) -> str:
///         """Get the domain hash of the typed data."""
///         ...
///     def message_hash(self) -> str:
///         """Get the message hash of the typed data."""
///         ...
///     def safe_tx_hash(self) -> str:
///         """Get the safe transaction hash of the typed data."""
///         ...
///     def error_code(self) -> int:
///         """Get the error code of the typed data."""
///         ...
///     def error_message(self) -> str:
///         """Get the error message of the typed data."""
///         ...
///     def raw_message(self) -> str:
///         """Get the raw message of the typed data."""
///         ...
STATIC const mp_rom_map_elem_t eth_parsed_typed_data_locals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&eth_parsed_typed_data_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_free), MP_ROM_PTR(&eth_parsed_typed_data_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_name), MP_ROM_PTR(&eth_parsed_typed_data_name_obj)},
    {MP_ROM_QSTR(MP_QSTR_version), MP_ROM_PTR(&eth_parsed_typed_data_version_obj)},
    {MP_ROM_QSTR(MP_QSTR_chain_id), MP_ROM_PTR(&eth_parsed_typed_data_chain_id_obj)},
    {MP_ROM_QSTR(MP_QSTR_verifying_contract), MP_ROM_PTR(&eth_parsed_typed_data_verifying_contract_obj)},
    {MP_ROM_QSTR(MP_QSTR_salt), MP_ROM_PTR(&eth_parsed_typed_data_salt_obj)},
    {MP_ROM_QSTR(MP_QSTR_primary_type), MP_ROM_PTR(&eth_parsed_typed_data_primary_type_obj)},
    {MP_ROM_QSTR(MP_QSTR_message), MP_ROM_PTR(&eth_parsed_typed_data_message_obj)},
    {MP_ROM_QSTR(MP_QSTR_from), MP_ROM_PTR(&eth_parsed_typed_data_from_obj)},
    {MP_ROM_QSTR(MP_QSTR_domain_hash), MP_ROM_PTR(&eth_parsed_typed_data_domain_hash_obj)},
    {MP_ROM_QSTR(MP_QSTR_message_hash), MP_ROM_PTR(&eth_parsed_typed_data_message_hash_obj)},
    {MP_ROM_QSTR(MP_QSTR_safe_tx_hash), MP_ROM_PTR(&eth_parsed_typed_data_safe_tx_hash_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_code), MP_ROM_PTR(&eth_parsed_typed_data_error_code_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_message), MP_ROM_PTR(&eth_parsed_typed_data_error_message_obj)},
    {MP_ROM_QSTR(MP_QSTR_raw_message), MP_ROM_PTR(&eth_parsed_typed_data_raw_message_obj)},
};
STATIC MP_DEFINE_CONST_DICT(eth_parsed_typed_data_locals_dict, eth_parsed_typed_data_locals_table);

STATIC const mp_obj_type_t eth_parsed_typed_data_type = {
    {&mp_type_type},
    .name = MP_QSTR_EthParsedTypedData,
    .locals_dict = (mp_obj_dict_t*)&eth_parsed_typed_data_locals_dict,
};

/// mock:global
/// def eth_parse_typed_data(ur: ResultContext, xpub: str) -> EthParsedTypedData:
///     """Parse the Ethereum typed data."""
///     ...
STATIC mp_obj_t mp_eth_parse_typed_data(mp_obj_t ur_in, mp_obj_t xpub_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    const char* xpub_str = mp_obj_str_get_str(xpub_in);

    PtrT_TransactionParseResult_DisplayETHTypedData c_res =
        eth_parse_typed_data((PtrUR)ur_data, (PtrString)xpub_str);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("eth_parse_typed_data returned NULL"));
    }

    mp_obj_eth_parsed_typed_data_t* py_res = m_new_obj(mp_obj_eth_parsed_typed_data_t);
    py_res->base.type = &eth_parsed_typed_data_type;
    py_res->c_res = c_res;

    return MP_OBJ_FROM_PTR(py_res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_eth_parse_typed_data_obj, mp_eth_parse_typed_data);

/// mock:global
/// def eth_ur_encode_signature(ur: EncodeResultContext, signature: str, origin: str) -> UREncodeResult:
///     """Encode the signature signature."""
///     ...
STATIC mp_obj_t mp_eth_ur_encode_signature(mp_obj_t ur_in, mp_obj_t signature_in, mp_obj_t origin_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    size_t signature_len;
    const uint8_t* signature_data = (const uint8_t*)mp_obj_str_get_data(signature_in, &signature_len);

    const char* origin_str = mp_obj_str_get_str(origin_in);

    PtrT_UREncodeResult c_res = eth_ur_encode_signature(
        (PtrUR)ur_data, (PtrBytes)signature_data, signature_len, (PtrString)origin_str
    );
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("eth_ur_encode_signature returned NULL"));
    }

    mp_obj_ur_encode_result_t* py_res = m_new_obj(mp_obj_ur_encode_result_t);
    py_res->base.type = &ur_encode_result_type;
    py_res->c_res = c_res;

    return MP_OBJ_FROM_PTR(py_res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mp_eth_ur_encode_signature_obj, mp_eth_ur_encode_signature);

/// mock:global
/// def eth_get_current_ur_path(ur: ResultContext) -> str:
///     """Get the current UR path."""
///     ...
STATIC mp_obj_t mp_eth_get_current_ur_path(mp_obj_t ur_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    PtrString c_res = eth_get_current_ur_path((PtrUR)ur_data);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("eth_get_current_ur_path returned NULL"));
    }

    mp_obj_t obj = mp_obj_new_str(c_res, strlen(c_res));
    free_ptr_string(c_res);

    return obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_eth_get_current_ur_path_obj, mp_eth_get_current_ur_path);

/// mock:global
/// def hpx_app_call_get_payload(ur: ResultContext) -> str:
///     """Get the payload of the HPX app call."""
///     ...
STATIC mp_obj_t mp_hpx_app_call_get_payload(mp_obj_t ur_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    PtrString payload = hpx_app_call_get_payload((PtrUR)ur_data);
    if (strlen(payload) == 0) {
        free_ptr_string(payload);
        mp_raise_ValueError(MP_ERROR_TEXT("hpx_app_call_get_payload returned NULL"));
    }
    mp_obj_t obj = mp_obj_new_str(payload, strlen(payload));
    free_ptr_string(payload);
    return obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_hpx_app_call_get_payload_obj, mp_hpx_app_call_get_payload);

/// mock:global
/// def eth_sign_tx(ur: ResultContext, seed: bytes) -> UREncodeResult:
///     """Sign the transaction."""
///     ...
STATIC mp_obj_t mp_eth_sign_tx(mp_obj_t ur_in, mp_obj_t seed_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    size_t seed_len;
    const uint8_t* seed_data = (const uint8_t*)mp_obj_str_get_data(seed_in, &seed_len);

    PtrT_UREncodeResult c_res = eth_sign_tx((PtrUR)ur_data, (PtrBytes)seed_data, seed_len);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("eth_sign_tx returned NULL"));
    }

    mp_obj_ur_encode_result_t* py_res = m_new_obj(mp_obj_ur_encode_result_t);
    py_res->base.type = &ur_encode_result_type;
    py_res->c_res = c_res;

    return MP_OBJ_FROM_PTR(py_res);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_eth_sign_tx_obj, mp_eth_sign_tx);

//////////////////////////////////////////////////End EVM functions//////////////////////////////
//////////////////////////////////////////////////Sol functions//////////////////////////////////

/// mock:global
/// def solana_check(ur: ResultContext, mfp: int) -> int:
///     """Check the Solana transaction."""
///     ...
STATIC mp_obj_t mp_solana_check(mp_obj_t ur_in, mp_obj_t mfp_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    uint32_t mfp_int = mp_obj_get_int_truncated(mfp_in);
    if (mfp_int < 0 || mfp_int > 0xFFFFFFFFU) {
        mp_raise_ValueError(MP_ERROR_TEXT("mfp value out of range for uint32_t"));
    }
    uint8_t mfp_bytes[4] = {0};
    mfp_bytes[0] = (mfp_int >> 24) & 0xFF;
    mfp_bytes[1] = (mfp_int >> 16) & 0xFF;
    mfp_bytes[2] = (mfp_int >> 8) & 0xFF;
    mfp_bytes[3] = mfp_int & 0xFF;

    PtrT_TransactionCheckResult c_res = solana_check((PtrUR)ur_data, (PtrBytes)mfp_bytes, 4);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("solana_check returned NULL"));
    }

    uint32_t error_code = c_res->error_code;
    free_TransactionCheckResult(c_res);
    return mp_obj_new_int_from_uint(error_code);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_solana_check_obj, mp_solana_check);

/// mock:global
/// def solana_get_path(ur: ResultContext) -> str:
///     """Get the Solana path."""
///     ...
STATIC mp_obj_t mp_solana_get_path(mp_obj_t ur_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    PtrString c_res = sol_get_path((PtrUR)ur_data);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("sol_get_path returned NULL"));
    }

    mp_obj_t obj = mp_obj_new_str(c_res, strlen(c_res));
    free_ptr_string(c_res);

    return obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_solana_get_path_obj, mp_solana_get_path);

/// mock:global
/// def solana_parse_tx_raw(ur: ResultContext) -> str:
///     """Parse the Solana transaction raw."""
///     ...
STATIC mp_obj_t mp_solana_parse_tx_raw(mp_obj_t ur_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    PtrString c_res = solana_parse_tx_raw((PtrUR)ur_data);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("solana_parse_tx_raw returned NULL"));
    }

    mp_obj_t obj = mp_obj_new_str(c_res, strlen(c_res));
    free_ptr_string(c_res);

    return obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_solana_parse_tx_raw_obj, mp_solana_parse_tx_raw);

/// mock:global
/// def solana_ur_encode_signature(ur: EncodeResultContext, signature: bytes) -> UREncodeResult:
///     """Encode the signature."""
///     ...
STATIC mp_obj_t mp_solana_ur_encode_signature(mp_obj_t ur_in, mp_obj_t signature_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    size_t signature_len;
    const uint8_t* signature_data = (const uint8_t*)mp_obj_str_get_data(signature_in, &signature_len);

    PtrT_UREncodeResult c_res =
        solana_ur_encode_signature((PtrUR)ur_data, (PtrBytes)signature_data, signature_len);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("solana_ur_encode_signature returned NULL"));
    }

    mp_obj_ur_encode_result_t* py_res = m_new_obj(mp_obj_ur_encode_result_t);
    py_res->base.type = &ur_encode_result_type;
    py_res->c_res = c_res;

    return MP_OBJ_FROM_PTR(py_res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_solana_ur_encode_signature_obj, mp_solana_ur_encode_signature);

//////////////////////////////////////////////////End Sol functions//////////////////////////////
//////////////////////////////////////////////////btc functions//////////////////////////////////

typedef struct _mp_obj_display_tx_detail_output_t {
    mp_obj_base_t base;
    char* address;
    char* amount;
    bool is_mine;
    bool is_external;
} mp_obj_display_tx_detail_output_t;

STATIC const mp_obj_type_t display_tx_detail_output_type;

STATIC mp_obj_t display_tx_detail_output_address(mp_obj_t self_in) {
    mp_obj_display_tx_detail_output_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->address == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->address, strlen(self->address));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_output_address_obj, display_tx_detail_output_address);

STATIC mp_obj_t display_tx_detail_output_amount(mp_obj_t self_in) {
    mp_obj_display_tx_detail_output_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->amount == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->amount, strlen(self->amount));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_output_amount_obj, display_tx_detail_output_amount);

STATIC mp_obj_t display_tx_detail_output_is_mine(mp_obj_t self_in) {
    mp_obj_display_tx_detail_output_t* self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->is_mine);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_output_is_mine_obj, display_tx_detail_output_is_mine);

STATIC mp_obj_t display_tx_detail_output_is_external(mp_obj_t self_in) {
    mp_obj_display_tx_detail_output_t* self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->is_external);
}
STATIC
MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_output_is_external_obj, display_tx_detail_output_is_external);

/// class DisplayTxDetailOutput:
///     """Display the BTC output detail."""
///     def address(self) -> str:
///         """Get the address."""
///         ...
///     def amount(self) -> str:
///         """Get the amount."""
///         ...
///     def is_mine(self) -> bool:
///         """Get the is mine."""
///         ...
///     def is_external(self) -> bool:
///         """Get the is external."""
///         ...
STATIC const mp_rom_map_elem_t display_tx_detail_output_locals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_address), MP_ROM_PTR(&display_tx_detail_output_address_obj)},
    {MP_ROM_QSTR(MP_QSTR_amount), MP_ROM_PTR(&display_tx_detail_output_amount_obj)},
    {MP_ROM_QSTR(MP_QSTR_is_mine), MP_ROM_PTR(&display_tx_detail_output_is_mine_obj)},
    {MP_ROM_QSTR(MP_QSTR_is_external), MP_ROM_PTR(&display_tx_detail_output_is_external_obj)},
};
STATIC MP_DEFINE_CONST_DICT(display_tx_detail_output_locals_dict, display_tx_detail_output_locals_table);

STATIC const mp_obj_type_t display_tx_detail_output_type = {
    {&mp_type_type},
    .name = MP_QSTR_DisplayTxDetailOutput,
    .locals_dict = (mp_obj_dict_t*)&display_tx_detail_output_locals_dict,
};

STATIC mp_obj_t display_tx_detail_output_new(DisplayTxDetailOutput* c_output) {
    if (c_output == NULL) {
        return mp_const_none;
    }
    mp_obj_display_tx_detail_output_t* o = m_new_obj(mp_obj_display_tx_detail_output_t);
    o->base.type = &display_tx_detail_output_type;
    o->address = m_new(char, strlen(c_output->address) + 1);
    strcpy(o->address, c_output->address);
    o->amount = m_new(char, strlen(c_output->amount) + 1);
    strcpy(o->amount, c_output->amount);
    o->is_mine = c_output->is_mine;
    o->is_external = c_output->is_external;
    return MP_OBJ_FROM_PTR(o);
}

typedef struct _mp_obj_display_tx_detail_input_t {
    mp_obj_base_t base;
    char* address;
    char* amount;
    bool is_mine;
    bool is_external;
} mp_obj_display_tx_detail_input_t;

STATIC const mp_obj_type_t display_tx_detail_input_type;

STATIC mp_obj_t display_tx_detail_input_address(mp_obj_t self_in) {
    mp_obj_display_tx_detail_input_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->address == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->address, strlen(self->address));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_input_address_obj, display_tx_detail_input_address);

STATIC mp_obj_t display_tx_detail_input_amount(mp_obj_t self_in) {
    mp_obj_display_tx_detail_input_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->amount == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->amount, strlen(self->amount));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_input_amount_obj, display_tx_detail_input_amount);

STATIC mp_obj_t display_tx_detail_input_is_mine(mp_obj_t self_in) {
    mp_obj_display_tx_detail_input_t* self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->is_mine);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_input_is_mine_obj, display_tx_detail_input_is_mine);

STATIC mp_obj_t display_tx_detail_input_is_external(mp_obj_t self_in) {
    mp_obj_display_tx_detail_input_t* self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->is_external);
}
STATIC
MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_input_is_external_obj, display_tx_detail_input_is_external);

/// class DisplayTxDetailInput:
///     """Display the BTC input detail."""
///     def address(self) -> str:
///         """Get the address."""
///         ...
///     def amount(self) -> str:
///         """Get the amount."""
///         ...
///     def is_mine(self) -> bool:
///         """Get the is mine."""
///         ...
///     def is_external(self) -> bool:
///         """Get the is external."""
///         ...
STATIC const mp_rom_map_elem_t display_tx_detail_input_locals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_address), MP_ROM_PTR(&display_tx_detail_input_address_obj)},
    {MP_ROM_QSTR(MP_QSTR_amount), MP_ROM_PTR(&display_tx_detail_input_amount_obj)},
    {MP_ROM_QSTR(MP_QSTR_is_mine), MP_ROM_PTR(&display_tx_detail_input_is_mine_obj)},
    {MP_ROM_QSTR(MP_QSTR_is_external), MP_ROM_PTR(&display_tx_detail_input_is_external_obj)},
};
STATIC MP_DEFINE_CONST_DICT(display_tx_detail_input_locals_dict, display_tx_detail_input_locals_table);

STATIC const mp_obj_type_t display_tx_detail_input_type = {
    {&mp_type_type},
    .name = MP_QSTR_DisplayTxDetailInput,
    .locals_dict = (mp_obj_dict_t*)&display_tx_detail_input_locals_dict,
};

STATIC mp_obj_t display_tx_detail_input_new(DisplayTxDetailInput* c_input) {
    if (c_input == NULL) {
        return mp_const_none;
    }
    mp_obj_display_tx_detail_input_t* o = m_new_obj(mp_obj_display_tx_detail_input_t);
    o->base.type = &display_tx_detail_input_type;
    o->address = m_new(char, strlen(c_input->address) + 1);
    strcpy(o->address, c_input->address);
    o->amount = m_new(char, strlen(c_input->amount) + 1);
    strcpy(o->amount, c_input->amount);
    o->is_mine = c_input->is_mine;
    o->is_external = c_input->is_external;
    return MP_OBJ_FROM_PTR(o);
}

typedef struct _mp_obj_display_tx_detail_t {
    mp_obj_base_t base;
    char* total_input_amount;
    char* total_output_amount;
    char* fee_amount;
    mp_obj_t from; // list of DisplayTxDetailInput
    mp_obj_t to;   // list of DisplayTxDetailOutput
    char* network;
    char* total_input_sat;
    char* total_output_sat;
    char* fee_sat;
    char* sign_status;
    uint32_t error_code;
    char* error_message;
} mp_obj_display_tx_detail_t;

STATIC const mp_obj_type_t display_tx_detail_type;

STATIC mp_obj_t display_tx_detail_total_input_amount(mp_obj_t self_in) {
    mp_obj_display_tx_detail_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->total_input_amount == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->total_input_amount, strlen(self->total_input_amount));
}
STATIC
MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_total_input_amount_obj, display_tx_detail_total_input_amount);

STATIC mp_obj_t display_tx_detail_total_output_amount(mp_obj_t self_in) {
    mp_obj_display_tx_detail_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->total_output_amount == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->total_output_amount, strlen(self->total_output_amount));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(
    display_tx_detail_total_output_amount_obj, display_tx_detail_total_output_amount
);

STATIC mp_obj_t display_tx_detail_fee_amount(mp_obj_t self_in) {
    mp_obj_display_tx_detail_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->fee_amount == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->fee_amount, strlen(self->fee_amount));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_fee_amount_obj, display_tx_detail_fee_amount);

STATIC mp_obj_t display_tx_detail_from(mp_obj_t self_in) {
    mp_obj_display_tx_detail_t* self = MP_OBJ_TO_PTR(self_in);
    return self->from;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_from_obj, display_tx_detail_from);

STATIC mp_obj_t display_tx_detail_to(mp_obj_t self_in) {
    mp_obj_display_tx_detail_t* self = MP_OBJ_TO_PTR(self_in);
    return self->to;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_to_obj, display_tx_detail_to);

STATIC mp_obj_t display_tx_detail_network(mp_obj_t self_in) {
    mp_obj_display_tx_detail_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->network == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->network, strlen(self->network));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_network_obj, display_tx_detail_network);

STATIC mp_obj_t display_tx_detail_total_input_sat(mp_obj_t self_in) {
    mp_obj_display_tx_detail_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->total_input_sat == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->total_input_sat, strlen(self->total_input_sat));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_total_input_sat_obj, display_tx_detail_total_input_sat);

STATIC mp_obj_t display_tx_detail_total_output_sat(mp_obj_t self_in) {
    mp_obj_display_tx_detail_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->total_output_sat == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->total_output_sat, strlen(self->total_output_sat));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_total_output_sat_obj, display_tx_detail_total_output_sat);

STATIC mp_obj_t display_tx_detail_fee_sat(mp_obj_t self_in) {
    mp_obj_display_tx_detail_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->fee_sat == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->fee_sat, strlen(self->fee_sat));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_fee_sat_obj, display_tx_detail_fee_sat);

STATIC mp_obj_t display_tx_detail_sign_status(mp_obj_t self_in) {
    mp_obj_display_tx_detail_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->sign_status == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->sign_status, strlen(self->sign_status));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_sign_status_obj, display_tx_detail_sign_status);

STATIC mp_obj_t display_tx_detail_error_code(mp_obj_t self_in) {
    mp_obj_display_tx_detail_t* self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(self->error_code);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_error_code_obj, display_tx_detail_error_code);

STATIC mp_obj_t display_tx_detail_error_message(mp_obj_t self_in) {
    mp_obj_display_tx_detail_t* self = MP_OBJ_TO_PTR(self_in);
    if (self->error_message == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(self->error_message, strlen(self->error_message));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(display_tx_detail_error_message_obj, display_tx_detail_error_message);

/// class DisplayTxDetail:
///     """Display the BTC transaction detail."""
///     def total_input_amount(self) -> str:
///         """Get the total input amount."""
///         ...
///     def total_output_amount(self) -> str:
///         """Get the total output amount."""
///         ...
///     def fee_amount(self) -> str:
///         """Get the fee amount."""
///         ...
///     def inputs(self) -> list[DisplayTxDetailInput]:
///         """Get the inputs."""
///         ...
///     def outputs(self) -> list[DisplayTxDetailOutput]:
///         """Get the outputs."""
///         ...
///     def network(self) -> str:
///         """Get the network."""
///         ...
///     def total_input_sat(self) -> str:
///         """Get the total input satoshis."""
///         ...
///     def total_output_sat(self) -> str:
///         """Get the total output satoshis."""
///         ...
///     def fee_sat(self) -> str:
///         """Get the fee satoshis."""
///         ...
///     def sign_status(self) -> str:
///         """Get the sign status."""
///         ...
///     def error_code(self) -> int:
///         """Get the error code."""
///         ...
///     def error_message(self) -> str:
///         """Get the error message."""
///         ...
STATIC const mp_rom_map_elem_t display_tx_detail_locals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_total_input_amount), MP_ROM_PTR(&display_tx_detail_total_input_amount_obj)},
    {MP_ROM_QSTR(MP_QSTR_total_output_amount), MP_ROM_PTR(&display_tx_detail_total_output_amount_obj)},
    {MP_ROM_QSTR(MP_QSTR_fee_amount), MP_ROM_PTR(&display_tx_detail_fee_amount_obj)},
    {MP_ROM_QSTR(MP_QSTR_inputs), MP_ROM_PTR(&display_tx_detail_from_obj)},
    {MP_ROM_QSTR(MP_QSTR_outputs), MP_ROM_PTR(&display_tx_detail_to_obj)},
    {MP_ROM_QSTR(MP_QSTR_network), MP_ROM_PTR(&display_tx_detail_network_obj)},
    {MP_ROM_QSTR(MP_QSTR_total_input_sat), MP_ROM_PTR(&display_tx_detail_total_input_sat_obj)},
    {MP_ROM_QSTR(MP_QSTR_total_output_sat), MP_ROM_PTR(&display_tx_detail_total_output_sat_obj)},
    {MP_ROM_QSTR(MP_QSTR_fee_sat), MP_ROM_PTR(&display_tx_detail_fee_sat_obj)},
    {MP_ROM_QSTR(MP_QSTR_sign_status), MP_ROM_PTR(&display_tx_detail_sign_status_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_code), MP_ROM_PTR(&display_tx_detail_error_code_obj)},
    {MP_ROM_QSTR(MP_QSTR_error_message), MP_ROM_PTR(&display_tx_detail_error_message_obj)},
};
STATIC MP_DEFINE_CONST_DICT(display_tx_detail_locals_dict, display_tx_detail_locals_table);

STATIC const mp_obj_type_t display_tx_detail_type = {
    {&mp_type_type},
    .name = MP_QSTR_DisplayTxDetail,
    .locals_dict = (mp_obj_dict_t*)&display_tx_detail_locals_dict,
};

STATIC mp_obj_t display_tx_detail_new(struct TransactionParseResult_DisplayTx* result) {
    if (result == NULL) {
        return mp_const_none;
    }
    mp_obj_display_tx_detail_t* o = m_new_obj(mp_obj_display_tx_detail_t);
    o->base.type = &display_tx_detail_type;

    o->total_input_amount = m_new(char, strlen(result->data->detail->total_input_amount) + 1);
    strcpy(o->total_input_amount, result->data->detail->total_input_amount);

    o->total_output_amount = m_new(char, strlen(result->data->detail->total_output_amount) + 1);
    strcpy(o->total_output_amount, result->data->detail->total_output_amount);

    o->fee_amount = m_new(char, strlen(result->data->detail->fee_amount) + 1);
    strcpy(o->fee_amount, result->data->detail->fee_amount);

    o->network = m_new(char, strlen(result->data->detail->network) + 1);
    strcpy(o->network, result->data->detail->network);

    o->total_input_sat = m_new(char, strlen(result->data->detail->total_input_sat) + 1);
    strcpy(o->total_input_sat, result->data->detail->total_input_sat);

    o->total_output_sat = m_new(char, strlen(result->data->detail->total_output_sat) + 1);
    strcpy(o->total_output_sat, result->data->detail->total_output_sat);

    o->fee_sat = m_new(char, strlen(result->data->detail->fee_sat) + 1);
    strcpy(o->fee_sat, result->data->detail->fee_sat);

    o->sign_status = m_new(char, strlen(result->data->detail->sign_status) + 1);
    strcpy(o->sign_status, result->data->detail->sign_status);

    o->from = mp_obj_new_list(0, NULL);
    o->to = mp_obj_new_list(0, NULL);

    for (size_t i = 0; i < result->data->detail->from->size; i++) {
        mp_obj_list_append(o->from, display_tx_detail_input_new(&result->data->detail->from->data[i]));
    }

    for (size_t i = 0; i < result->data->detail->to->size; i++) {
        mp_obj_list_append(o->to, display_tx_detail_output_new(&result->data->detail->to->data[i]));
    }

    o->error_code = result->error_code;
    if (result->error_message != NULL) {
        o->error_message = m_new(char, strlen(result->error_message) + 1);
        strcpy(o->error_message, result->error_message);
    } else {
        o->error_message = NULL;
    }
    return MP_OBJ_FROM_PTR(o);
}

/// mock:global
/// def btc_parse_psbt(ur_in: ResultContext, mfp: int, public_keys: list[tuple[str, str]]) -> DisplayTxDetail:
///     """Parse the PSBT."""
///     ...
STATIC mp_obj_t mp_btc_parse_psbt(mp_obj_t ur_in, mp_obj_t mfp_in, mp_obj_t public_keys_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    uint32_t mfp_int = mp_obj_get_int_truncated(mfp_in);
    if (mfp_int < 0 || mfp_int > 0xFFFFFFFFU) {
        mp_raise_ValueError(MP_ERROR_TEXT("mfp value out of range for uint32_t"));
    }
    uint8_t mfp_bytes[4] = {0};
    mfp_bytes[0] = (mfp_int >> 24) & 0xFF;
    mfp_bytes[1] = (mfp_int >> 16) & 0xFF;
    mfp_bytes[2] = (mfp_int >> 8) & 0xFF;
    mfp_bytes[3] = mfp_int & 0xFF;

    if (!mp_obj_is_type(public_keys_in, &mp_type_list)) {
        mp_raise_ValueError(MP_ERROR_TEXT("public_keys must be a list"));
    }
    size_t num_public_keys = MP_OBJ_SMALL_INT_VALUE(mp_obj_len(public_keys_in));
    ExtendedPublicKey* c_public_keys = m_new(ExtendedPublicKey, num_public_keys);
    mp_obj_t* items;
    mp_obj_list_get(public_keys_in, &num_public_keys, &items);
    for (size_t i = 0; i < num_public_keys; i++) {
        mp_obj_t item = items[i];
        mp_obj_t path_obj, xpub_obj;
        mp_obj_tuple_get(item, NULL, NULL);
        size_t tuple_len;
        mp_obj_t* tuple_items;
        mp_obj_tuple_get(item, &tuple_len, &tuple_items);
        if (tuple_len != 2) {
            mp_raise_ValueError(MP_ERROR_TEXT("each public_key must be a tuple of (path, xpub)"));
        }
        path_obj = tuple_items[0];
        xpub_obj = tuple_items[1];
        c_public_keys[i].path = (PtrString)mp_obj_str_get_str(path_obj);
        c_public_keys[i].xpub = (PtrString)mp_obj_str_get_str(xpub_obj);
        LOG_DEBUG(MODULE, "Public key %d: path=%s, xpub=%s", i, c_public_keys[i].path, c_public_keys[i].xpub);
    }
    CSliceFFI_ExtendedPublicKey public_keys_slice;
    public_keys_slice.data = c_public_keys;
    public_keys_slice.size = num_public_keys;

    PtrT_TransactionParseResult_DisplayTx result =
        btc_parse_psbt((PtrUR)ur_data, (PtrBytes)mfp_bytes, 4, &public_keys_slice, NULL);
    m_free(c_public_keys);
    if (result == NULL || result->error_code != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("btc_parse_psbt returned NULL or error"));
    }

    mp_obj_t detail = display_tx_detail_new(result);
    free_TransactionParseResult_DisplayTx(result);
    return detail;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(mp_btc_parse_psbt_obj, mp_btc_parse_psbt);

/// mock:global
/// def btc_check_psbt(ur_in: ResultContext, mfp: int, public_keys: list[tuple[str, str]]) -> bool:
///     """Check the PSBT."""
///     ...
STATIC mp_obj_t mp_btc_check_psbt(mp_obj_t ur_in, mp_obj_t mfp_in, mp_obj_t public_keys_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    uint32_t mfp_int = mp_obj_get_int_truncated(mfp_in);
    if (mfp_int < 0 || mfp_int > 0xFFFFFFFFU) {
        mp_raise_ValueError(MP_ERROR_TEXT("mfp value out of range for uint32_t"));
    }
    uint8_t mfp_bytes[4] = {0};
    mfp_bytes[0] = (mfp_int >> 24) & 0xFF;
    mfp_bytes[1] = (mfp_int >> 16) & 0xFF;
    mfp_bytes[2] = (mfp_int >> 8) & 0xFF;
    mfp_bytes[3] = mfp_int & 0xFF;

    if (!mp_obj_is_type(public_keys_in, &mp_type_list)) {
        mp_raise_ValueError(MP_ERROR_TEXT("public_keys must be a list"));
    }
    size_t num_public_keys = MP_OBJ_SMALL_INT_VALUE(mp_obj_len(public_keys_in));
    ExtendedPublicKey* c_public_keys = m_new(ExtendedPublicKey, num_public_keys);
    mp_obj_t* items;
    mp_obj_list_get(public_keys_in, &num_public_keys, &items);
    for (size_t i = 0; i < num_public_keys; i++) {
        mp_obj_t item = items[i];
        mp_obj_t path_obj, xpub_obj;
        mp_obj_tuple_get(item, NULL, NULL);
        size_t tuple_len;
        mp_obj_t* tuple_items;
        mp_obj_tuple_get(item, &tuple_len, &tuple_items);
        if (tuple_len != 2) {
            mp_raise_ValueError(MP_ERROR_TEXT("each public_key must be a tuple of (path, xpub)"));
        }
        path_obj = tuple_items[0];
        xpub_obj = tuple_items[1];
        c_public_keys[i].path = (PtrString)mp_obj_str_get_str(path_obj);
        c_public_keys[i].xpub = (PtrString)mp_obj_str_get_str(xpub_obj);
        LOG_DEBUG(MODULE, "Public key %d: path=%s, xpub=%s", i, c_public_keys[i].path, c_public_keys[i].xpub);
    }
    CSliceFFI_ExtendedPublicKey public_keys_slice;
    public_keys_slice.data = c_public_keys;
    public_keys_slice.size = num_public_keys;

    PtrT_TransactionCheckResult c_res =
        btc_check_psbt((PtrUR)ur_data, (PtrBytes)mfp_bytes, 4, &public_keys_slice, NULL, NULL);
    m_free(c_public_keys);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("btc_check_psbt returned NULL"));
    }

    uint32_t error_code = c_res->error_code;
    free_TransactionCheckResult(c_res);
    return mp_obj_new_int_from_uint(error_code);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(mp_btc_check_psbt_obj, mp_btc_check_psbt);

/// mock:global
/// def btc_sign_psbt(ur_in: ResultContext, seed: bytes, mfp: int) -> str:
///     """Sign the PSBT."""
///     ...
STATIC mp_obj_t mp_btc_sign_psbt(mp_obj_t ur_in, mp_obj_t seed_in, mp_obj_t mfp_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    size_t seed_len;
    const uint8_t* seed_data = (const uint8_t*)mp_obj_str_get_data(seed_in, &seed_len);

    uint32_t mfp_int = mp_obj_get_int_truncated(mfp_in);
    if (mfp_int < 0 || mfp_int > 0xFFFFFFFFU) {
        mp_raise_ValueError(MP_ERROR_TEXT("mfp value out of range for uint32_t"));
    }
    uint8_t mfp_bytes[4] = {0};
    mfp_bytes[0] = (mfp_int >> 24) & 0xFF;
    mfp_bytes[1] = (mfp_int >> 16) & 0xFF;
    mfp_bytes[2] = (mfp_int >> 8) & 0xFF;
    mfp_bytes[3] = mfp_int & 0xFF;

    PtrT_UREncodeResult c_res =
        btc_sign_psbt((PtrUR)ur_data, (PtrBytes)seed_data, seed_len, (PtrBytes)mfp_bytes, 4);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("btc_sign_psbt returned NULL"));
    }
    mp_obj_ur_encode_result_t* py_res = m_new_obj(mp_obj_ur_encode_result_t);
    py_res->base.type = &ur_encode_result_type;
    py_res->c_res = c_res;

    return MP_OBJ_FROM_PTR(py_res);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_3(mp_btc_sign_psbt_obj, mp_btc_sign_psbt);

//////////////////////////////////////////////////End btc functions//////////////////////////////

//////////////////////////////////////////////////tron functions//////////////////////////////

/// mock:global
/// def tron_parse_keystone_path(ur_in: ResultContext) -> str:
///     """Parse the keystone path."""
///     ...
STATIC mp_obj_t mp_tron_parse_keystone_path(mp_obj_t ur_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    SimpleResponse_c_char* c_res = tron_parse_keystone_path((PtrUR)ur_data, KeystoneSignRequest);
    if (c_res == NULL || c_res->error_code != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("tron_parse_keystone_path returned NULL"));
    }

    mp_obj_t obj = mp_obj_new_str(c_res->data, strlen(c_res->data));
    free_simple_response_c_char(c_res);
    return obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_tron_parse_keystone_path_obj, mp_tron_parse_keystone_path);

/// mock:global
/// def tron_check_keystone(ur_in: ResultContext, mfp: int, xpub: str) -> bool:
///     """Check the keystone."""
///     ...
STATIC mp_obj_t mp_tron_check_keystone(mp_obj_t ur_in, mp_obj_t mfp_in, mp_obj_t xpub_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    uint32_t mfp_int = mp_obj_get_int_truncated(mfp_in);
    if (mfp_int < 0 || mfp_int > 0xFFFFFFFFU) {
        mp_raise_ValueError(MP_ERROR_TEXT("mfp value out of range for uint32_t"));
    }
    LOG_DEBUG(MODULE, "mfp: %04x", mfp_int);
    uint8_t mfp_bytes[4] = {0};
    mfp_bytes[0] = (mfp_int >> 24) & 0xFF;
    mfp_bytes[1] = (mfp_int >> 16) & 0xFF;
    mfp_bytes[2] = (mfp_int >> 8) & 0xFF;
    mfp_bytes[3] = mfp_int & 0xFF;

    const char* xpub_str = mp_obj_str_get_str(xpub_in);
    PtrT_TransactionCheckResult c_res =
        tron_check_keystone((PtrUR)ur_data, KeystoneSignRequest, (PtrBytes)mfp_bytes, 4, (PtrString)xpub_str);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("tron_check_keystone returned NULL"));
    }
    uint32_t error_code = c_res->error_code;
    LOG_DEBUG(MODULE, "error_code: %d", error_code);
    LOG_DEBUG(MODULE, "error: %s", c_res->error_message);
    free_TransactionCheckResult(c_res);
    return mp_obj_new_int_from_uint(error_code);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mp_tron_check_keystone_obj, mp_tron_check_keystone);

/// mock:global
/// def tron_parse_keystone_raw(ur_in: ResultContext) -> str:
///     """Parse the keystone raw."""
///     ...
STATIC mp_obj_t mp_tron_parse_keystone_raw(mp_obj_t ur_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    SimpleResponse_c_char* c_res = tron_parse_keystone_raw((PtrUR)ur_data, KeystoneSignRequest);
    if (c_res == NULL || c_res->error_code != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("tron_parse_keystone_raw returned NULL"));
    }

    mp_obj_t obj = mp_obj_new_str(c_res->data, strlen(c_res->data));
    free_simple_response_c_char(c_res);
    return obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_tron_parse_keystone_raw_obj, mp_tron_parse_keystone_raw);

/// mock:global
/// def tron_ur_encode_signature(ur_in: ResultContext, signature: str) -> EncodeResult:
///     """Encode the signature."""
///     ...
STATIC mp_obj_t mp_tron_ur_encode_signature(mp_obj_t ur_in, mp_obj_t signature_in) {
    const void* ur_data = ur_data_from_obj(ur_in);
    if (ur_data == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("expected UR data handle"));
    }

    size_t signature_len;
    const uint8_t* signature_data = (const uint8_t*)mp_obj_str_get_data(signature_in, &signature_len);
    PtrT_UREncodeResult c_res =
        tron_encode_signature((PtrUR)ur_data, KeystoneSignRequest, (PtrBytes)signature_data, signature_len);
    if (c_res == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("tron_encode_signature returned NULL"));
    }
    mp_obj_ur_encode_result_t* py_res = m_new_obj(mp_obj_ur_encode_result_t);
    py_res->base.type = &ur_encode_result_type;
    py_res->c_res = c_res;
    return py_res;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_tron_ur_encode_signature_obj, mp_tron_ur_encode_signature);

//////////////////////////////////////////////////End tron functions//////////////////////////////

STATIC const mp_rom_map_elem_t ur_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ur_parser)},
    {MP_ROM_QSTR(MP_QSTR_URParseResult), MP_ROM_PTR(&ur_parse_result_type)},
    {MP_ROM_QSTR(MP_QSTR_URParseMultiResult), MP_ROM_PTR(&ur_parse_multi_result_type)},
    {MP_ROM_QSTR(MP_QSTR_DecoderHandle), MP_ROM_PTR(&decoder_handle_type)},
    {MP_ROM_QSTR(MP_QSTR_EncoderHandle), MP_ROM_PTR(&encoder_handle_type)},
    {MP_ROM_QSTR(MP_QSTR_DataHandle), MP_ROM_PTR(&data_handle_type)},
    {MP_ROM_QSTR(MP_QSTR_UREncodeResult), MP_ROM_PTR(&ur_encode_result_type)},
    {MP_ROM_QSTR(MP_QSTR_UREncodeMultiResult), MP_ROM_PTR(&ur_encode_multi_result_type)},
    {MP_ROM_QSTR(MP_QSTR_parse_ur), MP_ROM_PTR(&mp_parse_ur_obj)},
    {MP_ROM_QSTR(MP_QSTR_receive), MP_ROM_PTR(&mp_receive_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_next_part), MP_ROM_PTR(&ur_encode_multi_result_get_next_part_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_connect_metamask_ur), MP_ROM_PTR(&mp_get_connect_metamask_ur_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_connect_hpx_ur), MP_ROM_PTR(&mp_get_connect_hpx_ur_obj)},
    {MP_ROM_QSTR(MP_QSTR_hpx_app_call_get_payload), MP_ROM_PTR(&mp_hpx_app_call_get_payload_obj)},
    {MP_ROM_QSTR(MP_QSTR_EthParsedRaw), MP_ROM_PTR(&eth_parsed_raw_type)},
    {MP_ROM_QSTR(MP_QSTR_EthParsedPersonalMessage), MP_ROM_PTR(&eth_parsed_personal_message_type)},
    {MP_ROM_QSTR(MP_QSTR_EthParsedTypedData), MP_ROM_PTR(&eth_parsed_typed_data_type)},
    {MP_ROM_QSTR(MP_QSTR_eth_parse_raw), MP_ROM_PTR(&mp_eth_parse_raw_obj)},
    {MP_ROM_QSTR(MP_QSTR_eth_check), MP_ROM_PTR(&mp_eth_check_obj)},
    {MP_ROM_QSTR(MP_QSTR_eth_parse_personal_message), MP_ROM_PTR(&mp_eth_parse_personal_message_obj)},
    {MP_ROM_QSTR(MP_QSTR_eth_parse_typed_data), MP_ROM_PTR(&mp_eth_parse_typed_data_obj)},
    {MP_ROM_QSTR(MP_QSTR_eth_ur_encode_signature), MP_ROM_PTR(&mp_eth_ur_encode_signature_obj)},
    {MP_ROM_QSTR(MP_QSTR_eth_get_current_ur_path), MP_ROM_PTR(&mp_eth_get_current_ur_path_obj)},
    {MP_ROM_QSTR(MP_QSTR_eth_sign_tx), MP_ROM_PTR(&mp_eth_sign_tx_obj)},
    {MP_ROM_QSTR(MP_QSTR_solana_check), MP_ROM_PTR(&mp_solana_check_obj)},
    {MP_ROM_QSTR(MP_QSTR_solana_get_path), MP_ROM_PTR(&mp_solana_get_path_obj)},
    {MP_ROM_QSTR(MP_QSTR_solana_parse_tx_raw), MP_ROM_PTR(&mp_solana_parse_tx_raw_obj)},
    {MP_ROM_QSTR(MP_QSTR_solana_ur_encode_signature), MP_ROM_PTR(&mp_solana_ur_encode_signature_obj)},
    {MP_ROM_QSTR(MP_QSTR_btc_parse_psbt), MP_ROM_PTR(&mp_btc_parse_psbt_obj)},
    {MP_ROM_QSTR(MP_QSTR_btc_check_psbt), MP_ROM_PTR(&mp_btc_check_psbt_obj)},
    {MP_ROM_QSTR(MP_QSTR_btc_sign_psbt), MP_ROM_PTR(&mp_btc_sign_psbt_obj)},
    {MP_ROM_QSTR(MP_QSTR_tron_parse_keystone_path), MP_ROM_PTR(&mp_tron_parse_keystone_path_obj)},
    {MP_ROM_QSTR(MP_QSTR_tron_check_keystone), MP_ROM_PTR(&mp_tron_check_keystone_obj)},
    {MP_ROM_QSTR(MP_QSTR_tron_parse_keystone_raw), MP_ROM_PTR(&mp_tron_parse_keystone_raw_obj)},
    {MP_ROM_QSTR(MP_QSTR_tron_ur_encode_signature), MP_ROM_PTR(&mp_tron_ur_encode_signature_obj)},
};
STATIC MP_DEFINE_CONST_DICT(ur_module_globals, ur_module_globals_table);

const mp_obj_module_t ur_parser_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t*)&ur_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ur_parser, ur_parser_module);
