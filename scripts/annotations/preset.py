from . import create_tu
from typing import Dict, List, Optional, Set, Tuple
from .anno import Target, anno, get_context, get_ctx_of_cursor
from clang.cindex import Index, Cursor, CursorKind, SourceLocation, SourceRange, TranslationUnit, Type
from .config import Config
from .use import Use
import re
from mako.template import Template
import os

def try_get_ptr_ee(field: Cursor) -> Optional[Cursor]:
    if field.kind != CursorKind.FIELD_DECL: return
    attrs: List[Cursor] = list(field.get_children())
    if len(attrs) != 3: return
    may_std, may_ptr, may_type = attrs
    if may_std.kind != CursorKind.NAMESPACE_REF or may_std.spelling != 'std': return
    if may_ptr.kind != CursorKind.TEMPLATE_REF or may_ptr.spelling != 'shared_ptr': return
    if may_type.kind != CursorKind.TYPE_REF: return
    return may_type

def get_include_param(path: str):
    # TODO: 判断是否为rtthread文件夹下的
    # ./os/task.hxx -> <os/task_preset.hxx>
    path = os.path.relpath(path)
    if path.startswith('rt-thread/'):
        print('start with rt')
        result = '<rtthread.h>'
    else:
        result = re.sub(r'^(?P<path>(?:\w+/)*)(?P<base>\w+)\.(?P<ext>\w+)', r'<\g<path>\g<base>.\g<ext>>', path)
    return result
    ...

def make_prefix(name: str, prefix='k'):
    return f'{prefix}{name[0].upper()}{name[1:]}'
    ...

@anno(target=Target.Class, with_context=True)
def preset(cursor: Cursor, *args):
    ...
    # print(f'preset invoked at class {cursor.spelling}')

    
@preset.post
def post(cursor: Cursor, *args):
    # print(f'preset post runned for {cursor.spelling}')

    class_name = cursor.spelling

    preset_nss: Cursor =  [c for c in cursor.lexical_parent.get_children() if 
        c.kind == CursorKind.NAMESPACE and 
        c.spelling == 'Preset' and 
        c.location.file.name == cursor.location.file.name
    ]
    if len(preset_nss) > 0:
        print('preset namespace already defined')
        return

    ctx = get_context()

    # TODO: 当存在多个构造函数的时候，选择首选构造函数
    ctor: Cursor = next((c for c in cursor.get_children() if c.kind == CursorKind.CONSTRUCTOR))
    # print(f'found ctor {ctor.spelling} {ctor.raw_comment}')
    includes: Set[str] = set()
    includes.add('<utilities/singleton.hxx>')
    # includes.add(f'"{ctor.location.file.name.split("/")[-1]}"')
    params: List[str] = []
    #[file_name, field_tpye, field_name]
    configs: Dict[str, Dict[str, List[str]]] = {}
    arg: Cursor
    fields = dict([(c.spelling, c) for c in cursor.get_children() if c.kind == CursorKind.FIELD_DECL])
    for arg in ctor.get_arguments():
        bound_field: Cursor = fields[arg.spelling]
        type: Type = bound_field.type
        # print(f'{arg.spelling} {bound_field.raw_comment} {type.spelling}')
        ctx = get_ctx_of_cursor(bound_field, as_const=True)
        if ctx is not None:
            # print(f'associated {bound_field.spelling}: {ctx}')
            ...
        ptr_ee = try_get_ptr_ee(bound_field)
        type_str_rm_cv = type.spelling.split(" ")[-1]
        
        if ptr_ee is not None: # may preset
            # print(f'ptr ee type: {ptr_ee.spelling}@{ptr_ee.get_definition().location.file.name}')
            # includes.add(get_include_param(ptr_ee.get_definition().location.file.name))
            use = list(filter(lambda x: isinstance(x, Use), ctx or []))
            use = use[-1] if len(use) > 0 else None
            tep_str = f'<{", ".join(use.params)}>' if use is not None else ''
            params.append(f'{ptr_ee.spelling.split(" ")[-1]}{tep_str}::get()')
        else: #may config
            conf = list(filter(lambda x: isinstance(x, Config), ctx or []))
            conf = conf[-1] if len(conf) > 0 else None
            field_name = make_prefix(bound_field.spelling)
            params.append(field_name)
            if conf is not None:
                # includes.add(get_include_param(type.get_declaration().location.file.name))
                if conf.type not in configs:
                    configs[conf.type] = {}
                config_types = configs[conf.type]
                if type_str_rm_cv not in config_types:
                    config_types[type_str_rm_cv] = []
                prop = lambda: ...
                prop.name = field_name
                prop.cursor = bound_field
                config_types[type_str_rm_cv].append(prop)
                ...
            else:
                print('W: conf is none, currently not supported')
            ...
        ...
    
    curr_dir = os.path.dirname(__file__)
    t = Template(filename=os.path.join(curr_dir, 'preset.mako'))
    rendered = t.render(
        includes=includes, 
        class_name=class_name,
        params=params,
        configs=configs,
        args=args,
    )

    with open(cursor.location.file.name, 'a') as f:
        f.write(rendered)
        ...
    print('preset generated!')

    for config_type, config_items in configs.items():
        config_file = f'./configs/{config_type}.cxx'
        includes: Set[str] = set()
        includes.add(get_include_param(cursor.location.file.name))
        for field_type_str, fields in config_items.items():
            field_type: Type = fields[0].cursor.type
            includes.add(get_include_param(field_type.get_declaration().location.file.name))
            ...

        config_t = Template(filename=os.path.join(curr_dir, 'preset.config.mako'))
        rendered = config_t.render(
            includes=includes,
            config_items=config_items,
            class_name=class_name
        )
        
        print(rendered)

        if os.path.exists(config_file):
            # 直接从模板生成文件
            print(f'config file {config_type} exists!')
            
            tu = create_tu(config_file)
            ns: Cursor
            for ns in [child for child in tu.cursor.get_children() 
                if child.kind == CursorKind.NAMESPACE and
                child.location.file.name == config_file and
                child.spelling == 'Preset'
            ]:
                print(ns.spelling)
                var: Cursor
                for var in [child for child in ns.get_children()
                    if child.kind == CursorKind.VAR_DECL
                ]:
                    var_type, var_from = var.get_children()
                    var_from = var_from.spelling.split(' ')[-1]
                    if var_from != f'Preset::{class_name}': continue
                    print(var.spelling)
                    rng: SourceRange = ns.extent
                    

                    ...
                ...
                break

            ...
        else:
            ...
        print('curr_config_file', config_file)
        ...

    print('config generated!')
