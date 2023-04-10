MODULE = lfsr
TOPNAME = $(MODULE)
INC_PATH ?=

VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc -O3 --x-assign fast --x-initial fast --noassert

BUILD_DIR = ./build
NXDC_FILES = ./constr/$(TOPNAME).nxdc
OBJ_DIR_NVBOARD = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

all: ./obj_dir/V$(MODULE)
	@echo " ### executing file V$(MODULE) ### "
	./obj_dir/V$(MODULE)

./obj_dir/V$(MODULE): sim
	@echo " ### Simulating end ### "

wave: 
	@echo " ### use gtkwave to view the waveform ###"
	gtkwave waveform.vcd

.PHONY:run
run: ./obj_dir/V$(MODULE)
	@echo "### run the file /obj_dir/V$(MODULE) ###"
	@./obj_dir/V$(MODULE)
	
.PHONY: sim
sim:
	$(call git_commit, "sim RTL") 
	@echo " ### Verilator Simulating Begin ###  "
	verilator -Wall --trace --cc --exe --build csrc/tb_$(MODULE).cpp vsrc/*.v --top-module $(TOPNAME)

#@echo "Write this Makefile by your self."


.PHONY:clean
clean:
	-rm -rf obj_dir $(BUILD_DIR)
	-rm *.vcd

#PIN constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@
	
# project source
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "$(TOPNAME)_nvboard.cpp")
CSRCS += $(SRC_AUTO_BIND)

#rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk

#rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
LDFLAGS += -lSDL2 -lSDL2_image

$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR_NVBOARD)
	$(VERILATOR) $(VERILATOR_CFLAGS) --top-module $(TOPNAME) $^ \
	$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR_NVBOARD) --exe -o $(abspath $(BIN))

all_nvboard: default

default: $(BIN)

$(shell mkdir -p $(BUILD_DIR))



run_nvboard: $(BIN)
	@$^

