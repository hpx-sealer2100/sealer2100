ifneq ($(V),1)
Q := @
endif


PROTO_NAMES = messages \
	messages-bootloader  \
	messages-common  \
	messages-fs \
	messages-management


PROTO_OPTIONS = $(PROTO_NAMES:=.options)
PROTO_COMPILED = $(PROTO_NAMES:=.pb)
PROTO_HEADERS = $(PROTO_COMPILED:=.h)

all: messages_map.h messages_map_limits.h messages.pb.h

PYTHON ?= python

# produces also all of $(PROTO_HEADERS)
messages.pb.h: $(PROTO_COMPILED) $(PROTO_OPTIONS)
	@printf "  NANOPB  $@\n"
	$(Q)nanopb_generator $(PROTO_COMPILED) \
		-L '#include "%s"' \
		-T -D . \
		-s "mangle_names:M_FLATTEN"
	$(Q)$(PYTHON) headers-of-pb.py $(PROTO_HEADERS)
# 	@grep " CALLBACK, " $(PROTO_HEADERS) && echo "CALLBACK field found"  && exit 1  || :

%.pb: %.proto
	@printf "  PROTOC  $@\n"
	@protoc -I/usr/include -I. $< -o $@

messages_%_pb2.py: messages-%.proto
	@printf "  PROTOC  $@\n"
	$(Q)protoc -I/usr/include -I. $< --python_out=.

%_pb2.py: %.proto
	@printf "  PROTOC  $@\n"
	$(Q)protoc -I/usr/include -I. $< --python_out=.

messages_map.h messages_map_limits.h: messages_map.py messages_pb2.py
	$(Q)$(PYTHON) $< ${SKIPPED_MESSAGES}

clean:
	rm -f *.pb *.o *.d *.pb.c *.pb.h *_pb2.py messages_map.h messages_map_limits.h messages-all.h
