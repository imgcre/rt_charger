from typing import Dict, List, Optional, Union

from clang.cindex import Cursor
import annotations
from enum import Enum
import inspect

class Target(Enum):
    Class = 0
    Constructor = 1
    FieldDecl = 2

class Variant(Enum):
    Entry = 0 # 入口，应该自动调用
    Metadata = 1 # 元数据
    ...

contexts: Dict[str, List[Dict[int, object]]] = {}
__cur_context_name: str = None

def get_context(ctx_name: str = None) -> Optional[Dict[Cursor, object]]:
    if ctx_name is None:
        ctx_name = __cur_context_name
    # print(f'get ctx {ctx_name}')
    if ctx_name not in contexts: return
    ctx_stack = contexts[ctx_name]
    if len(ctx_stack) == 0: return
    return ctx_stack[-1] # 返回栈顶

def get_ctx_of_cursor(cursor: Cursor, ctx_name: str = None, *, as_const: bool = False):
    ctx: Dict[int, List[object]] = get_context(ctx_name)
    cursor_id = cursor.get_usr()
    if cursor_id not in ctx:
        if as_const: return
        ctx[cursor_id] = []
    return ctx[cursor_id]

class Anno(object):

    def __init__(self, func, targets: List[Target], name: str, variant:Variant, with_context: bool) -> None:
        self.func = func
        self.targets = targets
        self.name = name
        self.variant = variant
        self.__post_action = None
        self.with_context = with_context

    def invoke(self, *args, **kwargs):
        if self.with_context:
            if self.name not in contexts:
                contexts[self.name] = []
            contexts[self.name].append({})
            # print(f'push ctx {self.name}')
            ...
        # print('try invoke func')
        return self.func(*args, **kwargs)

    def is_match(self, target: Target) -> bool:
        return target in self.targets

    def set_post_action(self, post_action):
        self.__post_action = post_action
        return post_action

    def post_action(self, *args, **kwargs):
        if self.__post_action is not None:
            self.__post_action(*args, **kwargs)
        if self.with_context:
            contexts[self.name].pop()
            # print(f'pop ctx {self.name}')
            ...


def anno(*, target: Union[Target, List[Target]], name: str=None, variant: Variant=Variant.Entry, with_context=False):
    def wrapper(func):
        anno_name = name if name is not None else func.__name__
        curr_anno = Anno(func, target if isinstance(target, list) else [target], anno_name, variant, with_context)
        annotations.annos[anno_name] = curr_anno
        def post_func(func):
            global __cur_context_name
            __cur_context_name = anno_name
            curr_anno.set_post_action(func)
            return func
        def deco(*args, **kwargs):
            global __cur_context_name
            __cur_context_name = anno_name
            func(*args, **kwargs)
        deco.post = post_func
        return deco
    return wrapper

