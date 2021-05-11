import os
import json

CONFIG_NAME = 'STM32'

with open('.vscode/c_cpp_properties.json') as f:
    data = json.load(f)
conf = next(filter(lambda x: x['name'] == CONFIG_NAME, data['configurations']))
defs = ' -D'.join(['', *conf['defines']])
args = ' '.join(['', *conf['compilerArgs']])

quote = r"'\''"
os.system(f"sed -i -E 's/(CFLAGS = DEVICE \\+).+/\\1 {quote}{defs}{quote}/' rtconfig.py")
os.system(f"sed -i -E 's/(DEVICE =).+/\\1 {quote}{args}{quote}/' rtconfig.py")