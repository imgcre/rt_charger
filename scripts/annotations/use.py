from .anno import Target, Variant, anno, get_ctx_of_cursor
from clang.cindex import Cursor

class Use(object):
    def __init__(self, params) -> None:
        self.params = params

@anno(target=Target.FieldDecl)
def use(cursor: Cursor, *params):
    # print(f'use invoked at field {cursor.spelling}, params: {params}')
    ctx = get_ctx_of_cursor(cursor, 'preset')
    ctx.append(Use(params))