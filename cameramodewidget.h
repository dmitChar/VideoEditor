#ifndef CAMERAMODEWIDGET_H
#define CAMERAMODEWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>

class CameraModeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CameraModeWidget(QWidget *parent = nullptr);

signals:
    void startCamera();
    void stopCamera();

private:
    void setUpUI();
    QPushButton *startBtn;
    QPushButton *stopBtn;

};

#endif // CAMERAMODEWIDGET_H
