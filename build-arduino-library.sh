#!/bin/bash

# Copyright 2020 Lucas Hinderberger
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


# This script transforms libvtp into a structure that is appropriate for
# use as an Arduino library.

build_dir=arduino

# Prepare build environment
if [ -d "$build_dir" ]; then
  rm -r $build_dir || exit
fi

mkdir $build_dir || exit
mkdir "$build_dir/source" || exit

# Flatten directory structure
cp include/vtp/* "$build_dir/source"
cp src/* "$build_dir/source"
cp LICENSE "$build_dir/source"
cp NOTICE "$build_dir/source"

# Continue work in source directory
cd "$build_dir/source" || exit

# Add prefixes to filenames
for filename in *.c *.h; do
  mv "$filename" "vtp_$filename";
done;

# Adapt include paths
sed -i 's/#include <vtp\//#include <vtp_/g' *.c *.h

# Rename .c to .cpp
for filename in *.c; do
  mv "$filename" "$filename"pp;
done;

# Add notice of modification to source files
sed -i '1s/^/\/\* This file has been modified for distribution as an Arduino library \*\/\n\n/' *.cpp *.h

# Build zipfile for library
zip ../libvtp-arduino.zip *

echo "Arduino library built to $build_dir/ directory"
