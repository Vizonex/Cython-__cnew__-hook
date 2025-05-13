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




/// @brief The __new__ method for cython to scretly hook onto.
/// @param type 
/// @param args 
/// @param kwargs 
/// @return A New Type Object if all was deemed as successful otherwise NULL 
/// is returned which then prompts python to raise an exception.
PyObject* NewTypeObject_New(PyTypeObject* type, PyObject* args, PyObject* kwargs);







#endif // __NEWTYPE_H__