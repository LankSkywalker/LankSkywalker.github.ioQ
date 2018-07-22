#ifndef EMUCONTROLLER_H
#define EMUCONTROLLER_H

#include <QObject>
#include <QString>
#include <QDir>
#include <QSurfaceFormat>

class EmuController : public QObject
{
    Q_OBJECT

public:
    EmuController();
    void startGame(const QString &romFileName, const QString &zipFileName = "");
    void stopGame();

signals:
    void createGlWindow(QSurfaceFormat *format);
    void resize(int width, int height);
    void started();
    void finished();
};

#endif // EMUCONTROLLER_H
