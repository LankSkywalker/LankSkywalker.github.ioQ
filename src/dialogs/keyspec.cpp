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

#include "keyspec.h"
#include "../error.h"
#include "../common.h"

#include <QRegularExpression>


bool KeySpec::parseOne(KeySpec &k, const char **str)
{
    typedef QRegularExpression              Re;
    typedef QRegularExpressionMatch         ReMatch;
    typedef QRegularExpressionMatchIterator ReMatchIt;

    ReMatch match = Re("^ *(\\w+) *\\(([^)]*)\\)").match(*str);
    if (!match.hasMatch()) {
        return 0;
    }
    QString name = match.captured(1);
    QString insidePar = match.captured(2);
    ReMatchIt iMatches = Re(" *(\\d+) *([\\w+-]*) *,? *").globalMatch(insidePar);
    if (!iMatches.hasNext()) {
        LOG_W(TR("Input config has no arguments: ") + *str);
        *str += match.captured(0).toUtf8().length();
        return false;
    }
    ReMatch arg1 = iMatches.next();
    *str += match.captured(0).length();

    auto getVal = [](const ReMatch &m) {
        int n = m.captured(1).toInt();
        char sign = m.captured(2).toUtf8()[0];
        if (sign == '-' || sign == '+' || sign == Value::NO_SIGN) {
            return Value(n, Value::Sign(sign));
        } else {
            QString d = m.captured(2);
            Value::Direction dir = Value::Direction::UP;
            if (d.compare("up", Qt::CaseInsensitive) == 0) {
                dir = Value::UP;
            } else if (d.compare("down", Qt::CaseInsensitive) == 0) {
                dir = Value::DOWN;
            } else if (d.compare("left", Qt::CaseInsensitive) == 0) {
                dir = Value::LEFT;
            } else if (d.compare("right", Qt::CaseInsensitive) == 0) {
                dir = Value::RIGHT;
            } else {
                LOG_W(TR("Unknown direction: ") + d);
            }
            return Value(n, dir);
        }
    };

    if (name == "key") {
        k = KeySpec(KEY, getVal(arg1));
    } else if (name == "button") {
        k = KeySpec(BUTTON, getVal(arg1));
    } else if (name == "axis") {
        k = KeySpec(AXIS, getVal(arg1));
    } else if (name == "hat") {
        k = KeySpec(HAT, getVal(arg1));
    } else {
        LOG_W(TR("Unknown input config: ") + name);
        return false;
    }

    while (iMatches.hasNext()) {
        k.addValue(getVal(iMatches.next()));
    }

    return true;
}


QString KeySpec::toString() const
{
    const char *hatDirections[] = {
        "Up",
        "Down",
        "Left",
        "Right",
    };
    QString s;
    switch (type) {
    case KEY:    s += "key";    break;
    case BUTTON: s += "button"; break;
    case AXIS:   s += "axis";   break;
    case HAT:    s += "hat";    break;
    }
    bool firstIteration = true;
    s += "(";
    for (const Value &v : values) {
        if (!firstIteration) {
            s += ",";
        }
        if (v.number >= 0) {
            s += QString::number(v.number);
            if (type == AXIS && v.sign) {
                s += v.sign;
            }
            if (type == HAT) {
                s += QString(" ") + hatDirections[v.direction];
            }
        }
        firstIteration = false;
    }
    s += ")";
    return s;
}
