#include "test.h"
#include "../includeall.h"
namespace test
{
    struct TestStructC14
    {
        int a;
        std::string b;
        double c;
        int d;
    };

    CTest::CTest()
    {
        test_int_ = 0;
    }

    CTest::~CTest()
    {
    }
    void CTest::test()
    {
        auto [x1, x2, x3, x4] = TestStructC14{1, "2x", 3.0, 0b1111};

        std::cout << "x1: " << x1 << std::endl;
        std::cout << "x2: " << x2 << std::endl;
        std::cout << "x3: " << x3 << std::endl;
        std::cout << "x4: " << x4 << std::endl;
    }
}   