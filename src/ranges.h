#define PY_SSIZE_T_CLEAN
#ifndef PYGRANGES_RANGES_H
#define PYGRANGES_RANGES_H
#include "Python.h"
#include "cgranges.h"

typedef struct {
	PyObject_HEAD

	//store intervals
	cgranges_t *ranges;

	//index has been build
	int indexed;

} pygrange_Ranges;

extern PyTypeObject pygrange_RangesType;

#endif