require 'mkmf'

find_library('clang', 'clang_createIndex')

create_makefile("clang/clang")
