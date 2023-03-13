TOPNAME = top

# project source
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")

# CFLAGS += $(shell llvm-config --cxxflags) -fPIE -I $(NPC_HOME)/csrc/include
# LDFLAGS += -lSDL2 -lSDL2_image -lreadline -ldl $(shell llvm-config --libs)
CFLAGS +=  -fPIE -I $(NPC_HOME)/csrc/include
LDFLAGS +=  -lreadline -ldl $(shell llvm-config --libs)

# DIFF_REF_SO = $(NEMU_HOME)/build/riscv64-nemu-interpreter-so
# ARGS += -d $(DIFF_REF_SO)
IMG ?=

all:
	@echo "Write this Makefile by your self."

sim:  $(VSRCS) $(CSRCS) 
	verilator  --cc --exe --trace --build \
	--top-module $(TOPNAME) $^ \
	$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) 
	$(NPC_HOME)/obj_dir/Vtop $(IMG)
# $(NPC_HOME)/obj_dir/Vtop -d $(DIFF_REF_SO) $(IMG) 
# gtkwave wave.vcd

run: sim

test:
	echo $(IMG)

clean:
	rm -rf $(NPC_HOME)/obj_dir $(NPC_HOME)/wave.vcd

.PHONY: default all clean run

include ../Makefile
