import os

os.system(f"sed -i 's/\"cppStandard\": \"c++11\"/\"cppStandard\": \"c++20\"/' .vscode/c_cpp_properties.json")
os.system(f"sed -i 's#{os.getcwd()}#${{workspaceFolder}}#' .vscode/c_cpp_properties.json")
# os.system(f"sed -i -E 's/(CMAKE_CXX_STANDARD) 14/\\1 20/' CMakeLists.txt")
# os.system(f"sed -i 's#{os.getcwd()}#${{CMAKE_SOURCE_DIR}}#' CMakeLists.txt")
