/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "velox/common/base/Nulls.h"
#include "velox/vector/BaseVector.h"
#include "velox/vector/SelectivityVector.h"

namespace facebook::velox::functions {

/// This function returns a SelectivityVector for ARRAY/MAP vectors that selects
/// all rows corresponding to the specified rows. This flavor is intended for
/// use with the base vectors of decoded vectors. Use `nulls` and `rowMapping`
/// to pass in the null and index mappings from the DecodedVector.
template <typename T>
SelectivityVector toElementRows(
    vector_size_t size,
    const SelectivityVector& topLevelRows,
    const T* arrayBaseVector,
    const uint64_t* nulls,
    const vector_size_t* rowMapping) {
  VELOX_CHECK(
      arrayBaseVector->encoding() == VectorEncoding::Simple::MAP ||
      arrayBaseVector->encoding() == VectorEncoding::Simple::ARRAY);

  auto rawSizes = arrayBaseVector->rawSizes();
  auto rawOffsets = arrayBaseVector->rawOffsets();
  VELOX_DEBUG_ONLY const auto sizeRange =
      arrayBaseVector->sizes()->template asRange<vector_size_t>().end();
  VELOX_DEBUG_ONLY const auto offsetRange =
      arrayBaseVector->offsets()->template asRange<vector_size_t>().end();

  SelectivityVector elementRows(size, false);
  topLevelRows.applyToSelected([&](vector_size_t row) {
    auto index = rowMapping ? rowMapping[row] : row;
    if (nulls && bits::isBitNull(nulls, row)) {
      return;
    }

    VELOX_DCHECK_LE(index, sizeRange);
    VELOX_DCHECK_LE(index, offsetRange);

    auto size = rawSizes[index];
    auto offset = rawOffsets[index];
    elementRows.setValidRange(offset, offset + size, true);
  });
  elementRows.updateBounds();
  return elementRows;
}

/// This function returns a SelectivityVector for ARRAY/MAP vectors that selects
/// all rows corresponding to the specified rows.
template <typename T>
SelectivityVector toElementRows(
    vector_size_t size,
    const SelectivityVector& topLevelRows,
    const T* arrayBaseVector) {
  return toElementRows(
      size,
      topLevelRows,
      arrayBaseVector,
      arrayBaseVector->rawNulls(),
      nullptr);
}

/// Returns a buffer of vector_size_t that represents the mapping from
/// topLevelRows's element rows to its top-level rows. For example, suppose
/// `result` is the returned buffer, result[i] == j means the value at index i
/// in the element vector belongs to row j of the top-level vector. topLevelRows
/// must be non-null rows.
BufferPtr getElementToTopLevelRows(
    vector_size_t numElements,
    const SelectivityVector& topLevelRows,
    const vector_size_t* rawOffsets,
    const vector_size_t* rawSizes,
    const uint64_t* rawNulls,
    memory::MemoryPool* pool);

template <typename T>
BufferPtr getElementToTopLevelRows(
    vector_size_t numElements,
    const SelectivityVector& topLevelRows,
    const T* topLevelVector,
    memory::MemoryPool* pool) {
  auto rawNulls = topLevelVector->rawNulls();
  auto rawSizes = topLevelVector->rawSizes();
  auto rawOffsets = topLevelVector->rawOffsets();

  return getElementToTopLevelRows(
      numElements, topLevelRows, rawOffsets, rawSizes, rawNulls, pool);
}

} // namespace facebook::velox::functions
