/* toolchain */
#include <cassert>
#include <iostream>

struct TestFields
{
    bool test0 : 1;
    bool test1 : 1;
    bool test2 : 1;
    bool test3 : 1;
    bool test4 : 1;
    bool test5 : 1;
    bool test6 : 1;
    bool test7 : 1;
};

union TestFieldsUnion {
    TestFields fields;
    uint8_t value = 0;
};

static_assert(sizeof(TestFields) == 1);
static_assert(sizeof(TestFieldsUnion) == 1);

int main(void)
{
    TestFieldsUnion data;

    data.fields.test0 = true;
    data.fields.test2 = true;
    data.fields.test4 = true;
    data.fields.test6 = true;

    std::cout << "val: " << (int)data.value << std::endl;
    assert(data.value == 0b01010101);

    return 0;
}
