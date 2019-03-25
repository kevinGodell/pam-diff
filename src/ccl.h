#ifndef SRC_CCL_H_
#define SRC_CCL_H_

#include "engine.h"
#include <cstdint>
#include <vector>

void
LabelComponent(uint_fast32_t *stack, const Dimensions &dimensions, const Bounds &bounds, int_fast32_t labelNumber, uint_fast32_t x, uint_fast32_t y, int_fast32_t *labels);

// assign label value to each pixel, return number of labels (highest label number +1)
uint_fast32_t
LabelImage(const Dimensions &dimensions, const Bounds &bounds, int_fast32_t *labels);

#endif //SRC_CCL_H
