#include "glwidget.h"
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QDebug>
#include <QOpenGLExtraFunctions>
static const GLfloat VERTEX_DATA[] = {
    //face 1 : z= 0.5f
    0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
     -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
 };
GLWidget::GLWidget(const char* filename, QWidget *nulltpr):
    QOpenGLWidget(nulltpr), m_vbo(nullptr), m_vao(nullptr), m_shader(nullptr), camera_pos(0.f, 0.f, 2.f),
    camera_direction(0.f, 0.f, 1.f), volumnData(filename), visLookAt(0.5f, 0.5f, 0.5f),
    visLookUp(0.0f, 1.0f, 0.0f), lightPos(0.5f, -2.0f, 0.5f), lightColor(1.0f, 1.0f, 1.0f, 1.0f), alphaThreshold(0.2),
    interpolationType(0.0f), viewAlpha(0), viewTheta(M_PI * 1.5)
{
    setFocusPolicy(Qt::StrongFocus); // enable the widget to receive key press
    setViewAngel(viewAlpha, viewTheta);
    QVector3D v(sin(viewAlpha), cos(viewAlpha) * cos(viewTheta), cos(viewAlpha) * sin(viewTheta));
    visLookFrom = QVector3D(0.5f, 0.5f, 0.5f) + v * 2.0f;
}

GLWidget::~GLWidget() {

}
void GLWidget::initVolumnTexture(){
    QOpenGLExtraFunctions *f = this->context()->extraFunctions();
    f->glDeleteTextures(1, &m_volumn_texture);
    f->glGenTextures(1, &m_volumn_texture);
    f->glBindTexture(GL_TEXTURE_3D, m_volumn_texture);
    f->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // The array on the host has 1 byte alignment
    f->glTexImage3D(GL_TEXTURE_3D, 0, GL_R8,
                 volumnData.getMetaData().size[0],volumnData.getMetaData().size[1], volumnData.getMetaData().size[2],
                 0, GL_RED, GL_UNSIGNED_BYTE, volumnData.getRawData().data());
    f->glBindTexture(GL_TEXTURE_3D, 0);
}
void GLWidget::initializeGL()
{
    // set aspect ration
    this->aspectRatio = (float)this->width() / this->height();
    QOpenGLFunctions *f = this->context()->functions();
    f->glEnable(GL_DEPTH_TEST);
    // loading shaders
    if (m_shader) delete m_shader;
    m_shader = new QOpenGLShaderProgram();
    m_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertexShader.shader");
    m_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragmentShader.shader");
    if (m_shader->link()) {
        qDebug("Shaders link success.");
    } else {
        qDebug("Shaders link failed!");
    }
    initVolumnTexture();
    // build VAO
    if (m_vao) delete m_vao;
    m_vao = new QOpenGLVertexArrayObject();
    m_vao->create();
    m_vao->bind();
    // build VBO
    if (m_vbo) delete m_vbo;
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_vbo->create();
    m_vbo->bind();

    // store vertex to VBO
    m_vbo->allocate(VERTEX_DATA, 6 * 6 * 8 * sizeof(GLfloat));

    // apply data to vertex shader
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    // release objects
    m_vbo->release();
    m_vao->release();
    emit stateChanged();
}
void GLWidget::resizeGL(int w, int h)
{
    aspectRatio = (float)w / h;
}

void GLWidget::paintGL()
{
    QOpenGLFunctions *f = this->context()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_vao->bind();
    m_shader->bind();
    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_3D, m_volumn_texture);
    //build MVP matrix
    QMatrix4x4 projMat;
    projMat.perspective(45.0f, this->aspectRatio, 0.1f, 100.0f); // view: 45 degree
    QVector3D center = this->camera_pos - this->camera_direction * 5.0f;
    projMat.lookAt(this->camera_pos, center, QVector3D(0.f, 1.f, 0.f)); // look from (0,0,3) at (0,0,0), with (0, 1, 0) as the up direction
    QMatrix4x4 viewMat;
    QMatrix4x4 modelMat;
    m_shader->setUniformValue(m_shader->uniformLocation("projectMat"), projMat);
    m_shader->setUniformValue(m_shader->uniformLocation("viewMat"), viewMat);
    m_shader->setUniformValue(m_shader->uniformLocation("modelMat"), modelMat);
    m_shader->setUniformValue(m_shader->uniformLocation("lookFrom"), visLookFrom);
    m_shader->setUniformValue(m_shader->uniformLocation("lookAt"), visLookAt);
    m_shader->setUniformValue(m_shader->uniformLocation("lookUpVec"), visLookUp);
    QVector3D dataSize(volumnData.getMetaData().size[0], volumnData.getMetaData().size[1], volumnData.getMetaData().size[2]);
    m_shader->setUniformValue(m_shader->uniformLocation("dataSize"), dataSize);
    m_shader->setUniformValue(m_shader->uniformLocation("diffuseK"), 0.9f);
    m_shader->setUniformValue(m_shader->uniformLocation("lightPos"), lightPos);
    m_shader->setUniformValue(m_shader->uniformLocation("lightColor"), this->lightColor);
    m_shader->setUniformValue(m_shader->uniformLocation("interpolationType"), this->interpolationType);
    m_shader->setUniformValue(m_shader->uniformLocation("alphaThreshold"), this->alphaThreshold);
    // draw object
    f->glDrawArrays(GL_TRIANGLES, 0, 6);
    // release
    m_shader->release();
    m_vao->release();
    //update(); // requrest to schedule an update
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    // 2 degree rotation
    const float SIN_2_DEG = 0.03489949670250097;
    const float COS_2_DEG = 0.9993908270190958;
    float new_y, new_z;
    switch(event->key()) {
        case Qt::Key::Key_Up:
            setViewAngel(viewAlpha, viewTheta + M_PI / 90);
            new_z = visLookUp.z() * COS_2_DEG - visLookUp.y() * SIN_2_DEG;
            new_y = visLookUp.z() * SIN_2_DEG + visLookUp.y() * COS_2_DEG;
            visLookUp.setZ(new_z);
            visLookUp.setY(new_y);
            break;
        case Qt::Key::Key_Down:
            setViewAngel(viewAlpha, viewTheta - M_PI / 90);
            new_z = visLookUp.z() * COS_2_DEG + visLookUp.y() * SIN_2_DEG;
            new_y = -visLookUp.z() * SIN_2_DEG + visLookUp.y() * COS_2_DEG;
            visLookUp.setZ(new_z);
            visLookUp.setY(new_y);
            break;

        case Qt::Key::Key_Right:
            // rotate the vis viewpoint 2 degree anti-clockwise with respect to look-at point
            setViewAngel(viewAlpha + M_PI / 90, viewTheta);
            break;
        case Qt::Key::Key_Left:
            setViewAngel(viewAlpha - M_PI / 90, viewTheta);
            break;
    }
    emit stateChanged();
    update();
}
