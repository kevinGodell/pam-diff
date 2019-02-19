//#include <stdlib.h>
//#include <iostream>
//<stdint.h>
//#include <cstdint>

#define CALL_LabelComponent(x, y, returnLabel) {\
    STACK[SP] = x;\
    STACK[SP+1] = y;\
    STACK[SP+2] = returnLabel;\
    SP += 3;\
    goto START;\
}

#define RETURN {\
    SP -= 3;\
    switch (STACK[SP+2]) {\
        case 1 : goto RETURN1;\
        case 2 : goto RETURN2;\
        case 3 : goto RETURN3;\
        case 4 : goto RETURN4;\
        default: return;\
    }\
}

#define X (STACK[SP-3])

#define Y (STACK[SP-2])

void LabelComponent(unsigned short *STACK, unsigned int width, unsigned int height, /*const unsigned char *input, */ int labelNo, unsigned short x, unsigned short y, int *output) {
    //std::cout << "label component " << width << " " << height << " " << x << " " << y << std::endl;

    STACK[0] = x;
    STACK[1] = y;
    STACK[2] = 0;  /* return - component is labelled */
    int SP = 3;
    int index;

    START: /* Recursive routine starts here */

    //std::cout << "START" << std::endl;

    index = X + width * Y;

    //if (input[index] == 0) RETURN;   /* This pixel is not part of a component */
    //if (output[index] == -1) RETURN;//todo might not need this

    if (output[index] != 0) RETURN;   /* This pixel has already been labelled  */

    output[index] = labelNo;

    if (X > 0) CALL_LabelComponent(X - 1, Y, 1);   /* left  pixel */
    RETURN1:

    if (X < width - 1) CALL_LabelComponent(X + 1, Y, 2);   /* rigth pixel */
    RETURN2:

    if (Y > 0) CALL_LabelComponent(X, Y - 1, 3);   /* upper pixel */
    RETURN3:

    if (Y < height - 1) CALL_LabelComponent(X, Y + 1, 4);   /* lower pixel */
    RETURN4:

    RETURN;
}