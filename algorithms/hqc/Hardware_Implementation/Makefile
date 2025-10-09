# List of macros
#   HLS:       - HLS_DATATYPES: compile with HLS lib datatypes (see below)
#              - NO_HLS: compile with native C data types
#   ARCH:      - PERF: compile for high performances on HLS (data size, operator size, parallelism...)
#              - COMPACT: compile for area reduction for HLS
#   XIL_HOME:  - Path to the HLS libraries. 

CC:=g++

CFLAGS:=-g

HLS:=HLS_DATATYPES

ARCH:=PERF

V:=VERBOSE

DEFINES:=-D$(HLS) -D$(ARCH) -D$(V) 

# Path to the HLS libraries.
XIL_HOME:=xil

KEM_COMMON_OBJS:=api.o hqc.o parsing.o shake_prng.o vector.o code.o reed_solomon.o reed_muller.o gf.o gf2x.o shake_ds.o KeccakF1600_StatePermute.o Keccak_Squeezeblocks.o Keccak_Stream_Absorb.o hls_fft_add.o kem.o
KEM_HEADERS:=$(wildcard src/*.h)

SRC:=src
KEM_TB_DIR:=tests/kem
PRNG_TB_DIR:=tests/prng
TB_DATA_DIR:=ref_value/data

GENERIC_TB_SOURCE:=tests/tb_generic/src/tb_generic.cpp
TB_REFDATA_DEPENDENCIES:=$(wildcard ref_value/src/*) $(wildcard ref_value/hqc-128/src/*) $(wildcard ref_value/hqc-128/lib/fips202/*) ref_value/Makefile ref_value/hqc-128/Makefile

KEYPAIR_TB_TARGET:=$(KEM_TB_DIR)/bin/tb_crypto_kem_keypair
KEYPAIR_TB_DATA:=$(KEM_TB_DIR)/data/data_crypto_kem_keypair.dat
KEYPAIR_TB_MAIN:=$(KEM_TB_DIR)/src/tb_crypto_kem_keypair.cpp
KEYPAIR_TB_SOURCES:=$(KEYPAIR_TB_MAIN) $(addprefix $(KEM_TB_DIR)/bin/build/, $(KEM_COMMON_OBJS)) $(GENERIC_TB_SOURCE)

ENC_TB_TARGET:=$(KEM_TB_DIR)/bin/tb_crypto_kem_enc
ENC_TB_DATA:=$(KEM_TB_DIR)/data/data_crypto_kem_enc.dat
ENC_TB_MAIN:=$(KEM_TB_DIR)/src/tb_crypto_kem_enc.cpp
ENC_TB_SOURCES:=$(ENC_TB_MAIN) $(addprefix $(KEM_TB_DIR)/bin/build/, $(KEM_COMMON_OBJS)) $(GENERIC_TB_SOURCE)

DEC_TB_TARGET:=$(KEM_TB_DIR)/bin/tb_crypto_kem_dec
DEC_TB_MAIN:=$(KEM_TB_DIR)/src/tb_crypto_kem_dec.cpp
DEC_TB_SOURCES:=$(DEC_TB_MAIN) $(addprefix $(KEM_TB_DIR)/bin/build/, $(KEM_COMMON_OBJS)) $(GENERIC_TB_SOURCE)

ALLF_TB_TARGET:=$(KEM_TB_DIR)/bin/tb_crypto_kem_all_functions
ALLF_TB_MAIN:=$(KEM_TB_DIR)/src/tb_crypto_kem_all_functions.cpp
ALLF_TB_SOURCES:=$(ALLF_TB_MAIN) $(addprefix $(KEM_TB_DIR)/bin/build/, $(KEM_COMMON_OBJS)) $(GENERIC_TB_SOURCE)

PRNG_TB_TARGET:=$(PRNG_TB_DIR)/bin/tb_prng
PRNG_TB_DATA:=$(PRNG_TB_DIR)/data/data_prng.dat
PRNG_TB_MAIN:=$(PRNG_TB_DIR)/src/tb_prng.cpp
PRNG_TB_SOURCES:=$(PRNG_TB_MAIN) src/shake_prng.cpp src/vector.cpp src/KeccakF1600_StatePermute.cpp src/Keccak_Squeezeblocks.cpp src/Keccak_Stream_Absorb.cpp $(GENERIC_TB_SOURCE)

SEEDEXPANDER_TB_TARGET:=$(PRNG_TB_DIR)/bin/tb_seedexpander
SEEDEXPANDER_TB_DATA:=$(PRNG_TB_DIR)/data/data_seedexpander.dat
SEEDEXPANDER_TB_MAIN:=$(PRNG_TB_DIR)/src/tb_seedexpander.cpp
SEEDEXPANDER_TB_SOURCES:=$(SEEDEXPANDER_TB_MAIN) src/shake_prng.cpp src/vector.cpp src/KeccakF1600_StatePermute.cpp src/Keccak_Squeezeblocks.cpp src/Keccak_Stream_Absorb.cpp $(GENERIC_TB_SOURCE)


all: kem prng seedexpander

kem: keypair enc dec allf

prng: $(PRNG_TB_TARGET) 

seedexpander: $(SEEDEXPANDER_TB_TARGET)

keypair: $(KEYPAIR_TB_TARGET)

enc: $(ENC_TB_TARGET)

dec: $(DEC_TB_TARGET)

allf: $(ALLF_TB_TARGET)

ref-data: $(KEYPAIR_TB_DATA) $(ENC_TB_DATA) $(PRNG_TB_DATA) $(SEEDEXPANDER_TB_DATA)

$(KEYPAIR_TB_TARGET): $(KEYPAIR_TB_SOURCES) $(KEM_HEADERS) Makefile | $(KEM_TB_DIR)/bin/build $(KEYPAIR_TB_DATA)
	@/bin/echo -e "\n\n### Compiling $@"
	$(CC) $(CFLAGS) $(DEFINES) -I lib -I src -I $(XIL_HOME)/include -o $@ $(KEYPAIR_TB_SOURCES) $(KEM_HEADERS)

$(ENC_TB_TARGET): $(ENC_TB_SOURCES) $(KEM_HEADERS) Makefile | $(KEM_TB_DIR)/bin/build $(ENC_TB_DATA)
	@/bin/echo -e "\n\n### Compiling $@"
	$(CC) $(CFLAGS) $(DEFINES) -I lib -I src -I $(XIL_HOME)/include -o $@ $(ENC_TB_SOURCES) $(KEM_HEADERS)

$(DEC_TB_TARGET): $(DEC_TB_SOURCES) $(KEM_HEADERS) Makefile | $(KEM_TB_DIR)/bin/build
	@/bin/echo -e "\n\n### Compiling $@"
	$(CC) $(CFLAGS) $(DEFINES) -I lib -I src -I $(XIL_HOME)/include -o $@ $(DEC_TB_SOURCES) $(KEM_HEADERS)

$(ALLF_TB_TARGET): $(ALLF_TB_SOURCES) $(KEM_HEADERS) Makefile | $(KEM_TB_DIR)/bin/build
	@/bin/echo -e "\n\n### Compiling $@"
	$(CC) $(CFLAGS) $(DEFINES) -I lib -I src -I $(XIL_HOME)/include -o $@ $(ALLF_TB_SOURCES) $(KEM_HEADERS)

$(PRNG_TB_TARGET): $(PRNG_TB_SOURCES) $(KEM_HEADERS) Makefile | $(PRNG_TB_DIR)/bin $(PRNG_TB_DIR)/data $(PRNG_TB_DATA)
	@/bin/echo -e "\n\n### Compiling $@"
	$(CC) $(CFLAGS) $(DEFINES) -I lib -I src -I $(XIL_HOME)/include -o $@ $(PRNG_TB_SOURCES)

$(SEEDEXPANDER_TB_TARGET): $(SEEDEXPANDER_TB_SOURCES) $(KEM_HEADERS) Makefile | $(PRNG_TB_DIR)/bin $(PRNG_TB_DIR)/data $(SEEDEXPANDER_TB_DATA)
	@/bin/echo -e "\n\n### Compiling $@"
	$(CC) $(CFLAGS) $(DEFINES) -I lib -I src -I $(XIL_HOME)/include -o $@ $(SEEDEXPANDER_TB_SOURCES) 

$(KEYPAIR_TB_DATA) $(ENC_TB_DATA) $(PRNG_TB_DATA) $(SEEDEXPANDER_TB_DATA): $(TB_REFDATA_DEPENDENCIES) | $(KEM_TB_DIR)/data $(PRNG_TB_DIR)/data
	@/bin/echo -e "\n\n### Updating testbench reference data"
	cd ref_value && make -f ./Makefile all && make -f ./Makefile install

$(KEM_TB_DIR)/bin/build $(KEM_TB_DIR)/data $(PRNG_TB_DIR)/bin $(PRNG_TB_DIR)/data:
	@/bin/echo -e "\n\n### Creating dir $@"
	mkdir -p $@

$(KEM_TB_DIR)/bin/build/%.o: $(SRC)/%.cpp $(SRC)/%.h Makefile | $(KEM_TB_DIR)/bin/build
	@/bin/echo -e "\n\n### Compiling $@"
	$(CC) $(CFLAGS) $(DEFINES) -I lib -I src -I $(XIL_HOME)/include -c $< -o $@

run-keypair: keypair
	@/bin/echo -e "\n\n### Running test"
	cd $(KEM_TB_DIR)/bin && ./$(notdir $(KEYPAIR_TB_TARGET))

run-enc: enc
	@/bin/echo -e "\n\n### Running test"
	cd $(KEM_TB_DIR)/bin && ./$(notdir $(ENC_TB_TARGET))

run-dec: dec
	@/bin/echo -e "\n\n### Running test"
	cd $(KEM_TB_DIR)/bin && ./$(notdir $(DEC_TB_TARGET))

run-allf: allf
	cd $(KEM_TB_DIR)/bin && ./$(notdir $(ALLF_TB_TARGET))
	@/bin/echo -e "\n\n### Running test"

run-prng: prng
	@/bin/echo -e "\n\n### Running test"
	cd $(PRNG_TB_DIR)/bin && ./$(notdir $(PRNG_TB_TARGET))

run-seedexpander: seedexpander
	@/bin/echo -e "\n\n### Running test"
	cd $(PRNG_TB_DIR)/bin && ./$(notdir $(SEEDEXPANDER_TB_TARGET))

run-all: run-keypair run-enc run-dec run-allf run-prng run-seedexpander

clean:
	@/bin/echo -e "\n\n### Removing bin and data directories"
	rm -rf $(KEM_TB_DIR)/bin $(KEM_TB_DIR)/data
	rm -rf $(PRNG_TB_DIR)/bin $(PRNG_TB_DIR)/data
