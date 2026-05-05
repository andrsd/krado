#include "gmock/gmock.h"
#include "krado/qtr.h"
#include <chrono>

using namespace krado;

struct ABC;

struct IntPointLessThan {
    bool
    operator()(const Qtr<int> & ent1, const Qtr<int> & ent2) const
    {
        // std::cerr << "<" << std::endl;
        return *ent1 < *ent2;
        // return ent1.get() < ent2.get();
    }
};

struct IntCmp {
    bool
    operator()(const int * ent1, const int * ent2) const
    {
        return *ent1 < *ent2;
    }
};

struct IntCmpUnique {
    bool
    operator()(const std::unique_ptr<int> & ent1, const std::unique_ptr<int> & ent2) const
    {
        return *ent1 < *ent2;
    }
};

TEST(BDSMeshTest, test)
{
    std::set<Qtr<int>, IntPointLessThan> s;
    // s.reserve();
    for (int i = 1; i <= 1000000; i++) {
        // auto q = ;
        s.insert(Qtr<int>::alloc(i));
    }
    std::cerr << "sz = " << sizeof(Qtr<int>) << std::endl;

    // auto it = s.find()

    std::cerr << "find" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    // auto it = s.find(777);
    auto it = std::find_if(s.begin(), s.end(), [](const auto & ptr) {
        //
        return *ptr == 777;
    });
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() * 1e6 << "us\n";

    std::cerr << "it = " << **it << std::endl;

    std::cerr << "quit" << std::endl;
}

TEST(BDSMeshTest, test2)
{
    std::set<int *, IntCmp> s;
    for (int i = 1; i <= 1000000; i++) {
        auto obj = new int;
        *obj = i;
        s.insert(obj);
    }
    std::cerr << "sz = " << sizeof(int *) << std::endl;

    std::cerr << "find" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    int elem = 777;
    auto it = s.find(&elem);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() * 1e6 << "us\n";

    // auto it = std::find_if(s.begin(), s.end(), [](const auto & ptr) {
    //     //
    //     return *ptr == 5;
    // });
    std::cerr << "it = " << **it << std::endl;

    std::cerr << "quit" << std::endl;
}

TEST(BDSMeshTest, test3)
{
    std::set<std::unique_ptr<int>, IntCmpUnique> s;
    for (int i = 1; i <= 1000000; i++) {
        auto obj = std::make_unique<int>(i);
        s.emplace(std::move(obj));
    }
    std::cerr << "sz = " << sizeof(std::unique_ptr<int>) << std::endl;

    std::cerr << "find" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto it = std::find_if(s.begin(), s.end(), [](const auto & ptr) {
        //
        return *ptr == 777;
    });
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() * 1e6 << "us\n";
    std::cerr << "it = " << **it << std::endl;

    std::cerr << "quit" << std::endl;
}

TEST(BDSMeshTest, test4)
{
    std::map<int, Qtr<int>> s;
    // s.reserve(1000000);
    for (int i = 1; i <= 1000000; i++) {
        // auto q = ;
        s.emplace(i, Qtr<int>::alloc(i));
    }
    std::cerr << "sz = " << sizeof(Qtr<int>) << std::endl;

    // auto it = s.find()

    std::cerr << "find" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto it = s.find(777);

    // auto it = std::find_if(s.begin(), s.end(), [](const auto & ptr) {
    //     //
    //     return *ptr == 777;
    // });
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() * 1e6 << "us\n";

    std::cerr << "it = " << *it->second << std::endl;

    std::cerr << "quit" << std::endl;
}

TEST(BDSMeshTest, test5)
{
    // std::map<int, Qtr<ABC>> s;
}
