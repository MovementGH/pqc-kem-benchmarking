## Procedure for installation and synthesis of HQC
## Project: HQC-FPGA


## Summary
  - List of folders
  - Download Xilinx design tools
  - Install tools
  - Create and test pure HLS projects
  - Create and test optimized projects

## List of folders
  - pure-hls: Command files (TCL format) required to build, synthesize, simulate and place/route hardware project from the C source files 
  - optimized: Command files (TCL format) required to build, synthesize, simulate and place/route hardware project for manually optimized hardware projects
  - optimized/vhdl: Optimized vhdl source files for all possible configuration of the projects : COMPACT/PERF, keypair/encrypt/decrypt/all_functions
  
## Download Xilinx design tools
  - Download the Xilinx Vivado webpack tools : https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/2020-1.html
  - Register to Xilinx Website (free, no spam) 
  - Download the unified web installer (version 2020.1) for your OS. Do not choose the installer update ! Choose the full product installation.

## Install tools
  - Follow the install and licensing instruction https://www.xilinx.com/support/documentation/sw_manuals/xilinx2020_1/ug973-vivado-release-notes-install-license.pdf 
  - Select "Download and install" option
  - Accept the licence agreements
  - Select "Vivado" to install. You can choose "Vitis" if you need to develop advanced embedded systems, but the download will be longer.
  - Select "Vivado HL Webpack". This version is free of charge. 
  - To save time and space, below "Production devices", uncheck "SOCs", "Ultrascale", "Ultrascale+".
  - Select install directory.
  - Change the PATH environment variable to add Vivado folder : $XILINX/Vivado/2020.1
  - Where $XILINX is the installation folder of Xilinx software on your system.
    - On windows systems: Start Menu -> Parameters -> Environment variables -> Add variable "$XILINX/Vivado/2020.1"
    - On linux systems: open a terminal and type "export PATH=$XILINX/Vivado/2020.1/bin:$PATH". This will affect only the current terminal.
      To change PATH for every terminal, add the previous command into your profile : ~/.profile
      Type "source ~/.profile" to update the PATH with your modified profile    
  - Modify the main project Makefile so that the XIL_HOME var points to your installation
  
## Create and test pure HLS projects
  - All the process is controlled by 3 TCL scripts. These scripts are in HW-HQC/hls/pure-hls/. 
  - The possible settings are in settings.tcl. All the settings are explained is this file. Change the settings appropriately for your project. 
    Do not modify the two other TCL files
  
  - Change the project, the version, the processes applied.
    - Four projects are available : keypair/encaps/decaps/all_functions
  - Two versions: PERF for latency optimization, COMPACT for size optimization
  - Up to now, only HQC-128 is available. The other settings are for future versions
  - Verbose option are only for C simulation. There are no logs for C/VHDL cosimulation
  - The four processes that can be applied to projects are :
    - csim: compile C code, located in HW-HQC/src/ and in HW-HQC/tests/, with GCC and run a C simulation
    - csynth: synthesize VHDL files from the C code
    - cosim: perform first a C simulation with GCC and store results, the perform a VHDL simulation with Vivado and store results, finally compare C and VHDL results for correctness
      For cosim process, csynth process should have been performed 
    - implem: place/route the VHDL source file in an FPGA (Xilinx xc7a100tftg256-1).
        For implem process, csynth process should have been performed   
  - For example, if you want to validate a functionality, without synthesis, set "csim" variable to "yes", and "csynth", "cosim", "implem" to "no".
  - If a project has already been synthesized, and has not been modified or reset (see below), there is no need to re-synthesize it every time.
    For example, you can just perform cosimulation with : "cosim" variable set to "yes", and "csynth", "csim", "implem" set to "no". 
  - The reset option will delete previous projects with the same configuration
  
  - Create and validate project:
    - The run_hls.tcl script takes the settings, then creates the project and processes it.
  - If you have set the environment variable, just open a terminal and write : "vivado_hls run_hls.tcl".
    - If you do not have set the environment variable just search fot this tool in your Xilinx installation directory (it should be something like Vivado/Year.Version/bin/vivado_hls). 
  - The results will be stored in text files in the folders of your HLS project.
    - C simulation results: $VERSION/$HQC/$projetc_name/sol/sim/report/vhdl/*.log
    - C to VHDL synthesis results: $VERSION/$HQC/$projetc_name/sol/syn/report/*.rpt
    - Implementation results: $VERSION/$HQC/$projetc_name/sol/impl/report/vhdl/*.rpt
    - C/VHDL cosimulation results: $VERSION/$HQC/$projetc_name/sol/syn/report/*.rpt 
  - Note that cosimulation requires you to have libmpfr++-dev installed.
  
  - Once a project has been created, it can also be opened with the GUI tool.
    - On Linux system: write in a terminal "vivado_hls", and open the project with the menu.
    - On Windows system: start the GUI from the start menu.

## Create and test optimized projects
  - An optimized version of the VHDL files can be found inside HW-HQC/hls/optimized/vhdl
    - C-to-VHDL methodology can't take advantages of some features of VHDL. It is possible to further reduce the size of the logic by doing some manual changes.
    - These optimized VHDL files have been created by manually "factoring" a big function (Keccak internal permutation) in the HLS-generated VHDL.
    - Here, the function KeccakF1600_StatePermute is a big function and is instantiated several times. Our manual modification reduces the number of instantiations to two.
    - Modifying the eight projects (compact/perf for the four functions keygen/enc/dec/all_functions) manually takes less than an hour of cumulated work, many other manual improvements remain possible.
  - Including the eight full Vivado optimized projects in this release would have taken too much space, we thus provide the VHDL files and a set of TCL scripts that can be used to recreate the projects (which can then be used to do co-simulation, check performance measures, placement routing on a given device, check test benchs, etc.)
  - To recreate the projects follow the same procedure as above but using the TCL files in HW-HQC/hls/optimized
  - Warning: Reconstructing these optimized projects only works correctly if the sources in HW-HQC/src are untouched, if they have been modified some parts, specially co-simulation, may fail.
  - Internals:
    - When running the optimized tcl scripts, the Vivado project is created, the VHDL files are generated from the C source and then *they are overwritten by the optimized VHDL files that are in the HW-HQC/hls/optimized/vhdl directory*
    - Once the project has been created through the TCL script it can be used as any other project
