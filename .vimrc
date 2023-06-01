" Uncomment when working on embedded.
" let g:ale_c_cc_executable = 'arm-picolibc-eabi-gcc'
" let g:ale_cpp_cc_executable = 'arm-picolibc-eabi-g++'

" -Wpedantic
let common_opts = '-Isrc'
let common_opts .= ' -Wall'
let common_opts .= ' -Wextra'
let common_opts .= ' -Wno-psabi'

function! ThirdPartyIncludePath(subdir)
	return ' -Isrc/third-party/' . a:subdir
endfunction

function! PicoSdkInclude(path)
	return ThirdPartyIncludePath('pico-sdk/src/' . a:path . '/include')
endfunction

let pico_sdk_paths = [
\	'boards',
\	'common/pico_base',
\	'rp2_common/cmsis',
\	'rp2_common/pico_platform',
\	'rp2_common/hardware_base',
\	'rp2_common/hardware_uart',
\	'rp2040/hardware_regs',
\	'rp2040/hardware_structs',
\]

" pico-sdk makes the usual unfortunate choices with source organization
" and include-path cuteness.
for include in pico_sdk_paths
	let common_opts .= PicoSdkInclude(include)
endfor

let third_party_paths = ['json/single_include']
let third_party_paths = []

for include in third_party_paths
	let common_opts .= ThirdPartyIncludePath(include)
endfor

let g:ale_c_cc_options = '-std=gnu17 ' . common_opts
let g:ale_cpp_cc_options = '-std=gnu++20 ' . common_opts
