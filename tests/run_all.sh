#!/bin/bash
# Complete M1 Mac Benchmarking Script

set -e

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}=== PQC KEM M1 Mac Benchmarking ===${NC}\n"

# Set OpenSSL paths
export OPENSSL_PREFIX="/opt/homebrew/opt/openssl@3"

echo "Step 1: Cleaning..."
make clean 2>/dev/null || true
rm -f *.o *.a *.test
rm -rf output

echo -e "\nStep 2: Building Kyber..."
for variant in 512 768 1024; do
    echo "  Building Kyber-$variant..."
    
    # Compile benchmark.o
    [ ! -f benchmark.o ] && gcc -O3 -flto -fomit-frame-pointer -c benchmark.c -o benchmark.o
    
    # Compile Kyber
    KYBER_DIR="../algorithms/kyber/Optimized_Implementation/crypto_kem/kyber$variant"
    
    for src in cbd fips202 indcpa kem ntt poly polyvec reduce rng verify symmetric-shake; do
        gcc -O3 -flto -fomit-frame-pointer -I${OPENSSL_PREFIX}/include -c "$KYBER_DIR/$src.c" -o "$src.o" 2>/dev/null
    done
    
    ar rcs "kyber-$variant.a" *.o
    gcc -O3 -flto -fomit-frame-pointer -c main.c -I"$KYBER_DIR" -o "kyber-main-$variant.o"
    gcc -o "kyber-$variant.test" benchmark.o "kyber-main-$variant.o" "kyber-$variant.a" -flto -lm -L${OPENSSL_PREFIX}/lib -lcrypto
    
    rm -f cbd.o fips202.o indcpa.o kem.o ntt.o poly.o polyvec.o reduce.o rng.o verify.o symmetric-shake.o
    echo "  ✓ Done"
done

echo -e "\nStep 3: Building ECDH..."
for variant in 256 384 521; do
    echo "  Building ECDH-$variant..."
    
    gcc -O3 -flto -fomit-frame-pointer -I${OPENSSL_PREFIX}/include -c ../algorithms/ecdh/ecdh.c -o "ecdh.o" -DECDH_SECURITY_LEVEL=$variant
    ar rcs "ecdh-$variant.a" ecdh.o
    gcc -O3 -flto -fomit-frame-pointer -c main.c -I../algorithms/ecdh -DECDH_SECURITY_LEVEL=$variant -o "ecdh-main-$variant.o"
    gcc -o "ecdh-$variant.test" benchmark.o "ecdh-main-$variant.o" "ecdh-$variant.a" -flto -lm -L${OPENSSL_PREFIX}/lib -lcrypto
    rm -f ecdh.o
    echo "  ✓ Done"
done

echo -e "\nStep 4: Running tests..."
mkdir -p output

for test in kyber-512 kyber-768 kyber-1024 ecdh-256 ecdh-384 ecdh-521; do
    echo "  Testing $test..."
    ./$test.test > output/$test.txt 2>/dev/null
done

echo -e "\n${GREEN}=== RESULTS ===${NC}\n"
echo "Algorithm  | Encaps(μs) | Decaps(μs)"
echo "-----------|------------|------------"

for alg in kyber-512 kyber-768 kyber-1024 ecdh-256 ecdh-384 ecdh-521; do
    if [ -f "output/$alg.txt" ]; then
        E=$(grep -A1 "Encapsulation:" "output/$alg.txt" | grep "Median" | awk '{print $3/1000}')
        D=$(grep -A1 "Decapsulation:" "output/$alg.txt" | grep "Median" | awk '{print $3/1000}')
        printf "%-10s | %10s | %10s\n" "$alg" "$E" "$D"
    fi
done

echo -e "\n${GREEN}✓ Complete! Results in output/${NC}"
