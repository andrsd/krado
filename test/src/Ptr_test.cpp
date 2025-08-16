#include <gmock/gmock.h>
#include "krado/ptr.h"
#include "krado/exception.h"

using namespace krado;

namespace {

class MyBigObj {
public:
    MyBigObj(int size) : sz(size) { this->a = new int[size]; }

    ~MyBigObj() { delete[] this->a; }

    int
    size() const
    {
        return this->sz;
    }

    void
    resize(int new_size)
    {
        delete[] this->a;
        this->a = new int[new_size];
        this->sz = new_size;
    }

private:
    int sz;
    int * a;
};

class Base {
public:
    Base() : a(1) {}

    virtual int
    value() const
    {
        return this->a;
    }

private:
    int a;
};

class Derived : public Base {
public:
    Derived() : Base(), b(2) {}

    int
    value() const override
    {
        return this->b;
    }

private:
    int b;
};

std::vector<Ptr<Derived>>
create_objs()
{
    std::vector<Ptr<Derived>> vec;
    vec.push_back(Ptr<Derived>::alloc());
    vec.push_back(Ptr<Derived>::alloc());
    return vec;
}

} // namespace

TEST(PtrTest, nullptr_is_null)
{
    Ptr<MyBigObj> obj = nullptr;
    EXPECT_TRUE(obj.is_null());
    EXPECT_TRUE(obj == nullptr);
}

TEST(PtrTest, allocate)
{
    auto obj = Ptr<MyBigObj>::alloc(10000);
    EXPECT_EQ(obj.ref_count(), 1);
}

TEST(PtrTest, ptr_access)
{
    auto obj = Ptr<MyBigObj>::alloc(1359);
    EXPECT_EQ(obj->size(), 1359);
    EXPECT_EQ((*obj).size(), 1359);
}

TEST(PtrTest, circular_dependency)
{
    bool a_deleted = false;
    bool b_deleted = false;

    class B;

    class A {
    public:
        A(bool * del) : deleted(del) {}
        ~A() { *this->deleted = true; }

        Ptr<B> b_ptr;

    private:
        bool * deleted;
    };

    class B {
    public:
        B(bool * del) : deleted(del) {}
        ~B() { *this->deleted = true; }

        Ptr<A> a_ptr;

    private:
        bool * deleted;
    };

    Ptr<A> glob;
    {
        auto a = Ptr<A>::alloc(&a_deleted);
        auto b = Ptr<B>::alloc(&b_deleted);

        a->b_ptr = b;
        b->a_ptr = a;

        EXPECT_EQ(a.ref_count(), 2);
        EXPECT_EQ(b.ref_count(), 2);

        glob = a;
    }

    EXPECT_FALSE(a_deleted);
    EXPECT_FALSE(b_deleted);

    glob->b_ptr = nullptr;
}

TEST(PtrTest, copy)
{
    auto obj1 = Ptr<MyBigObj>::alloc(10000);
    Ptr<MyBigObj> obj2 = obj1;
    EXPECT_EQ(obj1.ref_count(), 2);
    EXPECT_EQ(obj2.ref_count(), 2);
}

TEST(PtrTest, null_ptr_access)
{
    Ptr<MyBigObj> ptr;
    try {
        *ptr;
        FAIL() << "Expected exception";
    }
    catch (const Exception & e) {
        EXPECT_STREQ(e.what(), "Access into a null pointer");
    }
}

TEST(PtrTest, store_convertible)
{
    auto ptr = Ptr<Derived>::alloc();
    std::vector<Ptr<Base>> storage;
    storage.push_back(ptr);
}

TEST(PtrTest, are_equal)
{
    auto obj1 = Ptr<MyBigObj>::alloc(10000);
    auto obj2 = obj1;
    auto obj3 = Ptr<MyBigObj>::alloc(10000);
    EXPECT_EQ(obj1, obj2);
    EXPECT_NE(obj1, obj3);
}

TEST(PtrTest, copy_ctors)
{
    auto vecs = create_objs();
    EXPECT_EQ(vecs.size(), 2);
}

TEST(PtrTest, copy_ctors2)
{
    auto vecs = create_objs();
    std::vector<Ptr<Base>> base_vec;
    std::vector<Ptr<Derived>> der_vec;
    der_vec.reserve(3);
    for (auto v : vecs) {
        base_vec.push_back(v);
        der_vec.push_back(v);
    }
    EXPECT_EQ(base_vec.size(), 2);
}

TEST(PtrTest, deallocate_via_nullptr)
{
    auto ptr = Ptr<Derived>::alloc();
    ptr = nullptr;
    EXPECT_EQ(ptr.ref_count(), 0);
}

TEST(PtrTest, move_operators)
{
    auto a = Ptr<int>::alloc(5);
    // move ctor
    Ptr<int> b = std::move(a);
    EXPECT_EQ(a.ref_count(), 0);
    EXPECT_EQ(b.ref_count(), 1);

    Ptr<int> c;
    // move assigment
    c = std::move(b);
    EXPECT_EQ(b.ref_count(), 0);
    EXPECT_EQ(c.ref_count(), 1);
}

TEST(PtrTest, op_static_cast)
{
    auto d = Ptr<Derived>::alloc();
    Ptr<Base> a = d;
    Ptr<Derived> b = static_ptr_cast<Derived>(a);
    EXPECT_EQ(b->value(), 2);

    auto c = Ptr<Derived>::cast(a);
    EXPECT_EQ(c->value(), 2);
}

TEST(PtrTest, op_dynamic_cast)
{
    auto d = Ptr<Derived>::alloc();
    Ptr<Base> a = d;
    Ptr<Derived> b = dynamic_ptr_cast<Derived>(a);
    EXPECT_EQ(b->value(), 2);
    EXPECT_EQ(a->value(), 2);

    auto c = Ptr<Derived>::downcast(a);
    EXPECT_EQ(c->value(), 2);
}
