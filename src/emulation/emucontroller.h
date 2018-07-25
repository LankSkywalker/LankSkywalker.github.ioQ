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
    bool isExecuting();
    void emitResumed();
    void emitPaused();

public slots:
    void play();
    void pause();
    void saveState();
    void loadState();
    void setSaveSlot(int n);
    void resetSoft();
    void resetHard();

signals:
    void createGlWindow(QSurfaceFormat *format);
    void destroyGlWindow();
    void resize(int width, int height);
    void started();
    void resumed();
    void paused();
    void finished();
};

#endif // EMUCONTROLLER_H
