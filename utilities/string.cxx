#include <string.hxx>
using namespace std;

vector<string> split(string_view str, char splitter, int cnt) {
    size_t pos = string::npos, nextPos = 0;
    auto result = vector<string>{};
    auto i = 0;
    while((nextPos = str.find(splitter, pos + 1)) != string::npos && !(cnt != -1 && i == cnt)) {
        result.push_back((string)str.substr(pos + 1, nextPos - pos - 1));
        pos = nextPos;
        ++i;
    }
    result.push_back((string)str.substr(pos + 1, str.size() - pos - 1));
    return result;
}
