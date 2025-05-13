#include "Python.h"
#include "newtypeproxy.h"
#include "newtype.h"


typedef struct {
    PyObject* author;
    PyObject* version;
    PyObject* NewType;
    PyObject* NewTypeProxy;
} NewTypeModuleState;

static struct PyModuleDef newtype_module;

static NewTypeModuleState* NewTypeModule_GetState(PyObject* module){
    return (NewTypeModuleState*)PyModule_GetState(module);
};


static NewTypeModuleState* NewTypeModule_GetGlobalState(){
    PyObject* module = PyState_FindModule(&newtype_module);
    return (module == NULL) ? NULL : NewTypeModule_GetState(module);
}


// *********************************************************************
// * NewType Methods                                                   *
// *********************************************************************

PyDoc_STRVAR(NewTypeObject_Doc, 
    "NewType(o:object) -> NewType;\n"\
    "NewType(name: str, bases: tuple[type, ...], dict: dict[str, Any], /, **kwds: Any) -> NewType\n"
    "--\n"\
    "A Subclass of Python's type object, Creates a New Object or Metaclass Object by creating Cython Compatable Hook for __new__ called __cnew__"
);

PyObject* NewTypeObject_New(PyTypeObject* type, PyObject* args, PyObject* kwargs){
    
    // if were just NewTypeObject alone we need to bail...
    if (type->tp_base == &PyType_Type){
        // Were not a subclass no need to call for a hook.
        return PyType_Type.tp_new(type, args, kwargs);
    }

    // Assume at this point that __slots__ is not officially here yet...
    // So were free to use a safer method to get what we wish...
    // GetAttrString can be unpredictable sometimes...
    PyObject* __cnew__ = PyDict_GetItemString(type->tp_dict, "__cnew__");
    if (__cnew__ == NULL) return NULL;

    // Create our Type Proxy So that Python doesn't assume we screwed something up.
    PyObject* proxy = NewTypeProxy_New(type);
    if (proxy == NULL) return NULL;
    // Make a new ref for proxy before adding it to our new argument list
    Py_INCREF(proxy);

    // Reorganize Positional Arguments so that type gets installed properly.
    Py_ssize_t nargs = PyTuple_GET_SIZE(args);
    PyObject* arg_list = PyList_New(nargs + 1);
    if (arg_list == NULL){
        Py_CLEAR(proxy);
        return NULL;
    };
    
    PyList_SET_ITEM(arg_list, 0, proxy);

    for (Py_ssize_t i = 0; i < nargs; i++ ){
        PyObject* arg = PyTuple_GET_ITEM(args, i);
        if (arg == NULL) return NULL;
        PyList_SET_ITEM(arg_list, i + 1, arg);
    }
    PyObject* new_args = PyList_AsTuple(arg_list);

    if (new_args == NULL) {
        Py_CLEAR(arg_list);
        Py_CLEAR(proxy);
        return NULL;
    }
    // Up Refcount by 1 incase were doing anything 
    // crazy with our new positional arguments
    Py_INCREF(new_args);
    
    // Delete Arglist we're done moving the NewType Proxy where we need it to be stored.
    Py_CLEAR(arg_list);

    // We're ready to Call __cnew__
    PyObject* new_type = PyObject_Call(__cnew__, new_args, kwargs);
    
    // Were done with Our Proxy and new_args now...
    Py_CLEAR(new_args);
    Py_CLEAR(proxy);
    // Cleanup is over so check if were failed.
    if (new_type == NULL) return NULL;

    return new_type;
} 



/*******************
 * NewProxy Type   *
 *******************/

static struct PyMethodDef newtypeproxy_methods[] = {
    NEWTYPEPROXY_CREATE_OBJECT_METHODEF,
    {NULL}
};

static PyTypeObject NewTypeProxy_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "newtype.newtype.NewTypeProxy",
    .tp_doc =  PyDoc_STR("NewTypeProxy()\n--\nA Capsule Type to store PyTypeObjects for finalization."),
    .tp_new = _NewTypeProxy_new,
    .tp_basicsize = sizeof(NewTypeProxyObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = newtypeproxy_methods
};


PyDoc_STRVAR(NewType__doc__, 
"NewType(object) -> NewType\n"
"NewType(name, bases, namespace, **kwds) -> NewType\n"
"--\n"
"A Subclass of type of specifically CPython's PyHeapTypeObject"
"meant to create a hook onto __new__ to allow hacking in the creation of"
"different cython metaclass objects at compile-time to then be ran"
"off in python at runtime."
"\n"
"Until Cython desires to make support for a __cnew__ api this is"
"meant to serve as bypass apporch to one. This gains access to"
"diffrent attribute access can't be accessed normally inside __cinit__"
" or __prepare__\n"
"An Example would be Enabling HeapTypeObjects to access __slots__ "
"and then setting it inside the namespace dictionary so that __slots__ can be accessed during __cinit__\n\n"
"OTHER IMPORTANT THINGS TO REMEMBER\n"
"----------------------------------\n"
"1. Obey Cython's Rules by using `__cinit__` for setting "
"up the attributes that don't need setting up inside of `__cnew__`.\n"
"2. Use `__cnew__` for hacking in attributes before `__cinit__` is used\n"
"3. `__cnew__` helps you gain access to base attributes\n"
"that aren't normally accessable in the `__prepare__` class method\n"
"4. if you notice something acting slow remember that CPython has to "
"get the function and call it.\n"
"5. `__cnew__` is not a supported feature yet "
"by cython but hopes to spark the demand "
"for one. And hopes to invent "\
" it as standard partice."
);


static PyTypeObject NewType_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "newtype.newtype.NewType",
    .tp_doc = NewType__doc__,
    .tp_new = NewTypeObject_New,
    .tp_basicsize = sizeof(NewTypeObject),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS | Py_TPFLAGS_BASETYPE,
};



static int newtype_traverse(PyObject* m, visitproc visit, void* arg){
    NewTypeModuleState* st = NewTypeModule_GetState(m);
    Py_VISIT(st->author);
    Py_VISIT(st->NewType);
    Py_VISIT(st->NewTypeProxy);
    Py_VISIT(st->version);
    return 0;
}

static int newtype_clear(PyObject* m){
    NewTypeModuleState* st = NewTypeModule_GetState(m);
    Py_CLEAR(st->author);
    Py_CLEAR(st->NewType);
    Py_CLEAR(st->NewTypeProxy);
    Py_CLEAR(st->version);
    return 0;
}

static void newtype_free(PyObject*m){
    newtype_clear(m);
}

static struct PyModuleDef newtype_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "newtype.newtype",
    .m_size = sizeof(NewTypeModuleState),
    .m_traverse = newtype_traverse,
    .m_clear = newtype_clear,
    .m_free = (freefunc)newtype_free
};




PyMODINIT_FUNC 
PyInit_newtype(void) 
{
    PyObject* module;
    module = PyState_FindModule(&newtype_module);
    if (module){
        Py_INCREF(module);
        return module;
    }

    NewType_Type.tp_base = &PyType_Type;
    if (PyType_Ready(&NewType_Type) < 0){
        return NULL;
    }
    
    if (PyType_Ready(&NewTypeProxy_Type) < 0){
        return NULL;
    }

    module = PyModule_Create(&newtype_module);
    Py_INCREF(&NewType_Type);
    Py_INCREF(&NewTypeProxy_Type);

    // Setup our new hackable Cython Type
    if (PyModule_AddObject(module, "NewType", (PyObject*)(&NewType_Type)) < 0){
        Py_DECREF(module);
        return NULL;
    }

    if (PyModule_AddObject(module, "NewTypeProxy", (PyObject*)(&NewTypeProxy_Type)) < 0){
        Py_DECREF(module);
        return NULL;
    }

    // Setup author of the project and version information
    if (PyModule_AddObject(module, "__author__", PyUnicode_FromString(NEWTYPE_AUTHOR)) < 0){
        Py_DECREF(module);
        return NULL;
    }

    if (PyModule_AddObject(module, "__version__", PyUnicode_FromString(NEWTYPE_VERSION)) < 0){
        Py_DECREF(module);
        return NULL;
    }

    // Now for the real treat
    PyNewType_CAPI* NewTypeCAPI;
    NewTypeCAPI->NewType = &NewType_Type;
    NewTypeCAPI->NewTypeProxy = &NewTypeProxy_Type;

    PyObject* c_api = PyCapsule_New(NewTypeCAPI, PyNewType_CAPSULE_NAME, NULL);
    
    if (PyModule_AddObject(module, "newtype_CAPI", c_api) < 0){
        Py_DECREF(module);
        return NULL;
    }

    return module;
}



