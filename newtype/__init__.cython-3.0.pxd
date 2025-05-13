# A Bypass Mechanism for cython to utilize
from cpython.object cimport PyTypeObject
from cpython.datetime cimport PyDateTime_CAPI


cdef extern from *:
    """
/* Using Newtype API declarations from "newtype/__init__.pxd" */    
    """


cdef extern from "newtype/newtype.h":
    ctypedef struct NewTypeObject:
        pass

    # Ignore size so that different types can be theoretically accepted.
    ctypedef class newtype.NewType [object NewTypeObject, check_size ignore]:
        pass

    ctypedef struct NewTypeProxyObject:
        pass

    # object NewTypeProxy_CAPI_CreateObject(NewTypeProxyObject* ntp, tuple args, dict kwargs)

    ctypedef class newtype.NewTypeProxy [object NewTypeProxyObject, check_size ignore]:
        cdef:
            PyTypeObject* ptype

        # cdef inline PyTypeObject* underlying_type(self) noexcept:
        #     return self.ptype

        # cdef inline object create(self, tuple args, dict kwargs):
        #     return NewTypeProxy_CAPI_CreateObject(<NewTypeProxyObject*>self, args, kwargs)

    ctypedef struct PyNewType_CAPI:
        PyTypeObject* NewType
        PyTypeObject* NewTypeProxy
    
    PyNewType_CAPI* PyNewTypeAPI

    int PyNewType_IMPORT

    
    bint PyNewType_Check(object op) 
    bint PyNewType_CheckExact(object op)

    bint PyNewTypeProxy_Check(object op)
    bint PyNewTypeProxy_CheckExact(object op)




cdef inline int import_newtype() except -1:
    PyNewType_IMPORT
    if PyNewTypeAPI != NULL:
        return 0
    return -1


    


# header only support
cdef extern from "newtype/newtype.c":
    pass


