CC=gcc
CFLAGS=-O3 -flto
LDFLAGS=-flto
ALGORITHMS=hqc kyber ecdh
ALGORITHMS_DIR=../algorithms

# Benchmarking
tests: $(addsuffix -tests, $(ALGORITHMS))
libs: $(addsuffix -libs, $(ALGORITHMS))

test:
	echo "Test, Public Key Size, Private Key Size, Signature Size, Signs/s, Verifies/s" > results.csv
	for file in *.test; do echo "========== $${file%.test} =========="; ./$$file; echo ""; done > results.txt 2>> results.csv

benchmark.o: benchmark.c
	$(CC) $(CFLAGS) -c -o benchmark.o benchmark.c

clean:
	rm -f *.o *.test *.a
	rm -f results.*

.PHONY: tests libs test clean $(addsuffix -tests, $(ALGORITHMS)) $(addsuffix -libs, $(ALGORITHMS))

# HQC
HQC_VARIANTS=128 192 256
HQC_DIR=$(ALGORITHMS_DIR)/hqc/Optimized_Implementation

.PRECIOUS: $(HQC_DIR)/hqc-%/bin

$(HQC_DIR)/hqc-%/bin:
	$(MAKE) -C $(HQC_DIR)/hqc-$* hqc-$*

hqc-%.a: $(HQC_DIR)/hqc-%/bin
	ar rcs $@ $(HQC_DIR)/hqc-$*/bin/build/*

hqc-%.o: main.c
	$(CC) -c $(CFLAGS) -o $@ main.c -I$(HQC_DIR)/hqc-$*/src

hqc-%.test: benchmark.o hqc-%.o hqc-%.a
	$(CC) -o $@ $(LDFLAGS) benchmark.o hqc-$*.o hqc-$*.a

hqc-tests: $(addsuffix .test, $(addprefix hqc-, $(HQC_VARIANTS)))
hqc-libs: $(addsuffix .a, $(addprefix hqc-, $(HQC_VARIANTS)))

# Kyber
KYBER_VARIANTS=512 768 1024
KYBER_DIR=$(ALGORITHMS_DIR)/kyber/Additional_Implementations/avx2/crypto_kem
KYBER_CFLAGS=-mavx2 -mbmi2 -mpopcnt -maes -march=native -mtune=native -O3 -fomit-frame-pointer
KYBER_LDFLAGS=-lcrypto
KYBER_C=cbd.c consts.c indcpa.c kem.c poly.c polyvec.c rejsample.c rng.c verify.c fips202.c fips202x4.c keccak4x/KeccakP-1600-times4-SIMD256.c symmetric-shake.c
KYBER_ASM=fq.S invntt.S ntt.S shuffle.S basemul.S
KYBER_SRC=$(KYBER_C) $(KYBER_ASM)
KYBER_OBJ=$(subst .c,.o,$(KYBER_C)) $(subst .S,.o,$(KYBER_ASM))

define KYBER_template
$(KYBER_DIR)/kyber$(1)/%.o: $(KYBER_DIR)/kyber$(1)/%.c
	$$(CC) $$(CFLAGS) $$(KYBER_CFLAGS) -c -o $$@ $$<

$(KYBER_DIR)/kyber$(1)/%.o: $(KYBER_DIR)/kyber$(1)/%.S
	$$(CC) $$(CFLAGS) $$(KYBER_CFLAGS) -I$(KYBER_DIR)/kyber$(1) -c -o $$@ $$<
endef

$(foreach v,$(KYBER_VARIANTS),$(eval $(call KYBER_template,$(v))))

kyber-%.a: $(addprefix $(KYBER_DIR)/kyber%/,$(KYBER_OBJ))
	ar rcs $@ $^

kyber-%.o: main.c
	$(CC) -c $(CFLAGS) -o $@ main.c -I$(KYBER_DIR)/kyber$*

kyber-%.test: benchmark.o kyber-%.o kyber-%.a
	$(CC) -o $@ $(LDFLAGS) $(KYBER_LDFLAGS) benchmark.o kyber-$*.o kyber-$*.a

kyber-tests: $(addsuffix .test, $(addprefix kyber-, $(KYBER_VARIANTS)))
kyber-libs: $(addsuffix .a, $(addprefix kyber-, $(KYBER_VARIANTS)))

# ECDH
ECDH_VARIANTS=256
ECDH_DIR=$(ALGORITHMS_DIR)/ecdh

$(ECDH_DIR)/ecdh-%/ecdh.o: $(ECDH_DIR)/ecdh-%/ecdh.c
	$(CC) $(CFLAGS) -c $(ECDH_DIR)/ecdh-$*/ecdh.c -o $(ECDH_DIR)/ecdh-$*/ecdh.o

ecdh-%.a: $(ECDH_DIR)/ecdh-%/ecdh.o
	ar rcs $@ $(ECDH_DIR)/ecdh-$*/*.o

ecdh-%.o: main.c
	$(CC) -c $(CFLAGS) -o $@ main.c -I$(ECDH_DIR)/ecdh-$*

ecdh-%.test: benchmark.o ecdh-%.o ecdh-%.a
	$(CC) -o $@ $(LDFLAGS) benchmark.o ecdh-$*.o ecdh-$*.a -lcrypto

ecdh-tests: $(addsuffix .test, $(addprefix ecdh-, $(ECDH_VARIANTS)))
ecdh-libs: $(addsuffix .a, $(addprefix ecdh-, $(ECDH_VARIANTS)))