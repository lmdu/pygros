#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "ranges.h"

static PyObject *pygros_ranges_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
	pygros_Ranges *obj = (pygros_Ranges *)type->tp_alloc(type, 0);

	Py_ssize_t i, n, c;
	Py_ssize_t start, end, label;

	const char *chrom;

	PyObject *intervals = NULL;
	PyObject *interval;

	static char* keywords[] = {"intervals", NULL};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", keywords, &intervals)) {
		return NULL;
	}

	if (!obj) return NULL;

	obj->ranges = cr_init();
	//obj->indexed = 0;

	if (intervals) {
		if (PySequence_Check(intervals)) {
			n = PySequence_Size(intervals);

			for (i = 0; i < n; ++i) {
				interval = PySequence_GetItem(intervals, i);
				c = PySequence_Size(interval);

				if (c < 3) {
					PyErr_SetString(PyExc_ValueError, "the interval must contain chrom name, start and end position");
					return NULL;
				}

				chrom = PyUnicode_AsUTF8(PySequence_GetItem(interval, 0));
				start = PyLong_AsSsize_t(PySequence_GetItem(interval, 1));
				end = PyLong_AsSsize_t(PySequence_GetItem(interval, 2));

				if (c > 3) {
					label = PyLong_AsSsize_t(PySequence_GetItem(interval, 3));
				} else {
					label = -1;
				}

				cr_add(obj->ranges, chrom, start, end, label);
			}

			cr_index(obj->ranges);
			//obj->indexed = 1;
		} else {
			PyErr_SetString(PyExc_TypeError, "the intervals must be list or tuple object");
			return NULL;
		}
	}

	return (PyObject *)obj;
}

static void pygros_ranges_dealloc(pygros_Ranges *self) {
	cr_destroy(self->ranges);
}

static PyObject *pygros_ranges_repr(pygros_Ranges *self) {
	if (self->ranges->n_r) {
		return PyUnicode_FromFormat("<Ranges> contains %zd ranges", self->ranges->n_r);
	} else {
		return PyUnicode_FromString("<Ranges> contains no ranges");
	}
}

static Py_ssize_t pygros_ranges_length(pygros_Ranges *self) {
	return self->ranges->n_r;
}

int pygros_ranges_contains(pygros_Ranges *self, PyObject *key) {
	const char *chrom = PyUnicode_AsUTF8(key);
	return cr_get_ctg(self->ranges, chrom) == -1 ? 0 : 1;
}

static PyObject *pygros_ranges_add(pygros_Ranges *self, PyObject *args, PyObject *kwargs) {
	Py_ssize_t start;
	Py_ssize_t end;
	Py_ssize_t label = -1;

	const char *chrom;

	static char* keywords[] = {"chrom", "start", "end", "label", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "snn|n", keywords, &chrom, &start, &end, &label)) {
		return NULL;
	}

	cr_add(self->ranges, chrom, start, end, label);
	Py_RETURN_NONE;
}

static PyObject *pygros_ranges_index(pygros_Ranges *self) {
	cr_index(self->ranges);
	Py_RETURN_NONE;
}

static PyObject *pygros_ranges_overlap(pygros_Ranges *self, PyObject *args, PyObject *kwargs) {
	Py_ssize_t start, end;
	Py_ssize_t i, n, *b = 0, max_b = 0;
	Py_ssize_t crs, cre, crl;
	PyObject *op, *ops;
	
	const char *chrom;

	static char* keywords[] = {"chrom", "start", "end", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "snn", keywords, &chrom, &start, &end)) {
		return NULL;
	}

	n = cr_overlap(self->ranges, chrom, start, end, &b, &max_b);

	ops = PyList_New(0);

	for (i = 0; i < n; ++i) {
		crs = cr_start(self->ranges, b[i]);
		cre = cr_end(self->ranges, b[i]);
		crl = cr_label(self->ranges, b[i]);

		op = Py_BuildValue("nnn", crs, cre, crl);
		PyList_Append(ops, op);
		Py_DECREF(op);
	}

	free(b);
	return ops;
}

static PyObject *pygros_ranges_within(pygros_Ranges *self, PyObject *args, PyObject *kwargs) {
	Py_ssize_t start, end;
	Py_ssize_t i, n, *b = 0, max_b = 0;
	Py_ssize_t crs, cre, crl;

	const char *chrom;

	PyObject *op, *ops;

	static char* keywords[] = {"chrom", "start", "end", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "snn", keywords, &chrom, &start, &end)) {
		return NULL;
	}
	
	n = cr_contain(self->ranges, chrom, start, end, &b, &max_b);

	ops = PyList_New(0);

	for (i = 0; i < n; ++i) {
		crs = cr_start(self->ranges, b[i]);
		cre = cr_end(self->ranges, b[i]);
		crl = cr_label(self->ranges, b[i]);

		op = Py_BuildValue("nnn", crs, cre, crl);
		PyList_Append(ops, op);
		Py_DECREF(op);
	}

	free(b);
	return ops;
}

static PyObject *pygros_ranges_contain(pygros_Ranges *self, PyObject *args, PyObject *kwargs) {
	Py_ssize_t start, end;
	Py_ssize_t i, n, *b = 0, max_b = 0;
	Py_ssize_t crs, cre, crl;
	
	const char *chrom;

	PyObject *ops;
	PyObject *op;

	static char* keywords[] = {"chrom", "start", "end", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "snn", keywords, &chrom, &start, &end)) {
		return NULL;
	}
	
	n = cr_overlap(self->ranges, chrom, start, end, &b, &max_b);

	ops = PyList_New(0);

	for (i = 0; i < n; ++i) {
		crs = cr_start(self->ranges, b[i]);
		cre = cr_end(self->ranges, b[i]);

		if (crs <= start && cre >= end) {
			crl = cr_label(self->ranges, b[i]);
			op = Py_BuildValue("nnn", crs, cre, crl);
			PyList_Append(ops, op);
			Py_DECREF(op);
		}
	}

	free(b);
	return ops;
}

static PySequenceMethods pygros_ranges_as_sequence = {
	.sq_length = (lenfunc)pygros_ranges_length,
	.sq_contains = (objobjproc)pygros_ranges_contains,
};

static PyMethodDef pygros_ranges_methods[] = {
	{"add", (PyCFunction)pygros_ranges_add, METH_VARARGS|METH_KEYWORDS, NULL},
	{"index", (PyCFunction)pygros_ranges_index, METH_NOARGS, NULL},
	{"overlap", (PyCFunction)pygros_ranges_overlap, METH_VARARGS|METH_KEYWORDS, NULL},
	{"contain", (PyCFunction)pygros_ranges_contain, METH_VARARGS|METH_KEYWORDS, NULL},
	{"within", (PyCFunction)pygros_ranges_within, METH_VARARGS|METH_KEYWORDS, NULL},
	{NULL, NULL, 0, NULL}
};

PyTypeObject pygros_RangesType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "Ranges",
	.tp_basicsize = sizeof(pygros_Ranges),
	.tp_dealloc = (destructor)pygros_ranges_dealloc,
	.tp_repr = (reprfunc)pygros_ranges_repr,
	.tp_as_sequence = &pygros_ranges_as_sequence,
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_methods = pygros_ranges_methods,
	.tp_new = pygros_ranges_new,
};
