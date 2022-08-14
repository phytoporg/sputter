#pragma once

#include <stdint.h>
#include <vector>

namespace sputter { namespace render {
    enum class UniformType
    {
        Invalid,
        Int,
        IVec2,
        IVec4,
        Float,
        Vec2,
        Vec3,
        Quat,
        Mat4,
        MaxUniformType
    };

    // For instances where templatization isn't as practical. Not type safe but
    // oh no.
    void SetUniformByType(uint32_t slot, UniformType type, void const* pValue);

    template<typename T>
    class Uniform
    {
    public:
        static void Set(uint32_t slot, const T& value);
        static void Set(uint32_t slot, const T* array, uint32_t arrayLength);
        static void Set(uint32_t slot, std::vector<T>& v);

    private:
        Uniform() = delete;
        Uniform(const Uniform& uniform) = delete;
        Uniform& operator=(const Uniform& uniform) = delete;
        ~Uniform() = delete;
    };

    template<typename T>
    void Uniform<T>::Set(uint32_t slot, const T& value)
    {
        Set(slot, static_cast<const T*>(&value), 1);
    }

    template<typename T>
    void Uniform<T>::Set(uint32_t slot, std::vector<T>& v)
    {
        Set(slot, v.data(), static_cast<uint32_t>(v.size()));
    }
} }
