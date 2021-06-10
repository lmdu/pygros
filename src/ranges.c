#define PY_SSIZE_T_CLEAN
#include "ranges.h"

static PyObject *pygrange_ranges_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
	pygrange_Ranges *obj = (pygrange_Ranges *)type->tp_alloc(type, 0);

	if (!obj) return NULL;

	obj->ranges = cr_init();
	obj->indexed = 0;

	return (PyObject *)obj;
}

static void pygrange_ranges_dealloc(pygrange_Ranges *self) {
	cr_destroy(self->ranges);
}

static PyObject *pygrange_ranges_repr(pygrange_Ranges *self) {
	if (self->ranges->n_r) {
		return PyUnicode_FromFormat("<Ranges> contains %zd ranges from %d chroms",
									self->ranges->n_r, self->ranges->n_ctg);
	} else {
		return PyUnicode_FromString("<Ranges> empty");
	}
	
}

static int64_t pygrange_ranges_length(pygrange_Ranges *self) {
	return self->ranges->n_r;
}

int pygrange_ranges_contains(pygrange_Ranges *self, PyObject *key) {
	const char *chrom = PyUnicode_AsUTF8(key);
	return cr_get_ctg(self->ranges, chrom) == -1? 0 : 1;
}

static PyObject *pygrange_ranges_add(pygrange_Ranges *self, PyObject *args, PyObject *kwargs) {
	const char *chrom;
	int start;
	int end;
	int label = -1;

	static char* keywords[] = {"chrom", "start", "end", "label", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sii|i", keywords, &chrom, &start, &end, &label)) {
		return NULL;
	}

	if (!self->indexed) {
		cr_add(self->ranges, chrom, start, end, label);
		Py_RETURN_NONE;
	} else {
		PyErr_SetString(PyExc_Exception, "Could not add interval after indexing");
		return NULL;
	}
}

static PyObject *pygrange_ranges_index(pygrange_Ranges *self) {
	if (!self->indexed) {
		cr_index(self->ranges);
		self->indexed = 1;
	}

	Py_RETURN_NONE;
}

static PyObject *pygrange_ranges_overlap(pygrange_Ranges *self, PyObject *args, PyObject *kwargs) {
	const char *chrom;
	int start;
	int end;

	static char* keywords[] = {"chrom", "start", "end", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sii", keywords, &chrom, &start, &end)) {
		return NULL;
	}

	Py_ssize_t i, n, *b = 0, max_b = 0;
	n = cr_overlap(self->ranges, chrom, start, end, &b, &max_b);

	PyObject *list = PyList_New(0);

	for (i = 0; i < n; ++i) {
		PyObject *it = Py_BuildValue("iii", cr_start(self->ranges, b[i]),
										cr_end(self->ranges, b[i]),
										cr_label(self->ranges, b[i]));

		PyList_Append(list, it);
		Py_DECREF(it);
	}

	free(b);
	return list;
}

static PyObject *pygrange_ranges_contain(pygrange_Ranges *self, PyObject *args, PyObject *kwargs) {
	const char *chrom;
	int start;
	int end;

	static char* keywords[] = {"chrom", "start", "end", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sii", keywords, &chrom, &start, &end)) {
		return NULL;
	}

	Py_ssize_t i, n, *b = 0, max_b = 0;
	n = cr_contain(self->ranges, chrom, start, end, &b, &max_b);

	PyObject *list = PyList_New(0);

	for (i = 0; i < n; ++i) {
		PyObject *it = Py_BuildValue("iii", cr_start(self->ranges, b[i]),
										cr_end(self->ranges, b[i]),
										cr_label(self->ranges, b[i]));

		PyList_Append(list, it);
		Py_DECREF(it);
	}

	free(b);
	return list;
}

static PyObject *pygrange_ranges_locate(pygrange_Ranges *self, PyObject *args, PyObject *kwargs) {
	const char *chrom;
	int start;
	int end;

	static char* keywords[] = {"chrom", "start", "end", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "sii", keywords, &chrom, &start, &end)) {
		return NULL;
	}

	Py_ssize_t i, n, *b = 0, max_b = 0;
	n = cr_locate(self->ranges, chrom, start, end, &b, &max_b);

	PyObject *list = PyList_New(0);

	for (i = 0; i < n; ++i) {
		PyObject *it = Py_BuildValue("iii", cr_start(self->ranges, b[i]),
										cr_end(self->ranges, b[i]),
										cr_label(self->ranges, b[i]));

		PyList_Append(list, it);
		Py_DECREF(it);
	}

	free(b);
	return list;
}

static PySequenceMethods pygrange_ranges_as_sequence = {
	(lenfunc)pygrange_ranges_length, /*sq_length*/
	0, /*sq_concat*/
	0, /*sq_repeat*/
	0, /*sq_item*/
	0, /*sq_slice */
	0, /*sq_ass_item*/
	0, /*sq_ass_splice*/
	(objobjproc)pygrange_ranges_contains, /*sq_contains*/
	0, /*sq_inplace_concat*/
	0, /*sq_inplace_repeat*/
};

static PyMethodDef pygrange_ranges_methods[] = {
	{"add", (PyCFunction)pygrange_ranges_add, METH_VARARGS|METH_KEYWORDS, NULL},
	{"index", (PyCFunction)pygrange_ranges_index, METH_NOARGS, NULL},
	{"overlap", (PyCFunction)pygrange_ranges_overlap, METH_VARARGS|METH_KEYWORDS, NULL},
	{"contain", (PyCFunction)pygrange_ranges_contain, METH_VARARGS|METH_KEYWORDS, NULL},
	{"locate", (PyCFunction)pygrange_ranges_locate, METH_VARARGS|METH_KEYWORDS, NULL},
	{NULL, NULL, 0, NULL}
};

PyTypeObject pygrange_RangesType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "Ranges",                        /* tp_name */
    sizeof(pygrange_Ranges),          /* tp_basicsize */
    0,                              /* tp_itemsize */
    (destructor)pygrange_ranges_dealloc,   /* tp_dealloc */
    0,                              /* tp_print */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
    0,                              /* tp_reserved */
    (reprfunc)pygrange_ranges_repr,                              /* tp_repr */
    0,                              /* tp_as_number */
    &pygrange_ranges_as_sequence,                   /* tp_as_sequence */
    0,                   /* tp_as_mapping */
    0,                              /* tp_hash */
    0,                              /* tp_call */
    0,                              /* tp_str */
    0,                              /* tp_getattro */
    0,                              /* tp_setattro */
    0,                              /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,             /* tp_flags */
    "Ranges",                              /* tp_doc */
    0,                              /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    0,     /* tp_iter */
    0,    /* tp_iternext */
    pygrange_ranges_methods,          /* tp_methods */
    0,          /* tp_members */
    0,                               /* tp_getset */
    0,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    0,                              /* tp_init */
    0,            /* tp_alloc */
    pygrange_ranges_new,              /* tp_new */
};
