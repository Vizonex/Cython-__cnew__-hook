from typing_extensions import Self
from typing import overload

__author__ = "Vizonex"
__version__ : str

class NewType(type):
    "A Subclass of type of specifically CPython's PyHeapTypeObject"\
    "meant to create a hook onto __new__ to allow hacking in the creation of"\
    "different cython metaclass objects at compile-time to then be ran"\
    "off in python at runtime."\
    "\n"\
    "Until Cython desires to make support for a __cnew__ api this is"\
    "meant to serve as bypass apporch to one. This gains access to"\
    "diffrent attribute access can't be accessed normally inside __cinit__"\
    " or __prepare__\n"\
    "An Example would be Enabling HeapTypeObjects to access __slots__ "\
    "and then setting it inside the namespace dictionary so that __slots__ can be accessed during __cinit__\n\n"\
    "OTHER IMPORTANT THINGS TO REMEMBER\n"\
    "----------------------------------\n"\
    "1. Obey Cython's Rules by using `__cinit__` for setting"\
    "up the attributes that don't need setting up inside of `__cnew__`.\n"\
    "2. Use `__cnew__` for hacking in attributes before `__cinit__` is used\n"\
    "3. `__cnew__` helps you gain access to base attributes\n"\
    "that aren't normally accessable in the `__prepare__` class method\n"\
    "4. if you notice something acting slow remember that CPython has to"\
    " get the function and call it.\n"\
    "5. `__cnew__` is not a supported feature yet "\
    "by cython but hopes to spark the demand "\
    "for one. And hopes to invent it as standard partice."

    @overload
    def __cnew__(cls:type[Self], o: object, /) -> type[Self]: ...
    @overload
    def __cnew__(
        cls: type[Self], name: str, bases: tuple[type, ...], namespace: dict[str, Any], /, **kwds: Any
    ) -> type[Self]: ... 
    