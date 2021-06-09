#pragma once

template<class U, class Field>
struct Offset {
    Offset(U& u, Field& field) {
        auto cpU = (char*)&u;
        auto cpField = (char*)&field;
        if(cpField < cpU || cpField + sizeof(field) > cpU + sizeof(u))
            throw std::runtime_error{"not the field of current inst"};
        offset = size_t(cpField - cpU);
    }

    Field& recover(U& u) {
        return *((Field*)((char*)&u + offset));
    }

    private:
    size_t offset;
};