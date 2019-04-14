//based on article @ https://www.codeproject.com/Articles/825200/%2FArticles%2F825200%2FAn-Implementation-Of-The-Connected-Component-Label
#include "ccl.h"
#include "engine.h"
#include <cstdint>
#include <memory>

#define CALL_LabelComponent(x, y, returnLabel) {\
            stack[stackIndex] = x;\
            stack[stackIndex+1] = y;\
            stack[stackIndex+2] = returnLabel;\
            stackIndex += 3;\
            goto START;\
        }

#define RETURN {\
            stackIndex -= 3;\
            switch (stack[stackIndex+2]) {\
                case 1 : goto RETURN1;\
                case 2 : goto RETURN2;\
                case 3 : goto RETURN3;\
                case 4 : goto RETURN4;\
                default: return;\
            }\
        }

#define X (stack[stackIndex-3])

#define Y (stack[stackIndex-2])

void
LabelComponent(const Config &config, const Bounds &bounds, const int32_t labelNumber, const uint32_t x, const uint32_t y, uint32_t *stack, int32_t *labels) {
    stack[0] = x;
    stack[1] = y;
    stack[2] = 0;
    uint32_t stackIndex = 3;
    uint32_t labelsIndex;

    START:// recursive routine starts here
    labelsIndex = config.width * Y + X;
    if (labels[labelsIndex] != -1) RETURN;// pixel is ignored(-2) or previously labelled(>= 0)
    labels[labelsIndex] = labelNumber;
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
uint32_t
LabelImage(const Config &config, const Bounds &bounds, int32_t *labels) {

    // have unique_ptr reserve memory for stack array on heap and manage destruction
    std::unique_ptr<uint32_t[]> up(new uint32_t[3 * (config.pixelCount + 1)]);

    // get pointer
    auto *stack = up.get();

    // label number
    int32_t labelNumber = -1;

    for (uint32_t y = bounds.minY; y <= bounds.maxY; ++y) {

        for (uint32_t x = bounds.minX, p = y * config.width + x; x <= bounds.maxX; ++x, ++p) {

            // ignored == -2, unlabeled == -1, labeled >= 0
            if (labels[p] != -1) continue;// pixel does not need to be labelled

            // increment label for next blob
            ++labelNumber;

            // send to C function for recursive labelling
            LabelComponent(config, bounds, labelNumber, x, y, stack, labels);

        }
    }

    return static_cast<uint32_t>(labelNumber + 1);
}