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

#include "cheatdialog.h"
#include "cheattree.h"
#include "../cheatparse.h"
#include "../core.h"
#include "../common.h"
#include "../error.h"
#include "../emulation/emulation.h"
#include <set>
#include <QFile>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QTreeWidget>
#include <QPushButton>
#include <QScrollArea>


static int32_t swap32(int32_t n)
{
    return (n & 0x000000ff) << 24
         | (n & 0x0000ff00) << 8
         | (n & 0x00ff0000) >> 8
         | (n & 0xff000000) >> 24;
}


CheatDialog::CheatDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Cheats");
    resize(400, 400);

    char cheatSection[24] = {};
    m64p_error getHeaderError;
    // Construct the name of the game section in the cheat file.
    {
        m64p_rom_header header;
        getHeaderError = CoreDoCommand(M64CMD_ROM_GET_HEADER,
                                       sizeof header, &header);
        if (getHeaderError == M64ERR_SUCCESS) {
            int l = snprintf(cheatSection,
                             sizeof cheatSection,
                             "%08X-%08X-C:%02X",
                             swap32(header.CRC1),
                             swap32(header.CRC2),
                             header.Country_code & 0xff);
        }
    }

    const char *fileContents;
    size_t fileSize;
    // Open and read the cheat file.
    {
        const char *fileName = ConfigGetSharedDataFilepath("mupencheat.txt");
        cheatFile.setFileName(fileName);
        fileContents = mapFile(cheatFile);
        fileSize = cheatFile.size();
    }

    // Add the tree widget.
    CheatTree *tree = new CheatTree;
    CheatModel *model = new CheatModel;
    tree->setModel(model);
    connect(model, &CheatModel::dataChanged, [](const QModelIndex &topLeft,
                                                const QModelIndex &bottomRight,
                                                const QVector<int> &roles) {
        const QModelIndex &index = topLeft;
        Cheat *cheat = static_cast<Cheat*>(index.internalPointer());
        bool on = cheat->checked;
        if (on && !cheat->options.empty()) {
            if (cheat->optionsFor < cheat->options.size()) {
                int n = OptionsDialog(cheat->options, *cheat).exec();
                if (n == 0) {
                    cheat->checked = false;
                    return;
                }
                n--;
                cheat->codes[cheat->optionsFor].value = n;
            }
        }
        if (on) {
            CoreAddCheat(cheat->fullName.toUtf8().data(), cheat->codes.data(),
                         cheat->codes.size());
            Emulation::activeCheats.insert(cheat->fullName);
        } else {
            Emulation::activeCheats.erase(cheat->fullName);
        }
        CoreCheatEnabled(cheat->fullName.toUtf8().data(), on);
    });

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tree);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    layout->addLayout(buttonLayout);
    QPushButton *clearButton = new QPushButton(TR("Clear all cheats"));
    connect(clearButton, &QPushButton::clicked, [this]() {
        for (auto &c : Emulation::activeCheats) {
            CoreCheatEnabled(c.toUtf8().data(), false);
        }
        Emulation::activeCheats.clear();
        close();
    });
    buttonLayout->addStretch();
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();

    bool parseOk = parseCheats(fileContents, fileSize, cheatSection,
                               Emulation::activeCheats, model->cheats);

    if (parseOk) {
        setLayout(layout);
    } else {
        QVBoxLayout *lay = new QVBoxLayout();
        QLabel *label = new QLabel;
        if (getHeaderError == M64ERR_SUCCESS) {
            label->setText(TR("Game not found in cheat file."));
        } else {
            label->setText(TR("Game is not running."));
        }
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        lay->addWidget(label);
        setLayout(lay);
        return;
    }
}


OptionsDialog::OptionsDialog(const std::map<uint16_t, QString> &values,
                             const Cheat &cheat, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(TR("Specify value"));
    resize(350, 350);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel(cheat.name));
    QLabel *desc = new QLabel(cheat.description);
    desc->setWordWrap(true);
    layout->addWidget(desc);
    setLayout(layout);
    QScrollArea *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    layout->addWidget(scroll);
    QVBoxLayout *scrollLayout = new QVBoxLayout;
    QFrame *frame = new QFrame;
    scroll->setWidget(frame);
    frame->setLayout(scrollLayout);

    for (auto &v : values) {
        QPushButton *b = new QPushButton(v.second);
        scrollLayout->addWidget(b);
        connect(b, &QPushButton::clicked, [=](bool) {
            done(v.first + 1);
        });
    }
    QSpacerItem *space = new QSpacerItem(0, 0, QSizePolicy::Minimum,
                                         QSizePolicy::Expanding);
    scrollLayout->addSpacerItem(space);
}
