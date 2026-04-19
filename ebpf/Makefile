BPF_CLANG ?= clang
BPF_CFLAGS ?= -O2 -g -target bpf

PROGRAMS := xdp_filter.o tc_monitor.o

all: $(PROGRAMS)

%.o: %.c
	$(BPF_CLANG) $(BPF_CFLAGS) -c $< -o $@

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
