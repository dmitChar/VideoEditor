#include "filemodewidget.h"

FileModeWidget::FileModeWidget(QWidget *parent)
    : QWidget{parent}
{
    setUpUI();
}


void FileModeWidget::setUpUI()
{
    auto *buttonLayout = new QHBoxLayout();

    QPushButton *startBtn = new QPushButton("Старт");
    QPushButton *stopBtn = new QPushButton("Стоп");
    QPushButton *pauseBtn = new QPushButton("Пауза");
    QPushButton *chooseFileButton = new QPushButton("Выбрать файл");

    buttonLayout->addWidget(startBtn);
    buttonLayout->addWidget(pauseBtn);
    buttonLayout->addWidget(stopBtn);
    buttonLayout->addWidget(chooseFileButton);

    startBtn->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    stopBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    pauseBtn->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    chooseFileButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    setLayout(buttonLayout);

    connect(startBtn, &QPushButton::clicked, this, &FileModeWidget::startRequested);
    connect(stopBtn, &QPushButton::clicked, this, &FileModeWidget::stopRequested);
    connect(pauseBtn, &QPushButton::clicked, this, &FileModeWidget::pauseRequested);
    connect(chooseFileButton, &QPushButton::clicked, this, &FileModeWidget::onFileSelectedBtnClicked);
}

void FileModeWidget::onFileSelectedBtnClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Выберите видео", QDir::homePath());
    if (!path.isEmpty())
        emit fileSelected(path);
}
