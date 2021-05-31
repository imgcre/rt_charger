from clang.cindex import Config, TranslationUnit
from clang.cindex import Index, Cursor, CursorKind
from asciitree import LeftAligned
from asciitree.traversal import Traversal
 
FILE_PATH = './configs/bsp.cxx'

def main():
    if Config.loaded:
        ...
    else:
        print('load')
        Config.set_library_path('/usr/lib/llvm-10/lib')

    index = Index.create()
    tu = index.parse(FILE_PATH, ['-x', 'c++', '-std=c++20', '-I.', '-I./rt-thread/include'])
    assert(isinstance(tu, TranslationUnit))
    #print(asciitree.draw_tree(tu.cursor, node_children, print_node))
    tr = LeftAligned(
        traverse=Traversal(
            get_children=node_children,
            get_text=print_node
        )
    )
    print(tr(tu.cursor))

    process_tu(tu.cursor)

def process_tu(cursor: Cursor):
    for child in cursor.get_children():
        if child.kind == CursorKind.CLASS_DECL:
            print(f'found class {child.spelling} with comment "{child.brief_comment}"')
            # TODO pass args to comment manager
            ...
        ...


def node_children(node: Cursor):
    return [c for c in node.get_children() if c.location.file.name == FILE_PATH]
    # return [c for c in node.get_children()]

def print_node(node: Cursor):
    text = node.spelling or node.displayname
    kind: CursorKind = node.kind
    
    decl_spec = ''
    if node.kind.name == 'CLASS_DECL':
        decl_spec = 'class'
        #node.get_

    return f'{kind.name} {text} [{node.brief_comment}]'

if __name__ == '__main__':
    main()
