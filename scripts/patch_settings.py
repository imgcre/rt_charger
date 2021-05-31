import os

os.system(f"sed -i 's/\"cppStandard\": \"c++11\"/\"cppStandard\": \"c++20\"/' .vscode/c_cpp_properties.json")
os.system(f"sed -i 's/CMAKE_CXX_STANDARD 14/CMAKE_CXX_STANDARD 20/' CMakeLists.txt")
