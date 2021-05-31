## 头文件
<%!
    def make_arg_decls(args):
        return f'<{", ".join(args)}>' if len(args) > 0 else ''

    def make_arg_impls(args):
        return f'<{", ".join([arg.split(" ")[-1] for arg in args])}>' if len(args) > 0 else ''
%>
%for include in includes:
#include ${include}
%endfor
namespace Preset {
class ${class_name}${make_arg_decls(args)}: public Singleton<${class_name}${make_arg_impls(args)}>, public ::${class_name} {
    friend singleton_t;
    ${class_name}(): ::${class_name}(${', '.join(params)}) {}
    %for config_type, config_items in configs.items():
    %for field_type, field_props in config_items.items():
    static const ${field_type} ${', '.join([prop.name for prop in field_props])};
    %endfor
    %endfor
};
}