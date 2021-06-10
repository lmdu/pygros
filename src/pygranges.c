#include <Python.h>
#include "version.h"
#include "ranges.h"

static struct PyModuleDef module_pygrange = {
	PyModuleDef_HEAD_INIT,
	"pygrange",
	"A python package for finding genomic range overlaps",
	-1,
	NULL,
};

static PyObject *pygrange_module_init(void) {
	PyObject *module;
	module = PyModule_Create(&module_pygrange);

	if (module == NULL) {
		return NULL;
	}

	//add version
	if (PyModule_AddStringConstant(module, "version", PYGRANGE_VERSION) < 0) {
		return NULL;
	}

	//GRanges
	if (PyType_Ready(&pygrange_RangesType) < 0) {
		return NULL;
	}
	Py_INCREF((PyObject *)&pygrange_RangesType);
	PyModule_AddObject(module, "Ranges", (PyObject *)&pygrange_RangesType);

	return module;
}

PyMODINIT_FUNC PyInit_pygrange() {
	return pygrange_module_init();
}
