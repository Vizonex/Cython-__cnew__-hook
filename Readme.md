## `__cnew__` Hook
A new way of hacking in your own metaclass during the `__new__` phase of the code in order to provie a safe and effective way of using new 

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

