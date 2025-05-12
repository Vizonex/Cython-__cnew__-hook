#ifndef __NEWTYPE_H__
#define __NEWTYPE_H__

#include "Python.h"
#include "structmember.h"

#define NEWTYPE_AUTHOR "Vizonex"
#define NEWTYPE_VERSION "0.0.1"

static PyTypeObject NewType_Type;


typedef struct _newtypeobject
{
    PyHeapTypeObject* nt_base;
    
} NewTypeObject;


/// @brief This is the method that we wish to Hook, 
/// This is only here to prevent crashing, however you could simply call 
/// this function when you're finished modifying the data before type-creation begins
/// @param args arguments for that type
/// @param kw keywords
/// @return The New Class Made
PyObject *NewTypeObject_CNew(PyTypeObject* type, PyObject *args, PyObject *kw)
{
    return PyType_Type.tp_new(type, args, kw);
}






#endif // __NEWTYPE_H__
