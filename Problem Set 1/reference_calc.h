#ifndef REFERENCE_H__
#define REFERENCE_H__

#include <stddef.h>
#include "custom_types.h"

void referenceCalculation(const uchar4* const rgbaImage,
                          unsigned char *const greyImage,
                          size_t numRows,
                          size_t numCols);

#endif
