#ifndef SRC_CCL_H_
#define SRC_CCL_H_

#include "engine.h"
#include <cstdint>

/*inline uint32_t
getX(uint32_t *stack, uint32_t stackPointer) {
    return stack[stackPointer - 3];
}

inline uint32_t
getY(uint32_t *stack, uint32_t stackPointer) {
    return stack[stackPointer - 2];
}*/

void
LabelComponent(uint32_t *stack, const Config &config, const Bounds &bounds, int32_t labelNumber, uint32_t x, uint32_t y, int32_t *labels);

// assign label value to each pixel, return number of labels (highest label number +1)
uint32_t
LabelImage(const Config &config, const Bounds &bounds, int32_t *labels);

#endif //SRC_CCL_H
