ROOT:=$(shell pwd)/hqc-128
include $(ROOT)/Makefile


GEN_TB_DATA_TARGET:=bin/gen_tb_data
GEN_TB_DATA_MAIN:=src/gen_tb_data.c

all: $(GEN_TB_DATA_TARGET)

local-folders: Makefile
	@/bin/echo -e "\n\n### Creating bin and data folders"
	mkdir -p bin
	mkdir -p data

$(GEN_TB_DATA_TARGET): $(HQC_OBJS) $(LIB_OBJS) Makefile | local-folders
	@/bin/echo -e "\n\n### $@"
	$(CC) $(CFLAGS) $(GEN_TB_DATA_MAIN) $(addprefix $(BUILD)/, $(HQC_OBJS)) $(addprefix $(BUILD)/, $(LIB_OBJS)) $(INCLUDE) $(LIB) -o $@

install: $(GEN_TB_DATA_TARGET)
	@/bin/echo -e "\n\n### Removing old testbench data"
	rm -rf data/*
	@/bin/echo -e "\n\n### Regenerating testbench data"
	cd data && ../$(GEN_TB_DATA_TARGET)
	@/bin/echo -e "\n\n### Copying testbench data to the correct directories"
	cp -f data/data_crypto_* ../tests/kem/data
	cp -f data/data_prng.dat ../tests/prng/data
	cp -f data/data_seedexpander.dat ../tests/prng/data

clean: myclean

myclean:
	@/bin/echo -e "\n\n### Removing bin and data folders"
	rm -rf data

