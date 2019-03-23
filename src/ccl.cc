//based on article @ https://www.codeproject.com/Articles/825200/%2FArticles%2F825200%2FAn-Implementation-Of-The-Connected-Component-Label
#include "ccl.h"
#include "engine.h"
#include <cstdint>
#include <memory>
#include <vector>

#define CALL_LabelComponent(x, y, returnLabel) {\
            stack[stackPointer] = x;\
            stack[stackPointer+1] = y;\
            stack[stackPointer+2] = returnLabel;\
            stackPointer += 3;\
            goto START;\
        }

#define RETURN {\
            stackPointer -= 3;\
            switch (stack[stackPointer+2]) {\
                case 1 : goto RETURN1;\
                case 2 : goto RETURN2;\
                case 3 : goto RETURN3;\
                case 4 : goto RETURN4;\
                default: return;\
            }\
        }

#define X (stack[stackPointer-3])

#define Y (stack[stackPointer-2])

void
LabelComponent(uint_fast32_t *stack, const Dimensions &dimensions, const Bounds &bounds, const int_fast32_t labelNumber, const uint_fast32_t x, const uint_fast32_t y, int_fast32_t *labels) {
    stack[0] = x;
    stack[1] = y;
    stack[2] = 0;
    int_fast32_t stackPointer = 3;
    int_fast32_t index;

    START:// recursive routine starts here
    index = dimensions.width * Y + X;
    if (labels[index] != -1) RETURN;// pixel is ignored(-2) or previously labelled(>= 0)
    labels[index] = labelNumber;
    if (X > bounds.minX) CALL_LabelComponent(X - 1, Y, 1);// left  pixel

    RETURN1:
    if (X < bounds.maxX) CALL_LabelComponent(X + 1, Y, 2);// right pixel

    RETURN2:
    if (Y > bounds.minY) CALL_LabelComponent(X, Y - 1, 3);// top pixel

    RETURN3:
    if (Y < bounds.maxY) CALL_LabelComponent(X, Y + 1, 4);// bottom pixel

    RETURN4:
    RETURN;
}

// assign label value to each pixel, return number of labels (highest label number +1)
uint_fast32_t
LabelImage(const Dimensions &dimensions, const Bounds &bounds, std::vector<int_fast32_t> &labelsVec) {

    std::unique_ptr<uint_fast32_t[]> stack(new uint_fast32_t[3 * (dimensions.pixelCount + 1)]);

    // label number
    int_fast32_t labelNumber = -1;

    for (uint_fast32_t y = bounds.minY; y <= bounds.maxY; ++y) {

        for (uint_fast32_t x = bounds.minX, p = y * dimensions.width + x; x <= bounds.maxX; ++x, ++p) {

            // ignored == -2, unlabeled == -1, labeled >= 0
            if (labelsVec[p] != -1) continue;// pixel does not need to be labelled

            // increment label for next blob
            ++labelNumber;

            // send to C function for recursive labelling
            LabelComponent(stack.get(), dimensions, bounds, labelNumber, x, y, labelsVec.data());

        }
    }

    return static_cast<uint_fast32_t>(labelNumber + 1);
}

// assign label value to each pixel, return number of labels (highest label number +1)
uint_fast32_t
LabelImage(const Dimensions &dimensions, const Bounds &bounds, int_fast32_t *labels) {

    std::unique_ptr<uint_fast32_t[]> stack(new uint_fast32_t[3 * (dimensions.pixelCount + 1)]);

    uint_fast32_t *stackPtr = stack.get();

    // label number
    int_fast32_t labelNumber = -1;

    for (uint_fast32_t y = bounds.minY; y <= bounds.maxY; ++y) {

        for (uint_fast32_t x = bounds.minX, p = y * dimensions.width + x; x <= bounds.maxX; ++x, ++p) {

            // ignored == -2, unlabeled == -1, labeled >= 0
            if (labels[p] != -1) continue;// pixel does not need to be labelled

            // increment label for next blob
            ++labelNumber;

            // send to C function for recursive labelling
            LabelComponent(stackPtr, dimensions, bounds, labelNumber, x, y, labels);

        }
    }

    return static_cast<uint_fast32_t>(labelNumber + 1);
}