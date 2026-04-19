BPF_CLANG ?= clang
BPF_HOST_TRIPLE ?= $(shell gcc -dumpmachine)
BPF_ARCH_INCLUDE ?= /usr/include/$(BPF_HOST_TRIPLE)
BPF_CFLAGS ?= -O2 -g -target bpf

ifneq ($(wildcard $(BPF_ARCH_INCLUDE)),)
BPF_CFLAGS += -I$(BPF_ARCH_INCLUDE)
endif

PROGRAMS := xdp_filter.o tc_monitor.o

all: $(PROGRAMS)

%.o: %.c
	$(BPF_CLANG) $(BPF_CFLAGS) -c $< -o $@

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
