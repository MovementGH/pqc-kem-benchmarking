CC=gcc
CFLAGS=-O3 -march=native -flto -fomit-frame-pointer
LDFLAGS=-flto -lm
ALGORITHMS=kyber ecdh
ALGORITHMS_DIR=../algorithms

ifneq ($(shell uname -p),arm)
ALGORITHMS := $(ALGORITHMS) kyber-avx hqc
else
# OpenSSL paths for M1 Mac
OPENSSL_PREFIX=/opt/homebrew/opt/openssl@3
CFLAGS := $(CFLAGS) -I$(OPENSSL_PREFIX)/include
LDFLAGS := $(LDFLAGS) -L$(OPENSSL_PREFIX)/lib
endif

# Common object files
COMMON_OBJS=benchmark.o

# Benchmarking
tests: $(addsuffix -tests, $(ALGORITHMS))
libs: $(addsuffix -libs, $(ALGORITHMS))

test:
	mkdir -p output
	echo 'Algorithm,"Public Key Size","Secret Key Size","Ciphertext Size","Encapsulation (ns)","Decapsulation (ns)","Handshake (ns)","Kilohandshakes/S"' > output/results.csv
	for file in *.test; do \
		./$$file > output/$${file%.test}.txt 2>> output/results.csv; \
	done

benchmark.o: benchmark.c benchmark.h
	$(CC) $(CFLAGS) -c -o benchmark.o benchmark.c

clean: $(addsuffix -clean, $(ALGORITHMS))
	rm -f *.o *.test *.a
	rm -f $(ALGORITHMS_DIR)/ecdh/*.o
	rm -rf output

.PHONY: tests libs test clean $(addsuffix -tests, $(ALGORITHMS)) $(addsuffix -libs, $(ALGORITHMS))

# HQC
HQC_VARIANTS=128 192 256
HQC_DIR=$(ALGORITHMS_DIR)/hqc/Optimized_Implementation

.PRECIOUS: $(HQC_DIR)/hqc-%/bin

$(HQC_DIR)/hqc-%/bin:
	$(MAKE) -C $(HQC_DIR)/hqc-$* clean
	$(MAKE) -C $(HQC_DIR)/hqc-$* hqc-$*

hqc-%.a: $(HQC_DIR)/hqc-%/bin
	$(AR) rcs $@ $(HQC_DIR)/hqc-$*/bin/build/*.o

hqc-main-%.o: main.c benchmark.h
	$(CC) -c $(CFLAGS) -o $@ main.c -I$(HQC_DIR)/hqc-$*/src

hqc-%.test: $(COMMON_OBJS) hqc-main-%.o hqc-%.a
	$(CC) -o $@ $(COMMON_OBJS) hqc-main-$*.o hqc-$*.a $(LDFLAGS)

hqc-tests: $(addsuffix .test, $(addprefix hqc-, $(HQC_VARIANTS)))
hqc-libs: $(addsuffix .a, $(addprefix hqc-, $(HQC_VARIANTS)))

hqc-clean:
	rm -rf $(HQC_DIR)/hqc-*/bin

# Kyber AVX
KYBER_AVX_VARIANTS=512 768 1024
KYBER_AVX_DIR=$(ALGORITHMS_DIR)/kyber/Additional_Implementations/avx2/crypto_kem
KYBER_AVX_CFLAGS=-mavx2 -mbmi2 -mpopcnt -maes -march=native -mtune=native -O3 -flto -fomit-frame-pointer
KYBER_AVX_LDFLAGS=-flto -lcrypto
KYBER_AVX_C=cbd.c consts.c indcpa.c kem.c poly.c polyvec.c rejsample.c rng.c verify.c fips202.c fips202x4.c keccak4x/KeccakP-1600-times4-SIMD256.c symmetric-shake.c
KYBER_AVX_ASM=fq.S invntt.S ntt.S shuffle.S basemul.S
KYBER_AVX_SRC=$(KYBER_AVX_C) $(KYBER_AVX_ASM)
KYBER_AVX_OBJ=$(subst .c,.o,$(KYBER_AVX_C)) $(subst .S,.o,$(KYBER_AVX_ASM))

define KYBER_AVX_template
$(KYBER_AVX_DIR)/kyber$(1)/%.o: $(KYBER_AVX_DIR)/kyber$(1)/%.c
	$$(CC) $$(CFLAGS) $$(KYBER_AVX_CFLAGS) -c -o $$@ $$<

$(KYBER_AVX_DIR)/kyber$(1)/%.o: $(KYBER_AVX_DIR)/kyber$(1)/%.S
	$$(CC) $$(CFLAGS) $$(KYBER_AVX_CFLAGS) -I$(KYBER_AVX_DIR)/kyber$(1) -c -o $$@ $$<
endef

$(foreach v,$(KYBER_AVX_VARIANTS),$(eval $(call KYBER_AVX_template,$(v))))

kyber-avx-%.a: $(addprefix $(KYBER_AVX_DIR)/kyber%/,$(KYBER_AVX_OBJ))
	$(AR) rcs $@ $^

kyber-avx-main-%.o: main.c benchmark.h
	$(CC) -c $(CFLAGS) -o $@ main.c -I$(KYBER_AVX_DIR)/kyber$*

kyber-avx-%.test: $(COMMON_OBJS) kyber-avx-main-%.o kyber-avx-%.a
	$(CC) -o $@ $(COMMON_OBJS) kyber-avx-main-$*.o kyber-avx-$*.a $(LDFLAGS) $(KYBER_AVX_LDFLAGS)

kyber-avx-tests: $(addsuffix .test, $(addprefix kyber-avx-, $(KYBER_AVX_VARIANTS)))
kyber-avx-libs: $(addsuffix .a, $(addprefix kyber-avx-, $(KYBER_AVX_VARIANTS)))

kyber-avx-clean:
	find $(KYBER_AVX_DIR) -name '*.o' -delete

# Kyber
KYBER_VARIANTS=512 768 1024
KYBER_DIR=$(ALGORITHMS_DIR)/kyber/Optimized_Implementation/crypto_kem
KYBER_CFLAGS=-O3 -march=native -flto -fomit-frame-pointer
KYBER_LDFLAGS=-flto -lcrypto
KYBER_C=cbd.c fips202.c indcpa.c kem.c ntt.c poly.c polyvec.c PQCgenKAT_kem.c reduce.c rng.c verify.c symmetric-shake.c
KYBER_SRC=$(KYBER_C) $(KYBER_ASM)
KYBER_OBJ=$(subst .c,.o,$(KYBER_C)) $(subst .S,.o,$(KYBER_ASM))

define KYBER_template
$(KYBER_DIR)/kyber$(1)/%.o: $(KYBER_DIR)/kyber$(1)/%.c
	$$(CC) $$(CFLAGS) $$(KYBER_CFLAGS) -c -o $$@ $$<
endef

$(foreach v,$(KYBER_VARIANTS),$(eval $(call KYBER_template,$(v))))

kyber-%.a: $(addprefix $(KYBER_DIR)/kyber%/,$(KYBER_OBJ))
	$(AR) rcs $@ $^

kyber-main-%.o: main.c benchmark.h
	$(CC) -c $(CFLAGS) -o $@ main.c -I$(KYBER_DIR)/kyber$*

kyber-%.test: $(COMMON_OBJS) kyber-main-%.o kyber-%.a
	$(CC) -o $@ $(COMMON_OBJS) kyber-main-$*.o kyber-$*.a $(LDFLAGS) $(KYBER_LDFLAGS)

kyber-tests: $(addsuffix .test, $(addprefix kyber-, $(KYBER_VARIANTS)))
kyber-libs: $(addsuffix .a, $(addprefix kyber-, $(KYBER_VARIANTS)))

kyber-clean:
	find $(KYBER_DIR) -name '*.o' -delete

# ECDH
ECDH_VARIANTS=256 384 521
ECDH_DIR=$(ALGORITHMS_DIR)/ecdh

$(ECDH_DIR)/ecdh-%.o: $(ECDH_DIR)/ecdh.c
	$(CC) $(CFLAGS) -c $(ECDH_DIR)/ecdh.c -o $(ECDH_DIR)/ecdh-$*.o -DECDH_SECURITY_LEVEL=$*

ecdh-%.a: $(ECDH_DIR)/ecdh-%.o
	$(AR) rcs $@ $(ECDH_DIR)/ecdh-$*.o

ecdh-main-%.o: main.c benchmark.h
	$(CC) -c $(CFLAGS) -o $@ main.c -I$(ECDH_DIR) -DECDH_SECURITY_LEVEL=$*

ecdh-%.test: $(COMMON_OBJS) ecdh-main-%.o ecdh-%.a
	$(CC) -o $@ $(COMMON_OBJS) ecdh-main-$*.o ecdh-$*.a $(LDFLAGS) -lcrypto

ecdh-tests: $(addsuffix .test, $(addprefix ecdh-, $(ECDH_VARIANTS)))
ecdh-libs: $(addsuffix .a, $(addprefix ecdh-, $(ECDH_VARIANTS)))

ecdh-clean:
	rm -f $(ECDH_DIR)/*.o
