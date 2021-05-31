%for include in includes:
#include ${include}
%endfor

namespace Preset {
%for config_type, config_fields in config_items.items():
const ${config_type}
    %for idx, config_field in enumerate(config_fields):
    ${class_name}::${config_field.name} = 0${',' if idx < len(config_fields) - 1 else ';'}
    %endfor
%endfor
}