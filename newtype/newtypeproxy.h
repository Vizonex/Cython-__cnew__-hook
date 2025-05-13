#ifndef __NEWTYPEPROXY_H__
#define __NEWTYPEPROXY_H__

#include "Python.h"


static PyTypeObject NewTypeProxy_Type;

typedef struct _newtypeproxy {
    PyObject_HEAD
    PyTypeObject* ptype;
} NewTypeProxyObject;

/// @brief Takes on the abundance of carrying a partial python object for handling different kinds of object creation. 
/// @param proxy the object were sending on through for later object creation
/// @return A Python Object if allocation was successful otherwise this returns NULL
static PyObject* NewTypeProxy_New(PyTypeObject* proxy){
    NewTypeProxyObject* newtypeproxy = (NewTypeProxyObject*)NewTypeProxy_Type.tp_alloc(&NewTypeProxy_Type, 0);
    if (newtypeproxy == NULL) return NULL;
    newtypeproxy->ptype = proxy;
    return (PyObject*)newtypeproxy;
}

static PyObject* _NewTypeProxy_new(PyTypeObject* type, PyObject *args, PyObject *kwargs){
    if ((kwargs != NULL) && (PyDict_GET_SIZE(kwargs) > 0)){
        PyErr_SetString(PyExc_TypeError, "NewTypeProxy Takes Zero Keyword Arguments");
    }
    else if (PyTuple_GET_SIZE(args) < 1) {
        PyErr_Format(
            PyExc_TypeError,
            "NewTypeProxy expected 1 argument, got %zd",
            PyTuple_GET_SIZE(args)
        );
        return NULL;
    }
    return NewTypeProxy_New(Py_TYPE(PyTuple_GET_ITEM(args, 0)));
}

/// @brief Creates a New Python Object with the proxied PyTypeObject sent with it
/// @param newtypeproxy the carrier class 
/// @return the new object created return NULL if it failed
static PyObject* NewTypeProxy_CreateObject(PyObject* newtypeproxy, PyObject* args, PyObject* kwargs){
    return PyType_Type.tp_new(((NewTypeProxyObject*)(newtypeproxy))->ptype, args, kwargs);
}

PyObject* NewTypeProxy_CAPI_CreateObject(NewTypeProxyObject* newtypeproxy, PyObject* args, PyObject* kwargs){
    return PyType_Type.tp_new(newtypeproxy->ptype, args, kwargs);
}



#define NEWTYPEPROXY_CREATE_OBJECT_METHODEF \
    {"create", (PyCFunction)(void(*)(void))(NewTypeProxy_CreateObject), METH_VARARGS | METH_KEYWORDS, "Creates a New Type from the type it's carrying."}



#endif // __NEWTYPEPROXY_H__