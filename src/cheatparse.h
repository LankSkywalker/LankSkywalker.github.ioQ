/***
 * Copyright (c) 2019, Robert Alm Nilsson
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

#ifndef CHEATPARSE_H
#define CHEATPARSE_H

#include <set>
#include <map>
#include <vector>
#include <QString>
#include <m64p_types.h>


class QString;

using CheatCode = m64p_cheat_code;

struct Cheat
{
    Cheat(QString name, QString fullName, QString description,
            Cheat *parent, bool on)
        : name(name), fullName(fullName), description(description),
          parent(parent), checked(on)
    {}
    Cheat(const Cheat &other) = default;
    Cheat &operator=(const Cheat &other) = default;
    QString name;
    QString fullName;
    QString description;
    Cheat *parent;
    std::map<QString, Cheat> children;
    std::vector<CheatCode> codes;  // Only valid if size of children == 0.
    bool checked;
    size_t optionsFor;
    std::map<uint16_t, QString> options;
};


bool parseCheats(const char *code, size_t codeSize, const char *section,
                 const std::set<QString> &activeCheats, Cheat &rootCheat);

#endif // CHEATPARSE_H
