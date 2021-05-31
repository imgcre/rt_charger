from .anno import Target, Variant, anno, get_context, get_ctx_of_cursor
from clang.cindex import Cursor

class Config(object):
    def __init__(self, type) -> None:
        self.type = type

# generate configs for the field
@anno(target=Target.FieldDecl)
def config(cursor: Cursor, type: str):
    # print(f'config invoked at field {cursor.spelling}, type: {type}')
    li = get_ctx_of_cursor(cursor, 'preset')
    li.append(Config(type))
