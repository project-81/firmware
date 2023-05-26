let g:ale_c_cc_executable = 'arm-picolibc-eabi-gcc'
let g:ale_cpp_cc_executable = 'arm-picolibc-eabi-g++'

let common_opts = '-Isrc -Wall -Wextra -Wpedantic'

let g:ale_c_cc_options = '-std=gnu17 ' . common_opts
let g:ale_cpp_cc_options = '-std=gnu++20 ' . common_opts
