from typing import Any, overload, TypeVar

T = TypeVar("T")
__version__: str

class NewType(type):
    @overload
    def __new__(cls, object:object) -> NewType | type: ...
    @overload
    def __new__(cls, name:str, bases:tuple[type, ...], namespace:dict[str, Any], **kwds) -> NewType | type: ...

class NewTypeProxy:
    """Carries Type Objects call upon, it is the first argument used with __cnew__"""
    
    def __init__(proxy:type[T]) -> NewTypeProxy: ...
    def create(self, *args, **kwargs) -> type[T]: ...
