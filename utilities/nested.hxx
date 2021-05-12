#pragma once

template <class Outer>
struct Nested {
    using nested_t = Nested<Outer>;
    using outer_t = Outer;
    Nested(outer_t* outer): outer(outer) { }
  protected:
    outer_t* outer;
};
