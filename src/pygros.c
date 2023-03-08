#include <Python.h>
#include "version.h"
#include "ranges.h"

static struct PyModuleDef module_pygros = {
	PyModuleDef_HEAD_INIT,
	"pygros",
	"A python package for finding genomic range overlaps",
	-1,
	NULL,
};

static PyObject *pygros_module_init(void) {
	PyObject *module;
	module = PyModule_Create(&module_pygros);

	if (module == NULL) {
		return NULL;
	}

	//add version
	if (PyModule_AddStringConstant(module, "__version__", PYGROS_VERSION) < 0) {
		return NULL;
	}

	//GRanges
	if (PyType_Ready(&pygros_RangesType) < 0) {
		return NULL;
	}
	Py_INCREF((PyObject *)&pygros_RangesType);
	PyModule_AddObject(module, "Ranges", (PyObject *)&pygros_RangesType);

	return module;
}

PyMODINIT_FUNC PyInit_pygros() {
	return pygros_module_init();
}
