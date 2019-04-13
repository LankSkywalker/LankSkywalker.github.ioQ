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

#include "cheatparse.h"
#include "dialogs/cheattree.h"
#include "error.h"
#include <cstdlib>
#include <cstring>
#include <utility>
#include <vector>


static bool isWhitespace(char c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}


// A string type made for parsing cheat files.
struct Str
{
    const char *data;
    size_t len;

    Str nextLine() const
    {
        if (len == 0) {
            return {NULL, 0};
        }
        for (size_t i = 0; i < len; i++) {
            if (data[i] == '\n') {
                return {data, i + 1};
            }
        }
        return *this;
    }

    Str nextWord() const
    {
        size_t i = 0;
        for (; i < len; i++) {
            if (!isWhitespace(data[i])) {
                break;
            }
        }
        size_t start = i;
        for (; i < len; i++) {
            if (isWhitespace(data[i])) {
                return {data + start, i - start};
            }
        }
        return {data + start, len - start};
    }

    bool startsWith(const char *s) const
    {
        for (size_t i = 0; s[i]; i++) {
            if (i >= len || data[i] != s[i]) {
                return false;
            }
        }
        return true;
    }

    Str after(size_t pos) const
    {
        if (pos >= len) {
            return {nullptr, 0};
        }
        return {data + pos, len - pos};
    }

    Str afterStr(Str s) const
    {
        if (s.data == nullptr) {
            return {nullptr, 0};
        }
        return after(s.data - data + s.len);
    }

    Str until(char ch) const
    {
        size_t i = 0;
        for (; i < len; i++) {
            if (data[i] == ch) {
                break;
            }
        }
        return {data, i};
    }

    Str trimmed() const
    {
        if (len == 0) {
            return {data, len};
        }
        size_t l = 0;
        while (l < len && isWhitespace(data[l])) {
            l++;
        }
        int r = len - 1;
        while (r >= l && isWhitespace(data[r])) {
            r--;
        }
        return {data + l, (size_t)r - l + 1};
    }

    bool eq(Str other) const
    {
        if (len != other.len) {
            return false;
        }
        return memcmp(data, other.data, len) == 0;
    }

    bool eq(const char *other) const
    {
        if (len != strlen(other)) {
            return false;
        }
        return memcmp(data, other, len) == 0;
    }

    uint32_t parseU32(int base = 10) const
    {
        uint32_t val = 0;
        for (size_t i = 0; i < len; i++) {
            const char c = data[i];
            val *= base;
            if (c >= '0' && c <= '9') {
                val += c - '0';
            } else if (c >= 'A' && c <= 'Z') {
                val += c - 'A' + 10;
            } else if (c >= 'a' && c <= 'z') {
                val += c - 'a' + 10;
            } else {
                abort();
            }
        }
        return val;
    }

    const QString toQString() const
    {
        return QByteArray::fromRawData(data, len);
    }

    const char *newCString() const
    {
        char *ptr = static_cast<char*>(malloc(len + 1));
        strncpy(ptr, data, len);
        ptr[len] = '\0';
        return ptr;
    }
};


// Finds the first line in haystack that makes predicate return true.
bool findLine(Str &haystack, bool (*predicate)(Str))
{
    for (;;) {
        Str line = haystack.nextLine();
        if (line.len == 0) {
            return false;
        }
        if (predicate(line)) {
            return true;
        }
        haystack = haystack.after(line.len);
    }
}


bool findGameSection(Str &haystack, Str section) {
    for (;;) {
        bool found = findLine(haystack, [](Str line) {
            return line.startsWith("crc ");
        });
        Str line = haystack.nextLine();
        if (!found) {
            return false;
        } else if (line.after(4).trimmed().eq(section)) {
            return true;
        } else {
            haystack = haystack.after(line.len);
        }
    }
}


bool findNextCheat(Str &haystack) {
    findLine(haystack, [](Str line) {
        return line.trimmed().startsWith("cn ")
            || line.startsWith("crc ");
    });
    Str line = haystack.nextLine();
    if (line.trimmed().startsWith("cn ")) {
        return true;
    } else {
        return false;
    }
}


bool findNextCheatCode(Str &haystack) {
    bool found = findLine(haystack, [](Str line) {
        line = line.trimmed();
        if (line.len == 0) {
            return false;
        }
        bool isCodeLine = (line.data[0] >= '0' && line.data[0] <= '9')
                       || (line.data[0] >= 'A' && line.data[0] <= 'F');
        return isCodeLine
            || line.startsWith("cn ")
            || line.startsWith("crc ");
    });
    Str line = haystack.nextLine();
    if (found && line.trimmed().data[0] < 'a') {
        return true;
    } else {
        return false;
    }
}


static Cheat &addCheat(Cheat &rootCheat, Str fullName,
                       const std::set<QString> &activeCheats)
{
    Cheat *parent = &rootCheat;
    Str namePart = fullName.until('\\');
    while (namePart.len) {
        auto child = parent->children.find(namePart.toQString());
        if (child == parent->children.end()) {
            Cheat newCheat {
                namePart.toQString(),
                fullName.toQString(),
                "",
                parent,
                activeCheats.count(fullName.toQString()),
            };
            parent->children.insert({newCheat.name, newCheat}).second;
            parent = &parent->children.find(newCheat.name)->second;
        } else {
            parent = &child->second;
        }

        namePart = fullName.afterStr(namePart).after(1).until('\\');
    }

    // After the loop, parent is the latest added cheat.
    return *parent;
}


bool parseCheats(const char *codePtr, size_t codeSize, const char *section,
                 const std::set<QString> &activeCheats, Cheat &rootCheat)
{
    Str code {codePtr, codeSize};

    if (!findGameSection(code, {section, strlen(section)})) {
        return false;
    }

    code = code.after(code.nextLine().len);

    Cheat *currentCheat = nullptr;
    std::vector<CheatCode> cheatCodes;

    while (code.len) {
        Str line = code.nextLine();
        Str firstWord = code.nextWord();

        // Skip comments
        if (firstWord.startsWith("//")) {
        }
        // Game section
        else if (firstWord.eq("crc")) {
            if (currentCheat && !cheatCodes.empty()) {
                currentCheat->codes = std::move(cheatCodes);
            }
            break;
        }
        // Cheat name
        else if (firstWord.eq("cn")) {
            if (currentCheat && !cheatCodes.empty()) {
                currentCheat->codes = std::move(cheatCodes);
            }
            Str fullName = line.afterStr(firstWord).trimmed();
            currentCheat = &addCheat(rootCheat, fullName, activeCheats);
            cheatCodes.clear();
        }
        // No reason to do the rest if we haven't seen a cheat name yet.
        else if (!currentCheat) {
            // Nothing here. This is just to skip the rest.
        }
        // Cheat description
        else if (firstWord.eq("cd")) {
            Str desc = line.afterStr(firstWord).trimmed();
            currentCheat->description = desc.toQString();
        }
        // Cheat code
        else if (firstWord.len == 8) {
            Str addr = firstWord;
            Str val = line.afterStr(addr).nextWord();

            if (val.len > 0 && val.data[0] != '?') {
                cheatCodes.push_back({addr.parseU32(16), val.parseU32(16)});
            } else {
                Str options = line.afterStr(val).trimmed();
                currentCheat->optionsFor = cheatCodes.size();
                cheatCodes.push_back({addr.parseU32(16), 0});
                if (currentCheat->options.size()) {
                    LOG_W("Options for two cheat codes in the same cheat "
                          "is not supported.");
                }
                for (;;) {
                    Str val = options.until(':');
                    if (val.len == 0) {
                        break;
                    }
                    Str rest = options.after(6);
                    size_t i;
                    char prevChar = '\0';
                    for (i = 0; i < rest.len; i++) {
                        if (rest.data[i] == '"' && prevChar != '\\') {
                            break;
                        }
                        prevChar = rest.data[i];
                    }
                    Str desc = {rest.data, i};
                    uint16_t num = val.parseU32(16);
                    currentCheat->options.insert({num, desc.toQString()});
                    // after(2) is after the closing " and then comma.
                    options = options.afterStr(desc).after(2);
                }
            }
        }

        code = code.afterStr(line);
    }

    return true;
}
