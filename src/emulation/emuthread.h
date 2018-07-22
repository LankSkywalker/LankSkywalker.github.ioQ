#ifndef EMUTHREAD_H
#define EMUTHREAD_H

#include <QThread>
#include <QDir>
#include <QString>

class EmuThread : public QThread
{
    Q_OBJECT

public:
    EmuThread(QString romFileName, QString zipFileName);

private:
    void run() Q_DECL_OVERRIDE;

    QString romFileName;
    QString zipFileName;
};

#endif // EMUTHREAD_H
