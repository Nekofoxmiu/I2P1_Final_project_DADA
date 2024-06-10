#include "element.h"
#include <stdbool.h>
#include <math.h>
/*
   [Element function]
*/
Elements *New_Elements(int label)
{
    Elements *pObj;
    pObj = (Elements *)malloc(sizeof(Elements));
    pObj->inter_len = 0;
    pObj->dele = false;
    // setting object member
    pObj->label = label;
    pObj->pDerivedObj = pObj;
    return pObj;
}

void NormalizeV(double *dx, double *dy)
{
    double len = sqrt(*dx * *dx + *dy * *dy);
    if (len > 0)
    {
        *dx /= len;
        *dy /= len;
    }
    else
    {
        printf("Unit Vector With Len 0! Set with Default Value\n");
        *dx = 0.7;
        *dy = 0.7;
    }
}

