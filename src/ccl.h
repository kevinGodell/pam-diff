#ifndef SRC_CCL_H
#define SRC_CCL_H

#include <cstdint>
#include <vector>

void
LabelComponent(uint_fast32_t *stack, uint_fast32_t width, uint_fast32_t minX, uint_fast32_t maxX, uint_fast32_t minY, uint_fast32_t maxY, int_fast32_t labelNumber, uint_fast32_t x, uint_fast32_t y, int_fast32_t *labels);

// assign label value to each pixel, return number of labels (highest label number +1)
uint_fast32_t
LabelImage(uint_fast32_t width, uint_fast32_t height, uint_fast32_t minX, uint_fast32_t maxX, uint_fast32_t minY, uint_fast32_t maxY, std::vector<int_fast32_t> &labelsVec);

#endif //SRC_CCL_H
