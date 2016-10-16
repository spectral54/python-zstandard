/**
* Copyright (c) 2016-present, Gregory Szorc
* All rights reserved.
*
* This software may be modified and distributed under the terms
* of the BSD license. See the LICENSE file for details.
*/

#include "python-zstandard.h"

PyDoc_STRVAR(ZstdCompressionDict__doc__,
"ZstdCompressionDict(data) - Represents a computed compression dictionary\n"
"\n"
"This type holds the results of a computed Zstandard compression dictionary.\n"
"Instances are obtained by calling ``train_dictionary()`` or by passing bytes\n"
"obtained from another source into the constructor.\n"
);

static int ZstdCompressionDict_init(ZstdCompressionDict* self, PyObject* args) {
	const char* source;
	Py_ssize_t sourceSize;

	self->dictData = NULL;
	self->dictSize = 0;

#if PY_MAJOR_VERSION >= 3
	if (!PyArg_ParseTuple(args, "y#", &source, &sourceSize)) {
#else
	if (!PyArg_ParseTuple(args, "s#", &source, &sourceSize)) {
#endif
		return -1;
	}

	self->dictData = malloc(sourceSize);
	if (!self->dictData) {
		PyErr_NoMemory();
		return -1;
	}

	memcpy(self->dictData, source, sourceSize);
	self->dictSize = sourceSize;

	return 0;
	}

static void ZstdCompressionDict_dealloc(ZstdCompressionDict* self) {
	if (self->dictData) {
		free(self->dictData);
		self->dictData = NULL;
	}

	PyObject_Del(self);
}

static PyObject* ZstdCompressionDict_dict_id(ZstdCompressionDict* self) {
	unsigned dictID = ZDICT_getDictID(self->dictData, self->dictSize);

	return PyLong_FromLong(dictID);
}

static PyMethodDef ZstdCompressionDict_methods[] = {
	{ "dict_id", (PyCFunction)ZstdCompressionDict_dict_id, METH_NOARGS,
	PyDoc_STR("dict_id() -- obtain the numeric dictionary ID") },
	{ NULL, NULL }
};

static Py_ssize_t ZstdCompressionDict_length(ZstdCompressionDict* self) {
	return self->dictSize;
}

static PySequenceMethods ZstdCompressionDict_sq = {
	(lenfunc)ZstdCompressionDict_length, /* sq_length */
	0,                                   /* sq_concat */
	0,                                   /* sq_repeat */
	0,                                   /* sq_item */
	0,                                   /* sq_ass_item */
	0,                                   /* sq_contains */
	0,                                   /* sq_inplace_concat */
	0                                    /* sq_inplace_repeat */
};

PyTypeObject ZstdCompressionDictType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"zstd.ZstdCompressionDict",     /* tp_name */
	sizeof(ZstdCompressionDict),    /* tp_basicsize */
	0,                              /* tp_itemsize */
	(destructor)ZstdCompressionDict_dealloc, /* tp_dealloc */
	0,                              /* tp_print */
	0,                              /* tp_getattr */
	0,                              /* tp_setattr */
	0,                              /* tp_compare */
	0,                              /* tp_repr */
	0,                              /* tp_as_number */
	&ZstdCompressionDict_sq,        /* tp_as_sequence */
	0,                              /* tp_as_mapping */
	0,                              /* tp_hash */
	0,                              /* tp_call */
	0,                              /* tp_str */
	0,                              /* tp_getattro */
	0,                              /* tp_setattro */
	0,                              /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	ZstdCompressionDict__doc__,     /* tp_doc */
	0,                              /* tp_traverse */
	0,                              /* tp_clear */
	0,                              /* tp_richcompare */
	0,                              /* tp_weaklistoffset */
	0,                              /* tp_iter */
	0,                              /* tp_iternext */
	ZstdCompressionDict_methods,    /* tp_methods */
	0,                              /* tp_members */
	0,                              /* tp_getset */
	0,                              /* tp_base */
	0,                              /* tp_dict */
	0,                              /* tp_descr_get */
	0,                              /* tp_descr_set */
	0,                              /* tp_dictoffset */
	(initproc)ZstdCompressionDict_init, /* tp_init */
	0,                              /* tp_alloc */
	PyType_GenericNew,              /* tp_new */
};

void compressiondict_module_init(PyObject* mod) {
	Py_TYPE(&ZstdCompressionDictType) = &PyType_Type;
	if (PyType_Ready(&ZstdCompressionDictType) < 0) {
		return;
	}

	Py_INCREF((PyObject*)&ZstdCompressionDictType);
	PyModule_AddObject(mod, "ZstdCompressionDict",
		(PyObject*)&ZstdCompressionDictType);
}