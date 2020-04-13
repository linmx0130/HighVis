#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWindow>
#include <QLayout>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QSlider>
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

private:
    QVBoxLayout *layout, *sidebarLayout;
    QWidget *sidebar;
    QHBoxLayout *hlayout;
    GLWidget *glWidget;
    QPlainTextEdit *logLabel;
    QComboBox *interpolationComboBox;
    QPushButton *lightColorPushButton;
    QSlider *thresholdSlider;
    QColor lightColor;
    void buildSidebar();
};
#endif // MAINWINDOW_H
