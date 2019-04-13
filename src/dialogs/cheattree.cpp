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

#include "cheattree.h"

CheatTree::CheatTree(QWidget *parent)
    : QTreeView(parent)
{
}


CheatModel::CheatModel(QWidget *parent)
    : QAbstractItemModel(parent), cheats("", "", "", nullptr, false)
{
}


QModelIndex CheatModel::index(int row, int column,
        const QModelIndex &parent) const
{
    const Cheat *parentCheat = static_cast<Cheat*>(parent.internalPointer());
    if (parentCheat == nullptr) {
        parentCheat = &cheats;
    }
    if (parentCheat->children.size() > 0) {
        int i = 0;
        for (auto &c : parentCheat->children) {
            if (i == row) {
                return createIndex(row, column, const_cast<Cheat*>(&c.second));
            }
            i++;
        }
    }
    return QModelIndex();
}


QModelIndex CheatModel::parent(const QModelIndex &index) const
{
    Cheat *childCheat = static_cast<Cheat*>(index.internalPointer());
    if (childCheat->parent && childCheat->parent != &cheats) {
        return createIndex(0, 0, childCheat->parent);
    }
    return QModelIndex();
}

int CheatModel::rowCount(const QModelIndex &parent) const
{
    Cheat *parentCheat = static_cast<Cheat*>(parent.internalPointer());
    if (parentCheat == nullptr) {
        return cheats.children.size();
    }
    return parentCheat->children.size();
}

int CheatModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant CheatModel::data(const QModelIndex &index, int role) const
{
    Cheat *cheat = static_cast<Cheat*>(index.internalPointer());
    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
            if (cheat->options.empty()) {
                return cheat->name;
            } else {
                return cheat->name + "...";
            }
        }
    } else if (role == Qt::CheckStateRole && cheat->children.empty()
               && index.column() == 0) {
        return cheat->checked ? Qt::Checked : Qt::Unchecked;
    } else if (role == Qt::ToolTipRole && !cheat->description.isEmpty()) {
        return "<span>" + cheat->description + "</span>";
    }
    return QVariant();
}

Qt::ItemFlags CheatModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == 0) {
        flags |= Qt::ItemIsUserCheckable;
    }
    return flags;
}

bool CheatModel::setData(const QModelIndex &index, const QVariant &value,
        int role)
{
    if (index.column() == 0 && role == Qt::CheckStateRole) {
        Cheat *cheat = static_cast<Cheat*>(index.internalPointer());
        cheat->checked = value.toBool();
        emit dataChanged(index, index);
        return true;
    }
    return QAbstractItemModel::setData(index, value, role);
}

QVariant CheatModel::headerData(int section, Qt::Orientation o, int role) const
{
    return QString();
}

bool CheatModel::toggle(const QModelIndex &index)
{
    Cheat *cheat = static_cast<Cheat*>(index.internalPointer());
    cheat->checked = !cheat->checked;
    return cheat->checked;
}
