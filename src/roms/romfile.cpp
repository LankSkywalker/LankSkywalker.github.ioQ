#include "romfile.h"
#include "../error.h"
#include "../common.h"

#if QT_VERSION >= 0x050000
#include <quazip5/quazip.h>
#include <quazip5/quazipfile.h>
#else
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#endif


void readRomFile(QByteArray &romData,
        const QString &romFileName,
        const QString &zipFileName)
{
    if (zipFileName != "") {
        QuaZipFile zippedFile(zipFileName, romFileName);

        zippedFile.open(QIODevice::ReadOnly);
        romData.append(zippedFile.readAll());
        zippedFile.close();
    } else {
        QFile romFile(romFileName);
        if (!romFile.exists()) {
            SHOW_W(TR("ROM file not found."));
            return;
        }

        romFile.open(QIODevice::ReadOnly);
        romData.append(romFile.readAll());
        romFile.close();
    }
}
