//credit: based on article @ https://www.codeproject.com/Articles/825200/%2FArticles%2F825200%2FAn-Implementation-Of-The-Connected-Component-Label
#define CALL_LabelComponent(x, y, returnLabel) { STACK[SP] = x; STACK[SP+1] = y; STACK[SP+2] = returnLabel; SP += 3; goto START; }
#define RETURN { SP -= 3;                \
                 switch (STACK[SP+2])    \
                 {                       \
                 case 1 : goto RETURN1;  \
                 case 2 : goto RETURN2;  \
                 case 3 : goto RETURN3;  \
                 case 4 : goto RETURN4;  \
                 default: return;        \
                 }                       \
               }
#define X (STACK[SP-3])
#define Y (STACK[SP-2])

inline void LabelComponent(uint_fast32_t *STACK, uint_fast32_t width, uint_fast32_t height, const bool *input, uint_fast32_t *output, uint_fast32_t label, uint_fast32_t x, uint_fast32_t y) {
    STACK[0] = x;
    STACK[1] = y;
    /* return - component is labelled */
    STACK[2] = 0;
    uint_fast32_t SP = 3;
    uint_fast32_t i = 0;
    /* Recursive routine starts here */
    START:
    i = X + width * Y;
    /* This pixel is not part of a component || This pixel has already been labelled */
    if (input[i] == false || output[i] != 0) RETURN;
    output[i] = label;
    /* left  pixel */
    if (X > 0) CALL_LabelComponent(X - 1, Y, 1);
    RETURN1:
    /* right pixel */
    if (X < width - 1) CALL_LabelComponent(X + 1, Y, 2);
    RETURN2:
    /* upper pixel */
    if (Y > 0) CALL_LabelComponent(X, Y - 1, 3);
    RETURN3:
    /* lower pixel */
    if (Y < height - 1) CALL_LabelComponent(X, Y + 1, 4);
    RETURN4:
    RETURN;
}

inline uint_fast32_t LabelImage(uint_fast32_t width, uint_fast32_t height, const bool *input, uint_fast32_t *output) {
    //std::cout<<"passed max label "<<maxLabel<<std::endl;
    uint_fast32_t *STACK = (uint_fast32_t *) malloc(3 * sizeof(uint_fast32_t) * (width * height + 1));
    uint_fast32_t label = 0;
    for (uint_fast32_t y = 0, x = 0, i = 0; y < height; y++) {
        for (x = 0; x < width; x++, i++) {
            /* This pixel is not part of a component || This pixel has already been labeled  */
            if (input[i] == false || output[i] != 0) continue;
            /* New component found */
            label++;
            LabelComponent(STACK, width, height, input, output, label, x, y);
        }
    }
    free(STACK);
    return label + 1;
}