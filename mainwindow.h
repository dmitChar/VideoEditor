#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QTimer>
#include <QtConcurrent>
#include <QFileDialog>
#include <QThreadPool>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <opencv2/opencv.hpp>
#include <atomic>
#include <deque>
#include <QMessageBox>
#include <QStackedWidget>

#include "factoryui.h"
#include "cameramodewidget.h"
#include "filemodewidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void start_video_processing(const QString &path);
    void start_camera_processing();
    void stop_video();
    void pause_video();
    void reset_charts();
    void update_frame();
    void update_charts();
    void toggle_motion_view(bool);
    void switchMode(int);
    void start_video();

private:
    QLabel *originalLabel;
    QLabel *processedLabel;
    QPushButton *startBtn;
    QPushButton *stopBtn;
    QPushButton *pauseBtn;
    QPushButton *resetBtn;
    QSlider *threadSlider;
    QLabel *threadLabel;
    QCheckBox *motionCheck;
    QComboBox *sourceBox;

    QChartView *chartView;
    QLineSeries *fpsSeries;
    QLineSeries *motionSeries;

    //cv::VideoCapture video;
    QTimer frameTimer;
    QTimer chartTimer;

    std::atomic<bool> isRunning = false;
    std::atomic<bool> isPaused = false;
    std::atomic<bool> showMotion = true;
    cv::Mat prev_frame;
    std::deque<double> fps_history;
    std::deque<double> motion_history;

    SourceType currentSourceMode = SourceType::File;
    std::unique_ptr<AbstractUI> source;
    QStackedWidget *stack;
    CameraModeWidget *camWidget;
    FileModeWidget *fileWidget;

    void process_frame(const cv::Mat &frame);
    QImage matToImage(const cv::Mat &mat);
    void setUpUI();


};
#endif // MAINWINDOW_H
