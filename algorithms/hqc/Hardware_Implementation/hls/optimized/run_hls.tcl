############################################################
## TCL script file for vivado HLS
## Project : HQC-FPGA
## For Vivado HLS 2020.1 and Vitis HLS 2020.2
##
## Do not modify
############################################################

# External depencies
source ./settings.tcl
source ./set_directives.tcl

# Convert project name 
switch $prj {
	"keypair" {set prj_name keypair}
	"encaps" {set prj_name enc}
	"decaps" {set prj_name dec}
	"all_functions" {set prj_name all_functions}
	default {error "Project not recognized : ${prj}"}
	exit 1
}

set top_module crypto_kem_${prj_name}
set sol_name sol

# Clock period
switch $version {
	"PERF" {set clk_period 6.6}
	"COMPACT" {set clk_period 8}
	default {error "Version not recognized : ${version}"}
}

# Verbose
switch $verb {
	"yes" {set verb VERBOSE}
	"no" {set verb NO_VERB}
	default {error "Version not recognized : ${version}"}
}

# Create folder tree
set prj_path $version/HQC-$coding
set sol_path  ${prj_name}/$sol_name

if { ![file exists $prj_path] } {
    file mkdir $prj_path
}

# Adapt source path set in settings.tcl
set sourcepath ../../$sourcedir
set tbpath ../../$tbdir
set genericpath ../../$genericdir

# Move into project folder
cd $prj_path

# Create constraint file
set_directives $top_module $version

puts ""
puts "-------------------------------------"
puts "-- Create project "
puts "-- $top_module" 
puts "-- $version" 
puts "-------------------------------------"
puts ""

if {$reset  == "yes"} {open_project -reset ${prj_name}} else {open_project ${prj_name}} 

set_top ${top_module}_hls

# Add source files
set hfiles [glob -directory $sourcepath *.h]
set api ${sourcepath}/api.h
foreach file $hfiles {
	if {$file != $api } {
		puts $file
		add_files $file -cflags "-DHLS_DATATYPES -DHLS -D${version} -D${verb}"
	}
}
set cfiles [glob -directory $sourcepath *.cpp]
set api ${sourcepath}/api.cpp
foreach file $cfiles {
	if {$file != $api} {
		add_files $file -cflags "-DHLS_DATATYPES -DHLS -D${version} -D${verb}"
	}
}

# Add testbench files
add_files -tb ${genericpath}/tb_generic.cpp -cflags "-I${sourcepath}/ -DHLS_DATATYPES -DHLS -D$version -D${verb}" -csimflags "-I${sourcepath}/ -DHLS_DATATYPES -DHLS -D$version -D${verb}"
add_files -tb ${tbpath}/tb_${top_module}.cpp -cflags "-I${sourcepath}/ -DHLS_DATATYPES -DHLS -D$version -D${verb}" -csimflags "-I${sourcepath}/ -DHLS_DATATYPES -DHLS -D$version -D${verb}" 

puts "---------------------"
puts "-- Create solution --"
puts "---------------------"
if {$reset  == "yes"} {open_solution -reset $sol_name} else {open_solution $sol_name} 

# Configure project
set_part ${device}${package}${speed}
create_clock -period $clk_period -name clk
config_export -description "$top_module" -display_name "$top_module" -format ip_catalog -rtl vhdl -vendor "ISAE" -vivado_optimization_level 2 -vivado_phys_opt all -vivado_report_level 1

# Add constraint file
source "directives.tcl"
file delete "directives.tcl"

# Configure design flow
config_schedule -effort high
config_schedule -relax_ii_for_timing
set_clock_uncertainty 0.5

# C Simulation
if {$csim == "yes"} {
# Import reference data files	
	set data_path ${sol_path}/csim/data
	if { ![file exists $data_path] } {
		file mkdir $data_path
	}
	set dat_file ../../../../ref_value/data/data_${top_module}.dat
	if { [file exists $dat_file] } {
		file copy -force $dat_file $data_path/
	}else {
		puts "ERROR : missing ref data file"
		exit 1
	}
	puts "----------------"
	puts "-- Simulation --"
	puts "----------------"
	csim_design
}

# C to VHDL synthesis
if {$csynth == "yes"} { 
	puts "---------------"
	puts "-- Synthesis --"
	puts "---------------"
	csynth_design
	
	# Import optimized VHDL files
	set sourcedir ../../vhdl/$prj_path/$prj_name/
	set destdir $prj_name/$sol_name/syn/vhdl/
	set vhdFiles [glob -directory $destdir *.vhd]
	file delete {*}$vhdFiles
	set vhdFiles [glob -directory $sourcedir *.vhd]
	foreach file $vhdFiles {
	   file copy -force $file $destdir
	}
	set destdir $prj_name/$sol_name/impl/vhdl/
	set vhdFiles [glob -directory $destdir *.vhd]
	file delete {*}$vhdFiles
	set vhdFiles [glob -directory $sourcedir *.vhd]
	foreach file $vhdFiles {
	   file copy -force $file $destdir
	}
}


# C/VHDL cosimulation
if {$cosim == "yes"} { 
	set data_path ${sol_path}/sim/data
	puts $data_path
	if { ![file exists $data_path] } {
		file mkdir $data_path
	}
	set dat_file ../../../../ref_value/data/data_${top_module}.dat
	if { [file exists $dat_file] } {
		file copy -force $dat_file $data_path/
	}else {
		puts "ERROR : missing ref data file"
		exit 1
	}
	puts "------------------"
	puts "-- Co-simulation --"
	puts "------------------"
	cosim_design  -rtl $language -tool xsim
}

#VHDL synthesis, place/route
if {$implem == "yes"} { 
	puts "--------------------"
	puts "-- Implementation --"
	puts "--------------------"
	export_design -flow impl -rtl $language -format ip_catalog -description "$top_module" -vendor "ISAE" -display_name "$top_module"
}

exit 0
