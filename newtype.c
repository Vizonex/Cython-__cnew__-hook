#define PY_SSIZE_T_CLEAN
#include "newtype.h"
#include "Python.h"
#include "structmember.h"



// TODO: Move CAPIs to a header file for further use with cython because that 
// what we wrote this module for in the first place.

typedef struct {
    PyObject* author;
    PyObject* version;
    PyObject* NewType;
} NewTypeModuleState;

static PyTypeObject NewType_Type;
static struct PyModuleDef newtype_module;


static NewTypeModuleState* NewTypeModule_GetState(PyObject* module){
    return (NewTypeModuleState*)PyModule_GetState(module);
};


static NewTypeModuleState* NewTypeModule_GetGlobalState(){
    PyObject* module = PyState_FindModule(&newtype_module);
    return (module == NULL) ? NULL : NewTypeModule_GetState(module);
}


// ********************************************************
// * NewType Methods
// ********************************************************


// vectorcall Type Meant to Provide a __cnew__ hook if none are defined.

PyObject *NewTypeObject_CNew_impl(PyObject *self, PyObject* args, PyObject* kwargs)
{       
    
    return NewTypeObject_CNew(Py_TYPE(self), args, kwargs);
}



/// @brief The Hook to tp_new to hook to cython so that our own methods can 
/// be hacked in as a result As a warning please remember that your __cnew__ 
/// function needs to return the correct object otherwise you could face 
/// deadly errors or other consequences down the line.
/// @param t 
/// @param args 
/// @param kw 
/// @return Our newly created type of object for Cython to manipulate later inside __cinit__

static PyObject* NewTypeObject_New(PyTypeObject *type, PyObject *args, PyObject *kwargs){
    if (type->tp_base == &PyType_Type){
        return NewTypeObject_CNew(type, args, kwargs);
    }
    // Proxy off PyTypeObject so that it has something...
    printf("_PyObject_New");
    PyObject* cls = _PyObject_New(type);
    cls->ob_type = type;
    printf("PyObject_GetAttrString");
    PyObject* cnew = PyObject_GetAttrString(type, "__cnew__");
    PyObject* new_args = PyTuple_New(PyTuple_GET_SIZE(args));

    for (Py_ssize_t i = 0; i < PyTuple_GET_SIZE(args); i++){
        PyTuple_SET_ITEM(new_args, i + 1, PyTuple_GET_ITEM(args, i));
    }
    puts("RUNNING!\n");
    PyObject* n = PyObject_Call(cnew, new_args, kwargs);
    Py_CLEAR(new_args);
    if (n == NULL){
        puts("DID NOT SUCCEED! :(");
        return NULL;
    }
    puts("SUCCESS!");
    return n;
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

static struct PyMethodDef newtype_methods[] = {
    {"__cnew__", (PyCFunction)NewTypeObject_CNew_impl, METH_KEYWORDS | METH_VARARGS,
        PyDoc_STR("NewType.__cnew__(cls, object) -> NewType\n"
            "NewType.__cnew__(cls, name:str, bases:tuple[type, ...], class_dict:dict, **kwds) -> NewType\n"
            "--\n"
            "A Hook to For Cython __new__ calls to be performed from. Secifically Compile-time hooks"
        )
    },
    {0}
};


static PyTypeObject NewType_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "newtype.newtype.NewType",
    .tp_doc = NewType__doc__,
    .tp_basicsize = sizeof(NewTypeObject),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_TYPE_SUBCLASS | Py_TPFLAGS_BASETYPE,
    .tp_new = NewTypeObject_New,
    .tp_methods = newtype_methods,
};

static int newtype_traverse(PyObject* m, visitproc visit, void* arg){
    NewTypeModuleState* st = NewTypeModule_GetState(m);
    Py_VISIT(st->author);
    Py_VISIT(st->NewType);
    Py_VISIT(st->version);
    return 0;
}

static int newtype_clear(PyObject* m){
    NewTypeModuleState* st = NewTypeModule_GetState(m);
    Py_CLEAR(st->author);
    Py_CLEAR(st->NewType);
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
    module = PyModule_Create(&newtype_module);
    Py_INCREF(&NewType_Type);
    // Setup our new hackable Cython Type
    if (PyModule_AddObject(module, "NewType", (PyObject*)(&NewType_Type)) < 0){
        return NULL;
    }
    // Setup author of the project and version information
    if (PyModule_AddObject(module, "__author__", PyUnicode_FromString(NEWTYPE_AUTHOR)) < 0){
        return NULL;
    }
    if (PyModule_AddObject(module, "__version__", PyUnicode_FromString(NEWTYPE_VERSION)) < 0){
        return NULL;
    }
    return module;
}


