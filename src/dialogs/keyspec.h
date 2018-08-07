/***
 * Copyright (c) 2018, Robert Alm Nilsson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the organization nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ***/

#ifndef KEYSPEC_H
#define KEYSPEC_H

#include <vector>
#include <cassert>
#include <QString>


// A KeySpec is a parsed config value, like button(2) or axis(0-,0+).
class KeySpec
{
public:
    enum Type {
        KEY, BUTTON, AXIS, HAT
    };

    // A Value is a thing inside the parentheses, eg 2 or 0+
    struct Value {
        enum Sign {
            NO_SIGN = 0,
            MINUS = '-',
            PLUS  = '+',
        };
        enum Direction {
            UP,
            DOWN,
            LEFT,
            RIGHT,
        };
        Value()
            : number(-1)
        {}
        Value(int number)
            : number(number)
            , sign(NO_SIGN)
        {}
        Value(int number, Sign sign)
            : number(number)
            , sign(sign)
        { }
        Value(int number, Direction dir)
            : number(number)
            , direction(dir)
        {
            assert(dir >= 0 && dir < 4);
        }
        Value invertedSign() const
        {
            return Value(number, Sign(sign ? sign ^ 6 : sign));
        }
        Value invertedDirection() const
        {
            return Value(number, Direction(direction ^ 1));
        }
        int number;
        union {
            Sign sign;
            Direction direction;
        };
    };

    KeySpec() {
    }

    KeySpec(Type type, Value value1)
        : type(type)
        , values{value1}
    {}

    KeySpec(Type type, Value value1, Value value2)
        : type(type)
        , values{value1, value2}
    {}

    KeySpec &operator=(const KeySpec &other)
    {
        type = other.type;
        values = other.values;
        return *this;
    }

    void addValue(const Value &v)
    {
        values.push_back(v);
    }

    QString toString() const;

    // Parses one key spec, eg "axis(0-,0+)" and updates k and str.
    static bool parseOne(KeySpec &k, const char **str);

    Type type;
    std::vector<Value> values;
};


#endif // KEYSPEC_H
