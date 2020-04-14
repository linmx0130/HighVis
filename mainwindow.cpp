#include "mainwindow.h"
#include <QVBoxLayout>
#include "volumndata.h"
#include <QLabel>
#include <QColorDialog>
#include <QMessageBox>

MainWindow::MainWindow(const char* filename, QWidget *parent)
    : QWidget(parent), layout(nullptr), lightColor(255, 255, 255)
{
    layout = new QVBoxLayout();
    hlayout = new QHBoxLayout();
    glWidget = new GLWidget(filename, this);
    glWidget->setMinimumHeight(100);
    logLabel = new QPlainTextEdit(this);
    hlayout->addWidget(glWidget);
    buildSidebar();
    hlayout->addWidget(sidebar);
    layout->addLayout(hlayout);
    layout->addWidget(logLabel);
    logLabel->setReadOnly(true);
    logLabel->setMaximumHeight(200);
    this->setLayout(layout);
    connect(glWidget, SIGNAL(stateChanged()), this, SLOT(glWidgetStateChange()));
    glWidgetStateChange();
}
void MainWindow::buildSidebar() {
    sidebarLayout = new QVBoxLayout();

    QLabel *interpolationLabel = new QLabel("Interpolation Method:");
    sidebarLayout->addWidget(interpolationLabel);
    interpolationComboBox = new QComboBox();
    interpolationComboBox->addItem("Tricubic", QVariant(0.0f));
    interpolationComboBox->addItem("Trilinear", QVariant(1.0f));
    sidebarLayout->addWidget(interpolationComboBox);
    connect(interpolationComboBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onInterpolationChanged(const QString &)));

    lightColorPushButton = new QPushButton("Light color");
    lightColorPushButton->setPalette(QPalette(this->lightColor));
    connect(lightColorPushButton, SIGNAL(clicked()), this, SLOT(onLightColorPushButtonClicked()));

    sidebarLayout->addWidget(lightColorPushButton);
    QLabel *sliderLabel = new QLabel("Threshold:");
    sidebarLayout->addWidget(sliderLabel);
    thresholdSlider = new QSlider(Qt::Orientation::Horizontal);
    thresholdSlider->setRange(0, 100);
    thresholdSlider->setValue(20);
    connect(thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(onThresholdSliderChanged(int)));
    sidebarLayout->addWidget(thresholdSlider);

    QLabel *lightPosLabel = new QLabel("Light position (X, Y, Z): ");
    sidebarLayout->addWidget(lightPosLabel);
    QHBoxLayout *lightPosLayout = new QHBoxLayout;
    lightX = new QDoubleSpinBox(this);
    lightY = new QDoubleSpinBox(this);
    lightZ = new QDoubleSpinBox(this);
    lightX->setRange(-3.0, 3.0);
    lightX->setSingleStep(0.1);
    lightX->setValue(0.5);
    lightY->setRange(-3.0, 3.0);
    lightY->setSingleStep(0.1);
    lightY->setValue(-2.0);
    lightZ->setRange(-3.0, 3.0);
    lightZ->setSingleStep(0.1);
    lightZ->setValue(0.5);
    lightPosLayout->addWidget(lightX);
    lightPosLayout->addWidget(lightY);
    lightPosLayout->addWidget(lightZ);
    sidebarLayout->addLayout(lightPosLayout);
    connect(lightX, SIGNAL(valueChanged(double)), this, SLOT(onLightPositionChanged()));
    connect(lightY, SIGNAL(valueChanged(double)), this, SLOT(onLightPositionChanged()));
    connect(lightZ, SIGNAL(valueChanged(double)), this, SLOT(onLightPositionChanged()));

    saveImagePushButton = new QPushButton("Save screen shot");
    connect(saveImagePushButton, SIGNAL(clicked()), this, SLOT(onSaveImagePushButtonClicked()));
    sidebarLayout->addWidget(saveImagePushButton);
    sidebarLayout->addStretch();
    sidebar = new QWidget(this);
    sidebar->setLayout(sidebarLayout);
    sidebar->setMaximumSize(240, 600);
}
MainWindow::~MainWindow()
{

}

void MainWindow::glWidgetStateChange()
{
    QVector3D camera_pos = glWidget->getCameraPos();
    QString log;
    log = log.asprintf("Camera Pos = (%f, %f, %f).\n", camera_pos.x(), camera_pos.y(), camera_pos.z());
    QVector3D lightPos = glWidget->getLightPos();
    log.append(log.asprintf("LightPos = (%f, %f, %f).\n", lightPos.x(), lightPos.y(), lightPos.z()));
    logLabel->setPlainText(log);
}

void MainWindow::onInterpolationChanged(const QString &text)
{
    if (text == "Tricubic") {
        glWidget->setInterpolationType(0.0f);
    }
    if (text == "Trilinear") {
        glWidget->setInterpolationType(1.0f);
    }
}

void MainWindow::onLightColorPushButtonClicked()
{
    QColor newColor = QColorDialog::getColor(this->lightColor, this);
    this->lightColor = newColor;
    lightColorPushButton->setPalette(QPalette(this->lightColor));
    glWidget->setLightColor(newColor);
}

void MainWindow::onThresholdSliderChanged(int v)
{
    glWidget->setAlphaThreshold((float)v / 100.0f);
}

void MainWindow::onLightPositionChanged()
{
    glWidget->setLightPos(lightX->value(), lightY->value(), lightZ->value());
}

void MainWindow::onSaveImagePushButtonClicked()
{
    QImage img = glWidget->grabFramebuffer();
    img.save("HighVisScreenShot.png");
    QMessageBox mbox(QMessageBox::Information, "HighVis",
                     "Saved the image to HighVisScreenShot.png");
    mbox.exec();
}

