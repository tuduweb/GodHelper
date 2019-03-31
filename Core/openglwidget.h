#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class OpenGLWidget : public QOpenGLWidget,protected QOpenGLFunctions
{
public:
    explicit OpenGLWidget(QWidget *parent = 0);
    ~OpenGLWidget();

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

};

#endif // OPENGLWIDGET_H
