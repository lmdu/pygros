#ifndef PYGROS_RANGES_H
#define PYGROS_RANGES_H
#include <Python.h>
#include "cgranges.h"

typedef struct {
	PyObject_HEAD

	//store intervals
	cgranges_t *ranges;

	//index has been build
	//int indexed;

} pygros_Ranges;

extern PyTypeObject pygros_RangesType;

#endif
