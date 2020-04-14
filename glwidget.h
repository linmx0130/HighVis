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
#include <cmath>
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
    QVector3D getLightPos(){
        return lightPos;
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
    void setLightPos(float x, float y, float z){
        lightPos = QVector3D(x, y, z);
        update();
        emit stateChanged();
    }
    void setViewAngel(float viewAlpha, float viewTheta){
        this->viewAlpha = viewAlpha;
        this->viewTheta = viewTheta;
        QVector3D v(sin(viewAlpha), cos(viewAlpha) * cos(viewTheta), cos(viewAlpha) * sin(viewTheta));
        visLookFrom = QVector3D(0.5f, 0.5f, 0.5f) + v * 3.0f;
        update();
        emit stateChanged();
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
    float viewAlpha, viewTheta;
    QVector3D lightPos;
    QVector4D lightColor;
    float alphaThreshold;
    float interpolationType;
};

#endif // GLWIDGET_H
