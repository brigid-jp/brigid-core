#! /bin/sh -e

# Copyright (c) 2019 <dev@brigid.jp>
# This software is released under the MIT License.
# https://opensource.org/licenses/mit-license.php

for i in ax_check_openssl.m4 ax_cxx_compile_stdcxx.m4 ax_lua.m4
do
  curl -L "https://git.savannah.gnu.org/gitweb/?p=autoconf-archive.git;a=blob_plain;f=m4/$i" >"$i"
done
