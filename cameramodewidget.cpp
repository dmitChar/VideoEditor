#include "cameramodewidget.h"

CameraModeWidget::CameraModeWidget(QWidget *parent)
    : QWidget{parent}
{
    setUpUI();
}


void CameraModeWidget::setUpUI()
{
    auto *buttonLayout = new QHBoxLayout();

    QPushButton *startBtn = new QPushButton("Старт");
    QPushButton *stopBtn = new QPushButton("Стоп");

    buttonLayout->addWidget(startBtn);
    buttonLayout->addWidget(stopBtn);

    startBtn->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    stopBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setLayout(buttonLayout);

    connect(startBtn, &QPushButton::clicked, this, &CameraModeWidget::startCamera);
    connect(stopBtn, &QPushButton::clicked, this, &CameraModeWidget::stopCamera);

}
