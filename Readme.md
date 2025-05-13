## `__cnew__` Hook
A new way of hacking in your own metaclass into Cython during the `__new__` phase of the code in order to provie a safe and effective way of using new 
And create new optimized structures for Cython to use.

## How Would I use `__cnew__`?
The way you would want to use this NewType feature is if you wanted to make something like a Dataclass Structure or another Low level type that 
needs access to multiple C Level Attributes that you can't get from a non C Extension Type.

My Current Agenda is making modern dataclasses run faster by making Cython Able to handle metaclasses because maintaining source code
in Cython is Better than maintaining it in C.

Currently this is how we plan to make the Cython API Work. This example is held subject to change.
```cython
from cpython.mem cimport PyMem_Malloc
from newtype cimport NewType, NewTypeProxy

# HeapStructMeta class Gains access to the NewType which carries it's own
# PyHeapTypeObject allowing for new types to be created on the fly in Cython
# with zero limitations as NewType's __new__ call essentially asks for whatever
# __cnew__ has.
cdef class HeapStructMeta(NewType):
    cdef:
        # In Msgspec offsets are carried in a LowLevel Array
        Py_ssize_t *offsets

    @staticmethod
    def __cnew__(proxy: NewTypeProxy, str name, tuple bases, dict namespace, /, **kwds):
        # you can pass off object creation like so in this example as a method is provided
        # in the C-API
        # We had to sacarfice some things because tp_new doesn't carry a full type know that
        # it's PyTypeObject was wrapped to the NewTypeProxy to save ourselves in order to allow
        # the Hook to successfully work.
        cdef HeapStructMeta hs = proxy.create(name, bases, namespace, **kwds)
        # use your imagination...
        ...     
        # from there later down the line when you've headcounted your class members you could gain access to each member's
        # offsets as long as you made a __slots__ variable...
        cdef Py_ssize_t* offsets  = <Py_ssize_t*>PyMem_Malloc(sizeof(Py_ssize_t) * num_members);
        if offsets == NULL:
           raise MemoryError()
        # Remeber to return whatever the Metaclass really is because __cinit__ comes after it
        # and there's bad things that can happen if this is not done.
        return hs 

    # Because our hook has zero effect on __new__ itself or the tp_new slot
    # we are free to continue using __cinit__ as we wish 
    def __cinit__(self, int order, int idk):
        ...
    
```




## Hacking in dunder `__new__`
in this generated code made by cython when making a cdef extension with the subclass of type you can see that we could theoretically hack in our own values
if we can somehow gain access to whatever is in the new slot just before `__cinit__` is being called.

```c
static PyObject *__pyx_tp_new_5slots_SlotMeta(PyTypeObject *t, PyObject *a, PyObject *k) {
  struct __pyx_obj_5slots_SlotMeta *p;
  // We would somehow want to gain access to this specific part of the code in order for this to be successful.
  PyObject *o = __Pyx_PyType_GetSlot((&PyType_Type), tp_new, newfunc)(t, a, k);
  if (unlikely(!o)) return 0;
  p = ((struct __pyx_obj_5slots_SlotMeta *)o);
  p->__pyx___slots__ = ((PyObject*)Py_None); Py_INCREF(Py_None);
  if (unlikely(__pyx_pw_5slots_8SlotMeta_1__cinit__(o, a, k) < 0)) goto bad;
  return o;
  bad:
  Py_DECREF(o); o = 0;
  return NULL;
}
```

Currently the only known way to do so is with our own C Extension and then somehow finding a way to wrap it in.
I have a fork of the Current Cython Codespace if this plan goes poorly and I should be able to find a way to add 
it to the compiler itself if all else fails.

## My Motives
- [This video](https://www.youtube.com/watch?v=QV4uHSpl-Do) where I present why I am doing all of this and what it's purpose will end up being for.

