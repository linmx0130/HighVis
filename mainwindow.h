#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWindow>
#include <QLayout>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include "glwidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(const char* filename, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void glWidgetStateChange();
    void onInterpolationChanged(const QString& text);
    void onLightColorPushButtonClicked();
    void onThresholdSliderChanged(int v);
    void onLightPositionChanged();
    void onSaveImagePushButtonClicked();
private:
    QVBoxLayout *layout, *sidebarLayout;
    QWidget *sidebar;
    QHBoxLayout *hlayout;
    GLWidget *glWidget;
    QPlainTextEdit *logLabel;
    QComboBox *interpolationComboBox;
    QPushButton *lightColorPushButton, *saveImagePushButton;
    QSlider *thresholdSlider;
    QColor lightColor;
    QDoubleSpinBox *lightX, *lightY, *lightZ;
    void buildSidebar();
};
#endif // MAINWINDOW_H
