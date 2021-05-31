from enum import Enum
from clang.cindex import Config, TranslationUnit
from clang.cindex import Index, Cursor, CursorKind

import os
import re
import annotations
from annotations.anno import Target, Variant
import sys

index = Index.create()
  
def main():
    run_annotations(sys.argv[1])

# if name is none, then exec all cmd
def run_commands(comment: str, g, name: str=None):
    comment_r = re.compile(r'^//!\s*((\w+)\s*(?:\(.*?\))?)\s*$', re.MULTILINE)
    matches = comment_r.findall(comment)
    r = {}
    # print(matches)
    # config的定义少了一个参数
    for expr, cmd_name in matches:
        b_run_cmd = False
        if name is None:
            b_run_cmd = True
        elif name == cmd_name:
            b_run_cmd = True
        if b_run_cmd and cmd_name in g and g[cmd_name] is not None:
            # print(expr, g)
            res = eval(expr, g)
            while callable(res):
                res = res()
            r[cmd_name] = res
        ...
    return r

class Action(Enum):
    Pre = 0
    Post = 1

def get_commands(cursor: Cursor, action: Action, variant: Variant=None):
    annos = annotations.get()
    g = {}
    pass_invoke = None
    if action == Action.Pre:
        def pass_invoke(anno):
            return lambda *args, **kwargs: anno.invoke(cursor, *args, **kwargs)
    elif action == Action.Post:
        def pass_invoke(anno): #NOTE: 这里忽略了参数
            return lambda *args, **kwargs: anno.post_action(cursor, *args, **kwargs)
    for name, anno in annos.items():
        if not anno.is_match({
            CursorKind.CLASS_DECL: Target.Class,
            CursorKind.CONSTRUCTOR: Target.Constructor,
            CursorKind.FIELD_DECL: Target.FieldDecl
        }[cursor.kind]): continue
        if variant is not None and variant != anno.variant: continue
        if pass_invoke is None: continue
        g[name] = pass_invoke(anno)
    return g

def run_annotations(file_name: str):
    tu = annotations.create_tu(file_name)
    cursor: Cursor
    for cursor, action in dfs(tu.cursor, file_name):
        run_commands(cursor.raw_comment, get_commands(cursor, action, Variant.Entry))

def dfs(cursor: Cursor, file_name: str):
    if cursor.brief_comment is not None:
        yield cursor, Action.Pre
    for child in cursor.get_children():
        if child is None: continue
        if child.location.file.name != file_name: continue 
        yield from dfs(child, file_name)
    if cursor.brief_comment is not None:
        yield cursor, Action.Post

if __name__ == '__main__':
    main()
