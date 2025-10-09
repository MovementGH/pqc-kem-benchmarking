############################################################
## TCL script file for vivado HLS
## Project : HQC-FPGA
## For Vivado HLS 2020.1 and Vitis HLS 2020.2
##
## Do not modify
############################################################

proc set_directives {top_module version} {
    # {{{
	set dirfile [open "directives.tcl" w] 
	fconfigure $dirfile -translation crlf

	puts $dirfile "############################################################"
	puts $dirfile "## Auto-generated constraint file for vivado HLS"
	puts $dirfile "## Do not modify"
	puts $dirfile "############################################################"
	
	if {$top_module == "crypto_kem_all_functions"} {set ext hls} else {set ext internal}
	
	if {$version == "COMPACT"} { 
		puts $dirfile "set_directive_expression_balance -off ${top_module}_${ext}"
	}

	###########################################
	# Memory structure directives for perf arch
	###########################################
	if {$version == "PERF"} { 

		puts $dirfile {set_directive_array_partition -type complete -dim 1 "KeccakF1600_StatePermute" A}
		puts $dirfile {set_directive_array_partition -type complete -dim 1 "KeccakF1600_StatePermute" E}
		puts $dirfile {set_directive_array_partition -type complete -dim 1 "PermuteCompact" D}
		puts $dirfile {set_directive_array_partition -type cyclic -factor 8 -dim 1 "expand_and_sum" dst}
	}
	##############################################
	# Memory structure directives for compact arch
	##############################################

	if {$version == "COMPACT"} { 
		puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "PermuteCompact" BCD}
		puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "KeccakF1600_StatePermute" A}
		puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "KeccakF1600_StatePermute" E}
		puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "shake256_512_ds" state}
	}

	#################
	# Limits for area 
	#################

	puts $dirfile {set_directive_inline -off KeccakF1600_StatePermute}
	puts $dirfile {set_directive_inline -off vect_mul_add}
	puts $dirfile {set_directive_inline -off gf_inverse}
	puts $dirfile {set_directive_inline -off vect_add}
	puts $dirfile {set_directive_inline -off vect_copyresize}
	puts $dirfile {set_directive_inline -off vect_copyresize_mult}
	puts $dirfile {set_directive_inline -off vect_set_random_fixed_weight}
	puts $dirfile {set_directive_inline -off shake_prng}

	########################
	# Limits for parallelism 
	########################

	puts $dirfile {set_directive_inline -off vect_compare}
	puts $dirfile {set_directive_inline -off vect_compare_64}
	puts $dirfile {set_directive_inline -off hqc_public_key_from_string}
	puts $dirfile {set_directive_inline -off vector_init_64}
	puts $dirfile {set_directive_inline -off vect_set_random_fixed_weight_by_coordinates}
	if {$version == "PERF"} { 
		puts $dirfile {set_directive_inline -off vector_init}
	}

	#######################
	# Allocation directives
	#######################
	
	puts $dirfile "set_directive_allocation -limit 1 -type function \"${top_module}_${ext}\" vect_mul_add"
	puts $dirfile "set_directive_allocation -limit 1 -type function \"${top_module}_${ext}\" KeccakF1600_StatePermute"
	puts $dirfile "set_directive_allocation -limit 1 -type function \"${top_module}_${ext}\" hqc_public_key_from_string"
	puts $dirfile "set_directive_allocation -limit 1 -type function \"${top_module}_${ext}\" vect_add"
	puts $dirfile {set_directive_allocation -limit 1 -type function "hqc_pke_encrypt" vect_add}
	puts $dirfile {set_directive_allocation -limit 1 -type function "hqc_pke_encrypt" vect_mul_add}
	puts $dirfile "set_directive_allocation -limit 1 -type function \"${top_module}_${ext}\" vector_init_64"

	puts $dirfile "set_directive_allocation -limit 1 -type function \"${top_module}_${ext}\" reed_muller_encode"
	puts $dirfile "set_directive_allocation -limit 1 -type function \"${top_module}_${ext}\" reed_solomon_encode"
	puts $dirfile "set_directive_allocation -limit 1 -type function \"${top_module}_${ext}\" shake_prng"

	#############################################
	# Memory core directives (Structural choices)
	#############################################

	puts $dirfile "set_directive_resource -core RAM_2P_BRAM \"${top_module}_hls\" store_A"
	puts $dirfile "set_directive_resource -core RAM_2P_BRAM \"${top_module}_hls\" store_B" 
	puts $dirfile "set_directive_resource -core RAM_2P_BRAM \"${top_module}_hls\" h_s"
	puts $dirfile "set_directive_resource -core RAM_2P_BRAM \"${top_module}_hls\" ct"
	puts $dirfile "set_directive_resource -core RAM_2P_BRAM \"${top_module}_hls\" mc"
	puts $dirfile "set_directive_resource -core RAM_2P_BRAM \"${top_module}_hls\" pk64"
	puts $dirfile "set_directive_resource -core RAM_2P_BRAM \"${top_module}_hls\" sk64"
	puts $dirfile "set_directive_resource -core RAM_2P_LUTRAM \"${top_module}_hls\" state_seedexpander"
	puts $dirfile "set_directive_resource -core RAM_2P_LUTRAM \"${top_module}_hls\" state_prng"
	puts $dirfile "set_directive_resource -core RAM_2P_LUTRAM \"${top_module}_hls\" random_vector_A"
	puts $dirfile "set_directive_resource -core RAM_2P_LUTRAM \"${top_module}_hls\" random_vector_B"

	puts $dirfile {set_directive_resource -core RAM_2P_BRAM "crypto_kem_dec_internal" uv2}

	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "vect_mul" a2_large}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "vect_mul" res_mult}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "vect_mul_add" a2_large}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "vect_mul_add" res_mult}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "code_encode" store_rs_encode}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "shake256_512_ds" state}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "vect_set_random_fixed_weight_by_coordinates" rand_bytes}

	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "compute_elp" X_sigma_p}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "reed_muller_decode" expanded}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "reed_muller_decode" transform}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "add_fft_retrieve_error_poly" index}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "code_decode" store_rs_decode}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "add_fft" big_W}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "add_fft" _A}
	puts $dirfile {set_directive_resource -core RAM_2P_LUTRAM "add_fft" w16}

	##############################
	# Memory core directives (RAM)
	##############################

	puts $dirfile {set_directive_array_map -instance rs_mem_1 -mode horizontal reed_solomon_decode z}
	puts $dirfile {set_directive_array_map -instance rs_mem_1 -mode horizontal reed_solomon_decode sigma}
	puts $dirfile {set_directive_array_map -instance rs_mem_1 -mode horizontal reed_solomon_decode error}
	puts $dirfile {set_directive_array_map -instance rs_mem_1 -mode horizontal reed_solomon_decode error_values}
	puts $dirfile {set_directive_array_map -instance rs_mem_1 -mode horizontal reed_solomon_decode syndromes}
	puts $dirfile {set_directive_array_map -instance rs_mem_1 -mode horizontal compute_roots small_w}

	##############################
	# Memory core directives (ROM)
	##############################

	puts $dirfile {set_directive_resource -core ROM_nP_LUTRAM "add_fft" gammas_sums_R}
	puts $dirfile {set_directive_resource -core ROM_nP_LUTRAM "compute_error_values" exp_array}
	puts $dirfile {set_directive_resource -core ROM_1P_LUTRAM "Permute" KeccakF_RoundConstants}
	puts $dirfile {set_directive_resource -core ROM_1P_LUTRAM "PermuteCompact" KeccakF_RoundConstants}

	#######################################################
	# Inline for seedexpander register and permute function
	#######################################################

	# kem
	puts $dirfile {set_directive_inline "crypto_kem_keypair_internal"}
	puts $dirfile {set_directive_inline "crypto_kem_enc_internal"}
	puts $dirfile {set_directive_inline "crypto_kem_dec_internal"}

	# hqc
	puts $dirfile {set_directive_inline "hqc_pke_keygen"}
	puts $dirfile {set_directive_inline "hqc_pke_encrypt"}
	puts $dirfile {set_directive_inline "hqc_pke_decrypt"}

	# shake_prng
	puts $dirfile {set_directive_inline -off "shake_prng"}
	puts $dirfile {set_directive_inline -off "seedexpander_init"}
	puts $dirfile {set_directive_inline "seedexpander"}
	puts $dirfile {set_directive_inline -off "seedexpander_mult_type"}

	# vector
	puts $dirfile {set_directive_inline "vect_set_random_from_prng"}
	puts $dirfile {set_directive_inline "vect_set_random_mult_type"}

	# Keccak
	puts $dirfile {set_directive_inline "Keccak_Squeezeblocks_64_Inc"}
	puts $dirfile {set_directive_inline "Keccak_Stream_Absorb_With_DS_From_Byte_0"}

	###########################
	# Inline for area reduction
	###########################

	# KeccekF1600_permute
	puts $dirfile {set_directive_inline "PermuteCompact"}

	puts $dirfile {set_directive_inline "compute_error_values"}
	puts $dirfile {set_directive_inline "compute_syndromes"}
	puts $dirfile {set_directive_inline "compute_elp"}
	puts $dirfile {set_directive_inline "compute_roots"}
	puts $dirfile {set_directive_inline "compute_z_poly"}
	puts $dirfile {set_directive_inline "correct_errors"}
	puts $dirfile {set_directive_inline "encode_8bits"}

	puts $dirfile {set_directive_inline "naive_convolution_mult"}
	puts $dirfile {set_directive_inline "reduce_naive"}

	puts $dirfile {set_directive_inline "hqc_secret_key_from_string"}

	# code
	puts $dirfile {set_directive_inline "code_encode"}
	puts $dirfile {set_directive_inline "code_decode"}
	

	#######################################
	# Loop directives for latency reduction
	#######################################
	if {$version == "PERF"} { 
		# kem
		puts $dirfile {set_directive_pipeline -II 2 "crypto_kem_dec_internal/control_loop"}
	 
		# vector
		puts $dirfile {set_directive_pipeline "vect_set_random_fixed_weight_by_coordinates/memset"}
		puts $dirfile {set_directive_pipeline "vect_set_random_fixed_weight_by_coordinates/vect_coord3"}
		puts $dirfile {set_directive_pipeline -II 4 "vect_set_random_fixed_weight/vect_weight"}
		puts $dirfile {set_directive_pipeline "vect_add/vect_addi"}
		puts $dirfile {set_directive_pipeline "vect_copyresize/vect_rze_1"}
		puts $dirfile {set_directive_pipeline "vect_copyresize/vect_rze_2"}
		puts $dirfile {set_directive_pipeline "vect_copyresize_64/vect_rze_1"}
		puts $dirfile {set_directive_pipeline "vect_copyresize_64/vect_rze_2"}
		puts $dirfile {set_directive_pipeline -II 4 "vect_copyresize_mult/vect_rze_1"}
		puts $dirfile {set_directive_pipeline "vect_copyresize_mult/expand_loop"}
		puts $dirfile {set_directive_pipeline -II 1 "vect_compare/vect_comp"}
		puts $dirfile {set_directive_pipeline -II 2 "vect_compare_mult_type/vect_comp"}
		puts $dirfile {set_directive_pipeline -II 8 "vect_compare_64/vect_comp"}
		puts $dirfile {set_directive_pipeline "vect_compare_64/comp_shrink_loop"}
		puts $dirfile {set_directive_pipeline "vect_set_random_from_randombytes/copy_v64_loop"}
		puts $dirfile {set_directive_pipeline "vector_init/vector_init_vartype_loop"}
		puts $dirfile {set_directive_pipeline "vector_init_mult_type/vector_init_mult_type_loop"}
		puts $dirfile {set_directive_pipeline "vector_init_64/vector_init_kem_uint64_loop"}
		puts $dirfile {set_directive_pipeline "vect_set_random_from_prng/vect_set_random__shrink_loop"}
		
		# Keccak
		puts $dirfile {set_directive_pipeline -off "Keccak_Squeezeblocks/Keccak_Squeezeblocks_loop1"}
		puts $dirfile {set_directive_pipeline -II 2 "Keccak_Squeezeblocks/Keccak_Squeezeblocks_loop1_1"}
		puts $dirfile {set_directive_pipeline -II 2 "Keccak_Squeezeblocks_64/Keccak_Squeezeblocks_loop1_1"}
		puts $dirfile {set_directive_pipeline "Keccak_Squeezeblocks_64_Inc/Keccak_Squeezeblocks_64_loop1"}
		puts $dirfile {set_directive_pipeline -off "Keccak_Squeezeblocks_64_Inc/Keccak_Squeezeblocks_64_loop2"}
		puts $dirfile {set_directive_pipeline "Keccak_Squeezeblocks_64_Inc/Keccak_Squeezeblocks_64_loop2_1"}
		puts $dirfile {set_directive_pipeline "Keccak_Stream_Absorb_Finalize/Keccak_Stream_Absorb_Finalize_label0"}
		puts $dirfile {set_directive_pipeline "Keccak_Stream_Absorb_Aligned_Last_with_domain/Keccak_Stream_Absorb_Aligned_Last_with_domain_label1"}
		
		# shake_prng
		puts $dirfile {set_directive_pipeline "seedexpander_init/memset"}
		puts $dirfile {set_directive_pipeline "seedexpander/seedexpander_shrink_loop"}
		puts $dirfile {set_directive_pipeline "seedexpander_mult_type/seedexpander_shrink_loop"}
		puts $dirfile {set_directive_pipeline "shake256_512_ds/memset"}
		puts $dirfile {set_directive_pipeline "seedexpander_init/memset"}
		
		# HQC
		puts $dirfile {set_directive_pipeline "hqc_pke_encrypt/memset_seed_1"}
		puts $dirfile {set_directive_pipeline "hqc_pke_encrypt/memset_seed_2"}
		puts $dirfile {set_directive_pipeline "hqc_pke_decrypt/memset_seed"}
		
		# KeccakF1600_permute
		puts $dirfile {set_directive_unroll "KeccakF1600_StatePermute/swap_loop"}
		puts $dirfile {set_directive_unroll "PermuteCompact/BCl_loop"}
		puts $dirfile {set_directive_unroll "PermuteCompact/BCl_loop_2"}
		puts $dirfile {set_directive_unroll "PermuteCompact/D_loop"}
		puts $dirfile {set_directive_unroll "PermuteCompact/main_loop"}
		puts $dirfile {set_directive_unroll "PermuteCompact/BC_loop"}
		puts $dirfile {set_directive_unroll "PermuteCompact/E_loop"}
		puts $dirfile {set_directive_unroll "PermuteCompact/BC_loop_1"}
		puts $dirfile {set_directive_unroll "PermuteCompact/E_loop_1"}
		
		# gf
		puts $dirfile {set_directive_unroll "gf_square/squarestep"}
		puts $dirfile {set_directive_unroll "gf_clmul/multstep"}
		puts $dirfile {set_directive_unroll "gf_reduce/reductionstep"}
		puts $dirfile {set_directive_unroll "gf_inverse/gf_inverse_loop"}
		
		# gf2x
		puts $dirfile {set_directive_pipeline -II 4 "naive_convolution_mult/naive_convolution_mult1"}
		puts $dirfile {set_directive_pipeline -II 1 "vector_init_res_mult/memset_init_mult"}
		puts $dirfile {set_directive_pipeline -II 4 "vect_mul_add/store_loop"}
		puts $dirfile {set_directive_pipeline -II 32 "reduce_naive_add/reduce_1"}
		puts $dirfile {set_directive_unroll "vect_mul_add/a2_expand_loop"}
		puts $dirfile {set_directive_pipeline "reduce_naive_add/a_shrink_loop"}
		
		# reed_solomon
		puts $dirfile {set_directive_pipeline -II 48 "reed_solomon_encode/rs_encode1"}
		puts $dirfile {set_directive_pipeline -II 2 "reed_solomon_encode/rs_encode4"}
		puts $dirfile {set_directive_pipeline "reed_solomon_encode/rs_encode2"}
		puts $dirfile {set_directive_pipeline "reed_solomon_decode/memset_reed_solomon_decode"}
		puts $dirfile {set_directive_pipeline -II 2 "reed_solomon_decode/rs_decode"}

		puts $dirfile {set_directive_pipeline -II 4 "compute_syndromes/synd2"}

		puts $dirfile {set_directive_pipeline  "correct_errors/correct_err"}
		puts $dirfile {set_directive_pipeline -II 3 "compute_elp/elp1_2"}
		puts $dirfile {set_directive_pipeline -II 2 "compute_elp/elp1_3"}
		puts $dirfile {set_directive_pipeline -II 2 "compute_elp/elp1_4"}
		puts $dirfile {set_directive_pipeline "compute_elp/elp2"}
		puts $dirfile {set_directive_pipeline "compute_elp/elp3"}

		puts $dirfile {set_directive_pipeline -II 4 "compute_z_poly/z_poly3"}

		puts $dirfile {set_directive_pipeline -II 4 "compute_error_values/error_values1"}
		puts $dirfile {set_directive_pipeline -II 2 "compute_error_values/error_values2"}
		puts $dirfile {set_directive_pipeline -II 4 "compute_error_values/error_values4"}
		puts $dirfile {set_directive_pipeline -II 3 "compute_error_values/error_values7"}
		puts $dirfile {set_directive_pipeline -II 2 "compute_error_values/error_values5"}
		puts $dirfile {set_directive_pipeline -II 4 "compute_error_values/error_values6"}
		puts $dirfile {set_directive_pipeline -II 2 "correct_errors/correct_err"}

		puts $dirfile {set_directive_pipeline -II 2 "encode_8bits/encode_loop"}

		# reed_muller
		puts $dirfile {set_directive_pipeline -II 3 "expand_and_sum/exp_sum2"}
		puts $dirfile {set_directive_unroll "expand_and_sum/exp_sum3"}

		puts $dirfile {set_directive_pipeline -II 2 "hadamard_loop/hadamard_loop1"}
		puts $dirfile {set_directive_pipeline "find_peaks/find_peak"}
		puts $dirfile {set_directive_pipeline -II 2 "reed_muller_encode/rm_encode3"}

		# parsing
		puts $dirfile {set_directive_pipeline -II 2 "hqc_public_key_from_string/public_from1"}
		puts $dirfile {set_directive_pipeline "hqc_secret_key_to_string/secret_to1"}

		puts $dirfile {set_directive_pipeline -II 8 "hqc_public_key_to_string/public_to1"}
		puts $dirfile {set_directive_pipeline "hqc_public_key_to_string/pk_expand_loop"}

		puts $dirfile {set_directive_pipeline "hqc_ciphertext_to_string/ct_to1"}
		puts $dirfile {set_directive_pipeline "hqc_ciphertext_to_string/ct_to2"}
		puts $dirfile {set_directive_pipeline "hqc_ciphertext_to_string/ct_to3"}
		puts $dirfile {set_directive_pipeline "hqc_ciphertext_to_string_64/ct_to1"}
		puts $dirfile {set_directive_pipeline "hqc_ciphertext_to_string_64/ct_to2"}
		puts $dirfile {set_directive_pipeline -II 8 "hqc_ciphertext_to_string_64/ct_to3"}
		puts $dirfile {set_directive_pipeline "hqc_ciphertext_to_string_64/ct_to3_shrink_loop"}

		puts $dirfile {set_directive_pipeline -II 1 "hqc_ciphertext_from_string/ct_from1"}
		puts $dirfile {set_directive_pipeline "hqc_ciphertext_from_string/ct_from2"}
		puts $dirfile {set_directive_pipeline "hqc_ciphertext_from_string_64/ct_from1"}
		puts $dirfile {set_directive_pipeline "hqc_ciphertext_from_string_64/ct_from2"}
		puts $dirfile {set_directive_pipeline -II 8 "hqc_ciphertext_from_string_64/ct_from3"}
		puts $dirfile {set_directive_pipeline "hqc_ciphertext_from_string_64/ct_from3_expand_loop"}

		# add_fft
		puts $dirfile {set_directive_pipeline -II 6 "add_fft_retrieve_error_poly/retreive_error"}

		puts $dirfile {set_directive_pipeline -II 2 "leaves_butterfly/loop_small_w_1"}
		puts $dirfile {set_directive_pipeline "leaves_butterfly/loop_small_w"} 
		puts $dirfile {set_directive_pipeline "leaves_butterfly/loop_gf_mul"}

		puts $dirfile {set_directive_pipeline "add_fft/memset_A"}
		puts $dirfile {set_directive_pipeline "add_fft/memset_a_fr_1_1"}
		puts $dirfile {set_directive_pipeline "add_fft/memset_a_fr_1_2"}
		puts $dirfile {set_directive_pipeline -II 7 "add_fft/round_1_4"}
		puts $dirfile {set_directive_pipeline "add_fft/Butterfly_1"}
		puts $dirfile {set_directive_pipeline "add_fft/Butterfly_2"}
		puts $dirfile {set_directive_pipeline -II 6 "add_fft/Butterfly_1_1"}
		puts $dirfile {set_directive_pipeline "add_fft/small_w_loop"}
		puts $dirfile {set_directive_pipeline "add_fft/round_1"}
	}
	####################################
	# directives for report purpose only
	####################################

	# Keccak
	puts $dirfile {set_directive_loop_tripcount -min 4 -max 555 "Keccak_Stream_Absorb_With_DS_From_Byte_0/sha_loop1"}
	puts $dirfile {set_directive_loop_tripcount -max 16 "Keccak_Squeezeblocks_64_Inc/Keccak_Squeezeblocks_64_loop1"}
	puts $dirfile {set_directive_loop_tripcount -max 5 "Keccak_Squeezeblocks_64_Inc/Keccak_Squeezeblocks_64_loop2"}

	# shake_prng
	puts $dirfile {set_directive_loop_tripcount -min 1 -max 2209 "seedexpander/seedexpander_main_loop"}
	puts $dirfile {set_directive_loop_tripcount -min 1 -max 2209 "seedexpander_mult_type/seedexpander_main_loop"}

	# vector
	puts $dirfile {set_directive_loop_tripcount -min 67 -max 77 "vect_set_random_fixed_weight_by_coordinates/vect_coord1"}
	puts $dirfile {set_directive_loop_tripcount -min 1 -max 1 "vect_set_random_fixed_weight_by_coordinates/vect_coord2"}
	puts $dirfile {set_directive_loop_tripcount -min 0 -max 77 "vect_set_random_fixed_weight_by_coordinates/vect_coord3"}

	#reed_solomon
	puts $dirfile {set_directive_loop_tripcount -min 1 -max 24 "compute_elp/elp1_2"}
	puts $dirfile {set_directive_loop_tripcount -min 1 -max 24 "compute_elp/elp3"}

	close $dirfile
# }}}
}
