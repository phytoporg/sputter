#include <iostream>
#include <string>
#include <cstdio>

#include <gtest/gtest.h>
#include <sputter/memory/reservedregion.h>
#include <sputter/containers/fixedmemoryvector.h>

struct BasicConstructorTestClass
{
    BasicConstructorTestClass()
        : String("default")
    {}

    BasicConstructorTestClass(const std::string& s)
        : String(s)
    {}

    std::string String;
};

using namespace sputter;
memory::ReservedRegion reservedRegion(0x1000);

TEST(create_vector, containers_test)
{
    // Just... don't throw?
    memory::FixedMemoryAllocator allocator(
        "allocator",
        reservedRegion.GetRegionBase(),
        reservedRegion.GetRegionSize());
    containers::FixedMemoryVector<BasicConstructorTestClass> vector(10, allocator);
}

TEST(add_one_entry, containers_test)
{
    memory::FixedMemoryAllocator allocator(
        "allocator",
        reservedRegion.GetRegionBase(),
        reservedRegion.GetRegionSize());
    containers::FixedMemoryVector<BasicConstructorTestClass> vector(10, allocator);

    const std::string TestStringValue("foo");
    vector.Emplace(TestStringValue);
    EXPECT_EQ(1, vector.Size());
    EXPECT_EQ(TestStringValue, vector[0].String);
}

TEST(add_ten_entries, containers_test)
{
    memory::FixedMemoryAllocator allocator(
        "allocator",
        reservedRegion.GetRegionBase(),
        reservedRegion.GetRegionSize());
    containers::FixedMemoryVector<BasicConstructorTestClass> vector(10, allocator);

    for (int i = 0; i < 10; ++i)
    {
        const std::string TestStringValue(std::to_string(i));
        vector.Emplace(TestStringValue);
    }

    EXPECT_EQ(10, vector.Size());

    for (int i = 0; i < 10; ++i)
    {
        const int x = atoi(vector[i].String.c_str());
        EXPECT_EQ(x, i);
    }
}
