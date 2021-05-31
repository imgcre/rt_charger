from importlib import import_module
import os
from typing import Dict

from clang.cindex import Index

annos= {}

index = Index.create()
def create_tu(file_name):
    return index.parse(file_name, ['-x', 'c++', '-std=c++20', '-I.', '-I./rt-thread/include'])

for module in [f".{''.join(x.split('.')[:-1])}" for x in os.listdir(os.path.dirname(__file__)) if x.endswith('.py') and x != '__init__.py']:
    import_module(module, __name__)

def test():
    print('test called')
    ...

def get():
    return annos
