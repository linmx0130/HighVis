#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QElapsedTimer>
#include "volumndata.h"
class GLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    GLWidget(const char* filename, QWidget *partent);
    ~GLWidget();
    QVector3D getCameraPos() {
        return visLookFrom;
    }
    QVector3D getCameraDirection() {
        return visLookAt - visLookFrom;
    }
    void setInterpolationType(float t) {
        this->interpolationType = t;
        update();
    }
    void setLightColor(QColor color) {
        this->lightColor = QVector4D(color.red() / 256.0,
                                     color.green()/256.0, color.blue()/256.0, 1.0);
        update();
    }
    void setAlphaThreshold(float t){
        this->alphaThreshold = t;
        update();
    }
signals:
    void stateChanged();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void keyPressEvent(QKeyEvent *event);
    void initVolumnTexture();
private:
    QOpenGLBuffer *m_vbo;
    QOpenGLVertexArrayObject *m_vao;
    QOpenGLShaderProgram *m_shader;
    GLuint m_volumn_texture;
    QVector3D camera_pos;
    QVector3D camera_direction;
    VolumnData volumnData;
    float aspectRatio;
    // vis view parameters
    QVector3D visLookFrom, visLookAt, visLookUp;
    QVector4D lightColor;
    float alphaThreshold;
    float interpolationType;
};

#endif // GLWIDGET_H
