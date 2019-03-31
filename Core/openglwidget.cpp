#include "openglwidget.h"

#include <QDebug>

OpenGLWidget::OpenGLWidget(QWidget *parent)
    :QOpenGLWidget(parent)
{
    //版本设置
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3,3);
    setFormat(format);

}

OpenGLWidget::~OpenGLWidget()
{
    //
}

void OpenGLWidget::initializeGL()
{
    //初始化OpenGL函数
    initializeOpenGLFunctions();
    //设置全局变量
    glClearColor(0.94f, 0.94f, 0.94f,1.00f);//背景色
}

void OpenGLWidget::paintGL()
{
    //清理屏幕
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWidget::resizeGL(int width, int height)
{
    //This virtual function is called whenever the widget has been resized. Reimplement it in a subclass. The new size is passed in w and h.

    //未使用
    Q_UNUSED(width);
    Q_UNUSED(height);
}

