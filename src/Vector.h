#ifndef Magnum_Vector_h
#define Magnum_Vector_h
/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Vector
 */

#include <cstring>
#include <cmath>

namespace Magnum {

/** @brief Vector */
template<class T, size_t size> class Vector {
    public:
        /** @brief Default constructor */
        inline Vector() {
            memset(_data, 0, size*sizeof(T));
        };

        /**
         * @brief Constructor
         * @param data  Array of @c size length.
         */
        inline Vector(const T* data) { setData(data); }

        /** @brief Copy constructor */
        inline Vector(const Vector<T, size>& other) {
            setData(other.data());
        }

        /** @brief Assignment operator */
        inline Vector<T, size>& operator=(const Vector<T, size>& other) {
            setData(other.data());
            return *this;
        }

        /**
         * @brief Raw data
         * @return Array of @c size length.
         */
        inline const T* data() const { return _data; }

        /**
         * @brief Set raw data
         * @param data Array of @c size length.
         */
        inline void setData(const T* data) {
            memcpy(_data, data, size*sizeof(T));
        }

        /** @brief Value at given position */
        inline T at(size_t pos) const { return _data[pos]; }

        /** @brief Value at given position */
        inline T operator[](size_t pos) const { return _data[pos]; }

        /** @brief Reference to value at given position */
        inline T& operator[](size_t pos) { return _data[pos]; }

        /** @brief Set value at given position */
        inline void set(size_t pos, T value) { _data[pos] = value; }

        /** @brief Add value to given position */
        inline void add(size_t pos, T value) { _data[pos] += value; }

        /** @brief Equality operator */
        inline bool operator==(const Vector<T, size>& other) const {
            return memcmp(_data, other.data(), size*sizeof(T)) == 0;
        }

        /** @brief Non-equality operator */
        inline bool operator!=(const Vector<T, size>& other) const {
            return !operator==(other);
        }

        /** @brief Dot product */
        T operator*(const Vector<T, size>& other) const {
            T out;

            for(size_t i = 0; i != size; ++i)
                out += at(i)*other.at(i);

            return out;
        }

        /** @brief Multiply vector */
        Vector<T, size> operator*(T number) const {
            Vector<T, size> out;

            for(size_t i = 0; i != size; ++i)
                out.set(i, at(i)*number);

            return out;
        }

        /** @brief Divide vector */
        Vector<T, size> operator/(T number) const {
            Vector<T, size> out;

            for(size_t i = 0; i != size; ++i)
                out.set(i, at(i)/number);

            return out;
        }

        /** @brief Add two vectors */
        Vector<T, size> operator+(const Vector<T, size>& other) const {
            Vector<T, size> out;

            for(size_t i = 0; i != size; ++i)
                out.set(i, at(i)+other.at(i));

            return out;
        }

        /** @brief Substract two vectors */
        Vector<T, size> operator-(const Vector<T, size>& other) const {
            Vector<T, size> out;

            for(size_t i = 0; i != size; ++i)
                out.set(i, at(i)-other.at(i));

            return out;
        }

        /** @brief Vector length */
        T length() const {
            T out;
            for(size_t i = 0; i != size; ++i)
                out += pow(at(i), 2);

            return sqrt(out);
        }

        /** @brief Normalized vector (of length 1) */
        Vector<T, size> normalized() const {
            return *this/length();
        }

    private:
        T _data[size];
};

}

#endif
