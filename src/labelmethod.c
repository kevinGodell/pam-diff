#include <stdint.h>

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

void LabelComponent(uint_fast32_t *stack, uint_fast32_t width, const uint_fast32_t minX, const uint_fast32_t maxX, const uint_fast32_t minY, const uint_fast32_t maxY, int_fast32_t labelNumber, uint_fast32_t x, uint_fast32_t y, int_fast32_t *labels) {
    stack[0] = x;
    stack[1] = y;
    stack[2] = 0;
    int_fast32_t stackPointer = 3;
    int_fast32_t index;

    START:// recursive routine starts here
    index = width * Y + X;
    if (labels[index] != -1) RETURN;// pixel is ignored(-1) or previously labelled(> 0)
    labels[index] = labelNumber;
    if (X > minX) CALL_LabelComponent(X - 1, Y, 1);// left  pixel

    RETURN1:
    if (X < maxX) CALL_LabelComponent(X + 1, Y, 2);// right pixel

    RETURN2:
    if (Y > minY) CALL_LabelComponent(X, Y - 1, 3);// top pixel

    RETURN3:
    if (Y < maxY) CALL_LabelComponent(X, Y + 1, 4);// bottom pixel

    RETURN4:
    RETURN;
}