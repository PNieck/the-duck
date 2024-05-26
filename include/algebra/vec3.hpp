#pragma once

#include <array>
#include <cmath>


namespace alg
{
    template <typename DataType>
    class Vector3 {
    public:
        Vector3() = default;
        Vector3(DataType scalar) { data.fill(scalar); }
        constexpr Vector3(DataType v1, DataType v2, DataType v3) {
            data[0] = v1;
            data[1] = v2;
            data[2] = v3;
        }

        inline DataType& X()
            { return data[0]; }

        inline DataType X() const
            { return data[0]; }

        inline DataType& Y()
            { return data[1]; }

        inline DataType Y() const
            { return data[1]; }

        inline DataType& Z()
            { return data[2]; }

        inline DataType Z() const
            { return data[2]; }

        Vector3 Normalize() const {
            float len = Length();

            return Vector3(
                data[0] / len,
                data[1] / len,
                data[2] / len
            );
        }

        inline float LengthSquared() const
            { return data[0]*data[0] + data[1]*data[1] + data[2]*data[2]; }
        
        inline float Length() const
            { return std::sqrt(LengthSquared()); }

        inline DataType* Data()
            { return data.data(); }

        inline const DataType* Data() const
            { return data.data(); }

        Vector3<DataType> operator+(const Vector3<DataType>& v) const {
            return Vector3<DataType>(
                data[0] + v.data[0],
                data[1] + v.data[1],
                data[2] + v.data[2]
            );
        }

        Vector3<DataType>& operator+=(const Vector3<DataType>& v) {
            data[0] += v.data[0];
            data[1] += v.data[1];
            data[2] += v.data[2];
            
            return *this;
        }

        Vector3<DataType>& operator-=(const Vector3<DataType>& v) {
            data[0] -= v.data[0];
            data[1] -= v.data[1];
            data[2] -= v.data[2];
            
            return *this;
        }

        Vector3<DataType>& operator*=(DataType scalar) {
            data[0] *= scalar;
            data[1] *= scalar;
            data[2] *= scalar;
            
            return *this;
        }

        Vector3<DataType> operator-(const Vector3<DataType>& v) const {
            return Vector3<DataType>(
                data[0] - v.data[0],
                data[1] - v.data[1],
                data[2] - v.data[2]
            );
        }


        Vector3<DataType> operator-() const {
            return Vector3<DataType>(
                -data[0],
                -data[1],
                -data[2]
            );
        }


        Vector3<DataType>& operator/=(DataType scalar) {
            data[0] /= scalar;
            data[1] /= scalar;
            data[2] /= scalar;
            
            return *this;
        }      

    protected:
        std::array<DataType, 3> data;
    };


    using Vec3 = Vector3<float>;


    template <typename DataType>
    Vector3<DataType> Cross(const Vector3<DataType>& v1, const Vector3<DataType>& v2) {
        return Vector3<DataType>(
            v1.Y() * v2.Z() - v1.Z() * v2.Y(),
            v1.Z() * v2.X() - v1.X() * v2.Z(),
            v1.X() * v2.Y() - v1.Y() * v2.X()
        );
    }


    template <typename DataType>
    DataType Dot(const Vector3<DataType>& v1, const Vector3<DataType>& v2) {
        return v1.X() * v2.X() + v1.Y() * v2.Y() + v1.Z() * v2.Z();
    }


    template <typename DataType>
    inline DataType Distance(const Vector3<DataType>& v1, const Vector3<DataType>& v2) {
        return (v1 - v2).Length();
    }


    template <typename DataType>
    inline DataType DistanceSquared(const Vector3<DataType>& v1, const Vector3<DataType>& v2) {
        return (v1 - v2).LengthSquared();
    }


    template <typename DataType>
    Vector3<DataType> operator*(DataType scalar, const Vector3<DataType>& v) {
        return Vector3<DataType>(
            v.X() * scalar,
            v.Y() * scalar,
            v.Z() * scalar
        );
    }


    template <typename DataType>
    inline Vector3<DataType> operator*(const Vector3<DataType>& v, DataType scalar)
        { return scalar * v; }


    template <typename DataType>
    inline Vector3<DataType> operator/(const Vector3<DataType>& v, DataType scalar)
        { return v * (1.f / scalar); }


    template <typename DataType>
    bool operator==(const Vector3<DataType>& v1, const Vector3<DataType>& v2) {
        return v1.X() == v2.X() &&
               v1.Y() == v2.Y() &&
               v1.Z() == v2.Z();
    }


    template <typename DataType>
    inline bool operator!=(const Vector3<DataType>& v1, const Vector3<DataType>& v2) {
        return !(v1 == v2);
    }
};
