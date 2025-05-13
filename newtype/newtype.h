#ifndef __NEWTYPE_H__
#define __NEWTYPE_H__

#include "Python.h"
#include "newtypeproxy.h"
#include "structmember.h"

#define NEWTYPE_AUTHOR "Vizonex"
#define NEWTYPE_VERSION "0.0.1"

static PyTypeObject NewType_Type;

typedef struct _newtypeobject
{
    PyHeapTypeObject* base;
} NewTypeObject;



/// @brief The __new__ method for cython to secretly hook onto.
/// @param type 
/// @param args 
/// @param kwargs 
/// @return A New Type Object if all was deemed as successful otherwise NULL 
/// is returned which then prompts python to raise an exception.
PyObject* NewTypeObject_New(PyTypeObject* type, PyObject* args, PyObject* kwargs);



// C-API is utilized so Cython Can Grab it immediatley Otherwise 
// Our NewType We Made would have zero purpose as having no no __new__ 
// hook in cython was the motive

typedef struct  {
    PyTypeObject* NewType;
    PyTypeObject* NewTypeProxy;

} PyNewType_CAPI;

#define PyNewType_CAPSULE_NAME "newtype.newtype.newtype_CAPI"

static PyNewType_CAPI *PyNewTypeAPI = NULL;

#define PyNewType_IMPORT \
    PyNewTypeAPI = (PyNewType_CAPI*)PyCapsule_Import(PyNewType_CAPSULE_NAME, 0)

#define PyNewType_Check(op) PyObject_TypeCheck(op, PyNewTypeAPI->NewType)
#define PyNewType_CheckExact(op) Py_IS_TYPE(op, PyNewTypeAPI->NewType)

#define PyNewTypeProxy_Check(op) PyObject_TypeCheck(op, PyNewTypeAPI->NewTypeProxy)
#define PyNewTypeProxy_CheckExact(op) Py_IS_TYPE(op, PyNewTypeAPI->NewTypeProxy)



#endif // __NEWTYPE_H__