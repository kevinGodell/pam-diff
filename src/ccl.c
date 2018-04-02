#define CALL_LabelComponent(x,y,returnLabel) { STACK[SP] = x; STACK[SP+1] = y; STACK[SP+2] = returnLabel; SP += 3; goto START; }
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


void LabelComponent(unsigned short* STACK, unsigned short width, unsigned short height, unsigned char* input, unsigned int* output, int labelNo, unsigned short x, unsigned short y)
{
  STACK[0] = x;
  STACK[1] = y;
  STACK[2] = 0;  /* return - component is labelled */
  int SP   = 3;
  int index;

START: /* Recursive routine starts here */

  index = X + width*Y;
  if (input [index] == 0) RETURN;   /* This pixel is not part of a component */
  if (output[index] != 0) RETURN;   /* This pixel has already been labelled  */
  output[index] = labelNo;

  if (X > 0) CALL_LabelComponent(X-1, Y, 1);   /* left  pixel */
RETURN1:

  if (X < width-1) CALL_LabelComponent(X+1, Y, 2);   /* rigth pixel */
RETURN2:

  if (Y > 0) CALL_LabelComponent(X, Y-1, 3);   /* upper pixel */
RETURN3:

  if (Y < height-1) CALL_LabelComponent(X, Y+1, 4);   /* lower pixel */
RETURN4:

  RETURN;
}


void LabelImage(unsigned short width, unsigned short height, unsigned char* input, unsigned int* output, unsigned int& maxLabel)
{

//std::cout<<"passed max label "<<maxLabel<<std::endl;


  unsigned short* STACK = (unsigned short*) malloc(3*sizeof(unsigned short)*(width*height + 1));
  
  unsigned int labelNo = 0;
  int index   = -1;
  //unsigned int index = 0;
  for (unsigned short y = 0; y < height; y++)
  {
    for (unsigned short x = 0; x < width; x++/*, index++, ind++*/)
    {
      index++;
      if (input [index] == 0) continue;   /* This pixel is not part of a component */
      if (output[index] != 0) continue;   /* This pixel has already been labelled  */
      /* New component found */
      labelNo++;
      LabelComponent(STACK, width, height, input, output, labelNo, x, y);
    }
  }

  maxLabel = labelNo;

  free(STACK);
}
