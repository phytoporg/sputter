#include <iostream>
#include <string>
#include <array>
#include <algorithm>
#include <stdexcept>
#include <cstdio>

#include <gtest/gtest.h>
#include <sputter/memory/reservedregion.h>
#include <sputter/containers/fixedmemoryvector.h>

struct BasicConstructorTestClass
{
    BasicConstructorTestClass(const std::string& s)
    {
        if (s.length() > StringStorage.max_size())
        {
            throw std::runtime_error("s is too large in BasicConstructorTestClass()");
        }

        std::fill(std::begin(StringStorage), std::end(StringStorage), 0);
        for (size_t i = 0; i < s.length(); ++i)
        {
            StringStorage[i] = s[i];
        }
        Length = s.length();
    }

    BasicConstructorTestClass()
        : BasicConstructorTestClass("default")
    {}

    std::string String()
    {
        return std::string(&StringStorage[0], &StringStorage[Length]);
    }

    std::array<char, 0x10> StringStorage;
    size_t Length;
};

using namespace sputter;
memory::ReservedRegion reservedRegion(0x2000);

TEST(create_vector, containers_test)
{
    // Just... don't throw?
    memory::FixedMemoryAllocator allocator(
        "allocator",
        reservedRegion.GetRegionBase(),
        reservedRegion.GetRegionSize());
    containers::FixedMemoryVector<BasicConstructorTestClass> vector(10, allocator);
}

TEST(capacity, containers_test)
{
    // Just... don't throw?
    memory::FixedMemoryAllocator allocator(
        "allocator",
        reservedRegion.GetRegionBase(),
        reservedRegion.GetRegionSize());
    containers::FixedMemoryVector<BasicConstructorTestClass> vector(10, allocator);
    EXPECT_EQ(10, vector.Capacity());
}

TEST(emplace_one_entry, containers_test)
{
    memory::FixedMemoryAllocator allocator(
        "allocator",
        reservedRegion.GetRegionBase(),
        reservedRegion.GetRegionSize());
    containers::FixedMemoryVector<BasicConstructorTestClass> vector(10, allocator);

    const std::string TestStringValue("foo");
    vector.Emplace(TestStringValue);
    EXPECT_EQ(1, vector.Size());
    EXPECT_EQ(TestStringValue, vector[0].String());
}

TEST(add_one_entry, containers_test)
{
    memory::FixedMemoryAllocator allocator(
        "allocator",
        reservedRegion.GetRegionBase(),
        reservedRegion.GetRegionSize());
    containers::FixedMemoryVector<BasicConstructorTestClass> vector(10, allocator);

    const std::string TestStringValue("foo");
    vector.Add(BasicConstructorTestClass(TestStringValue));
    EXPECT_EQ(1, vector.Size());
    EXPECT_EQ(TestStringValue, vector[0].String());
}

TEST(emplace_ten_entries, containers_test)
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
        const int x = atoi(vector[i].String().c_str());
        EXPECT_EQ(x, i);
    }
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
        vector.Add(BasicConstructorTestClass(TestStringValue));
    }

    EXPECT_EQ(10, vector.Size());

    for (int i = 0; i < 10; ++i)
    {
        const int x = atoi(vector[i].String().c_str());
        EXPECT_EQ(x, i);
    }
}

TEST(clear_all_entries, containers_test)
{
    memory::FixedMemoryAllocator allocator(
        "allocator",
        reservedRegion.GetRegionBase(),
        reservedRegion.GetRegionSize());
    containers::FixedMemoryVector<BasicConstructorTestClass> vector(10, allocator);

    for (int i = 0; i < 10; ++i)
    {
        const std::string TestStringValue(std::to_string(i));
        vector.Add(BasicConstructorTestClass(TestStringValue));
    }

    EXPECT_EQ(10, vector.Size());
    vector.Clear();
    EXPECT_EQ(0, vector.Size());
    EXPECT_EQ(vector.Empty(), true);
}

TEST(add_then_remove, containers_test)
{
    memory::FixedMemoryAllocator allocator(
        "allocator",
        reservedRegion.GetRegionBase(),
        reservedRegion.GetRegionSize());
    containers::FixedMemoryVector<BasicConstructorTestClass> vector(10, allocator);

    for (size_t i = 0; i < 10; ++i)
    {
        const std::string TestStringValue(std::to_string(i));
        vector.Add(BasicConstructorTestClass(TestStringValue));
    }

    EXPECT_EQ(10, vector.Size());

    vector.Remove(5);
    EXPECT_EQ(9, vector.Size());

    bool foundValue = false;
    for (size_t i = 0; i < vector.Size(); ++i)
    {
        if (vector[i].String() == "5")
        {
            foundValue = true;
        }
    }

    EXPECT_EQ(foundValue, false);
}

TEST(resize, containers_test)
{
    memory::FixedMemoryAllocator allocator(
        "allocator",
        reservedRegion.GetRegionBase(),
        reservedRegion.GetRegionSize());
    containers::FixedMemoryVector<BasicConstructorTestClass> vector(11, allocator);

    for (size_t i = 0; i < 10; ++i)
    {
        const std::string TestStringValue(std::to_string(i));
        vector.Add(BasicConstructorTestClass(TestStringValue));
    }

    EXPECT_EQ(10, vector.Size());
    vector.Resize(5);
    EXPECT_EQ(5, vector.Size());
    vector.Resize(11);
    EXPECT_EQ(11, vector.Size());

    for (size_t i = 5; i < vector.Size(); ++i)
    {
       EXPECT_EQ(vector[i].String(), "default");
    }
}

// TODO: New stuff -- Iterators
