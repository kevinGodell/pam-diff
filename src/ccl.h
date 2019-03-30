#ifndef SRC_CCL_H_
#define SRC_CCL_H_

#include "engine.h"
#include <cstdint>

/*inline uint_fast32_t
getX(uint_fast32_t *stack, uint_fast32_t stackPointer) {
    return stack[stackPointer - 3];
}

inline uint_fast32_t
getY(uint_fast32_t *stack, uint_fast32_t stackPointer) {
    return stack[stackPointer - 2];
}*/

void
LabelComponent(uint_fast32_t *stack, const Config &config, const Bounds &bounds, int_fast32_t labelNumber, uint_fast32_t x, uint_fast32_t y, int_fast32_t *labels);

// assign label value to each pixel, return number of labels (highest label number +1)
uint_fast32_t
LabelImage(const Config &config, const Bounds &bounds, int_fast32_t *labels);

#endif //SRC_CCL_H
