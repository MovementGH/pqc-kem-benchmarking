#!/bin/bash
export OPENSSL_CONF=~/oqs-build/openssl-oqs.cnf
OPENSSL=/opt/homebrew/opt/openssl@3/bin/openssl
OUTPUT_FILE=~/oqs-build/mlkem_tls_results.txt

echo "=== ML-KEM-768 (Kyber) TLS 1.3 Benchmark ===" | tee $OUTPUT_FILE
echo "Date: $(date)" | tee -a $OUTPUT_FILE
echo "" | tee -a $OUTPUT_FILE

for i in $(seq 1 30); do
    START=$(python3 -c "import time; print(time.time())")
    $OPENSSL s_client -connect localhost:8443 -groups mlkem768 < /dev/null > /dev/null 2>&1
    END=$(python3 -c "import time; print(time.time())")
    ELAPSED=$(python3 -c "print(f'{$END - $START:.4f}')")
    echo "Run $i: ${ELAPSED}s" | tee -a $OUTPUT_FILE
done

echo "" | tee -a $OUTPUT_FILE
echo "=== Complete ===" | tee -a $OUTPUT_FILE
