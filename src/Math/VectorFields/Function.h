#ifndef Magnum_Math_VectorFields_Function_h
#define Magnum_Math_VectorFields_Function_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <functional>

namespace Magnum { namespace Math { namespace VectorFields {

template<char character, class T> class VariableValue {
    VariableValue(const VariableValue<character, T>&) = delete;
    VariableValue(VariableValue<character, T>&&) = delete;
    VariableValue<character, T>& operator=(const VariableValue<character, T>&) = delete;
    VariableValue<character, T>& operator=(VariableValue<character, T>&&) = delete;

    public:
        static const char Character = character;

        inline constexpr VariableValue(T value): _value(value) {}

        inline constexpr operator T() const { return _value; }

    private:
        T _value;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<char character, class T> inline constexpr T extract() {
        throw;
    }
    template<char character, class T, class ...U> inline constexpr T extract(const VariableValue<character, T>& first, const U&...) {
        return first;
    }
    template<char character, class T, class U, class ...V> inline constexpr typename std::enable_if<!std::is_same<U, VariableValue<character, T>>::value, T>::type extract(const U&, const V&... next) {
        return extract<character, T>(next...);
    }
}
#endif

template<char character, class T> class Variable {
    public:
        static const char Character = character;

        template<class ...U> T operator()(U... values) {
            return Implementation::extract<character>(values...);
        }
};

template<class T, class Operand, class Functor> class UnaryOperator {
    public:
        UnaryOperator(const Operand& operand): operand(operand) {}

        template<class ...U> T operator()(U... values) {
            return Functor()(operand(values...));
        }

    private:
        Operand operand;
};

template<class T, class LeftOperand, class RightOperand, class Functor> class BinaryOperator {
    public:
        BinaryOperator(const Operand& left, const Operand& right) {}

        template<class ...U> T operator()(U... values) {
            return Functor()(leftOperand(values...), rightOperand(values...));
        }

    protected:
        LeftOperand leftOperand;
        RightOperand rightOperand;
};

}}}

#endif
