#ifndef GLWINDOW_H
#define GLWINDOW_H

#include "emuthread.h"
#include <QOpenGLWindow>


class GlWindow : public QOpenGLWindow
{
public:
    void initializeGL() Q_DECL_OVERRIDE;
    void exposeEvent(QExposeEvent *) Q_DECL_OVERRIDE
    {}
};


#endif // GLWINDOW_H
