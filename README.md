# Post Quantum KEM benchmarking tools

This repository contains implementations for 2 post quantum KEMs (Kyber and HQC) as well as a reference implementation of ECDH using openssl.

The PQC KEMs are sourced from [NIST's selected algorithms](https://csrc.nist.gov/Projects/post-quantum-cryptography/selected-algorithms) and were not modified by us. We implemented an ECDH wrapper to convert OpenSSL's api into NISTs api for consistant benchmarking.

## Running Standalone Tests

To build standalone performance tests, run
```bash
make -C tests -j
```

Then to run them, run
```bash
make -C tests test
```

The test results will be placed in `tests/output`. Individual algorithm statistics can be found in txt files, and comparative numbers can be found in `results.csv`.