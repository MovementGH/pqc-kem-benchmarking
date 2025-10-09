# HQC FPGA Implementation

## Usage

Build and run the KAT tests comparing the HLS-compatible implementation with the actual HQC-128 implementation in software
with `make run-all`. 

Modify the Makefile first lines to alter the main options (performance/compact implementation, C-native-types/HLS-types, 
standard/verbose) used to compile the HLS-compatible implementation.

Access the hls directory and follow the instructions in hls/README.md to install Xilinx tools and build a VHDL implementation, 
for an Artix-7 by default, from the HLS-compatible sources. The projects include the same test benchs as the software version. 
These tests check that outputs from the Hardware version are exactly the same as the ones from the software HLS-compatible 
version (and thus than the ones from the original software HQC-128 version).

## Resources

  * Makefile: Makefile for building the library unit tests in software from the HLS-compatible HQC sources
  * README.md: This file
  * hls: folders for hardware implementation, and a specific README.md file
  * hls/pure-hls: HLS configuration files (TCL format) required to build the hardware implementation
  * hls/optimized: HLS configuration files (TCL format) required to build the optimized hardware implementation
  * hls/optimized/vhdl: VHDL source files derived from the C to VHDL output with some manual refactoring to eliminate
    functions that have been instantiated by HLS more than needed
  * ref_value: The actual HQC-128 software implementation and a Makefile used to generate KAT values
  * ref_value/hqc-128: The HQC-128 sources
  * ref_value/src: The sources used to generate the KAT values from ref_value/hqc-128
  * src: The HLS-compatible HQC source files
  * tests: The unit tests that check that the hardware implementation leads to the same KAT values as the original software implementation
  * tests/kem: The unit tests for the KEM Keypair Generation, Encapsulation, and Decapsulation functions
  * tests/prng: The unit tests for the PRNG and Seed Expander
  * tests/tb_generic: The sources of the generic function used to build the KAT tests