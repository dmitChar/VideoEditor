#ifndef FILEMODEWIDGET_H
#define FILEMODEWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>

class FileModeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileModeWidget(QWidget *parent = nullptr);

private:
    QPushButton *startBtn;
    QPushButton *stopBtn;
    QPushButton *pauseBtn;
    QPushButton *chooseFileButton;

    void setUpUI();
signals:
    void startRequested();
    void stopRequested();
    void pauseRequested();
    void fileSelected(const QString &path);

public slots:
    void onFileSelectedBtnClicked();
};

#endif // FILEMODEWIDGET_H
