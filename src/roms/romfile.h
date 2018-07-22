#ifndef ROMFILE_H
#define ROMFILE_H

#include <QByteArray>
#include <QString>


// Reads the contents of the ROM file, optionally inside a ZIP file,
// and appends it to romData.
void readRomFile(QByteArray &romData,
        const QString &romFileName,
        const QString &zipFileName = "");


#endif // ROMFILE_H
