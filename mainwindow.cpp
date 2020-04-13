#include "mainwindow.h"
#include <QVBoxLayout>
#include "volumndata.h"
#include <QLabel>
#include <QColorDialog>
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
    sidebarLayout->addStretch();
    sidebar = new QWidget(this);
    sidebar->setLayout(sidebarLayout);
    sidebar->setMaximumSize(200, 600);
}
MainWindow::~MainWindow()
{

}

void MainWindow::glWidgetStateChange()
{
    QVector3D camera_pos = glWidget->getCameraPos();
    QString log;
    log = log.asprintf("Camera Pos = (%f, %f, %f).\n", camera_pos.x(), camera_pos.y(), camera_pos.z());
    QVector3D camera_dir = glWidget->getCameraDirection();
    log.append(log.asprintf("Camera Dir = (%f, %f, %f).\n", camera_dir.x(), camera_dir.y(), camera_dir.z()));
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

