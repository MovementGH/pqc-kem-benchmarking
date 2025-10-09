# Settings for HQC HLS synthesis

##############################################
# List of settings
#
#   version:       - PERF/COMPACT
#   prj:      	    - keypair/encrypt/decrypt/all_functions
#   csim:      	    - yes/no
#   csynth:      	- yes/no
#   cosim:      	- yes/no
#   implem:      	- yes/no
#   reset:      	    - yes/no
##############################################

#   version:       - PERF: target for performance: large multiplication, loop pipelining... 
#              		- COMPACT: target for size: small multiplication, no pipeline...
set version COMPACT

#   cosim:      	- yes: perform co-simulation C/VHDL
#              		- no: no co-simulation
#                      Remark: Co-simulation can be done only if VHDL synthesis has been performed
set cosim yes

#   implem:      	- yes: perfom VHDL synthesis and place/route
#              		- no: no place/route
#                      Remark: Co-simulation can be done only if VHDL synthesis has been performed   
set implem yes 

#   reset:      	    - yes: reset same previous HLS project: delete all generated files: VHDL, reports...
#              		- no: use previous project, if it exists, without resetting, keep all files
set reset yes 

##############################################
# Paths to source files
##############################################

# Path to source directory
set sourcedir ../../../src/
# Path to testbench directory
set tbdir ../src
# Path to generic testbench directory
set genericdir ../../tb_generic/src

##############################################
# Hardware specifications
##############################################

# design language
set language vhdl
# FPGA family
set family artix7
# FPGA device
set device xc7a100t
# Device package
set package ftg256
# Device speed
set speed -1
# Clock name
set ap_clock clk




