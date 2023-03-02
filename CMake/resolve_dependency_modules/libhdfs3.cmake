# Copyright (c) Facebook, Inc. and its affiliates.
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
include_guard(GLOBAL)

if(DEFINED ENV{VELOX_LIBHDFS3_URL})
  set(LIBHDFS3_SOURCE_URL "$ENV{VELOX_LIBHDFS3_URL}")
else()
  set(VELOX_LIBHDFS3_BUILD_VERSION 3.0.0.0)
  string(CONCAT LIBHDFS3_SOURCE_URL
                "https://github.com/apache/hawq/archive/refs/tags/rel/"
                "v${VELOX_LIBHDFS3_BUILD_VERSION}.tar.gz")
  set(VELOX_LIBHDFS3_BUILD_SHA256_CHECKSUM
      9c86567bc949a764b07503f106ec2909542ed5449f56bbc8d5952347212eecb2)
endif()

message(STATUS "Building Libhdfs3 from source")

FetchContent_Declare(
  libhdfs3
  URL ${LIBHDFS3_SOURCE_URL}
  URL_HASH SHA256=${VELOX_LIBHDFS3_BUILD_SHA256_CHECKSUM}
  SOURCE_SUBDIR depends/libhdfs3)

FetchContent_MakeAvailable(libhdfs3)
