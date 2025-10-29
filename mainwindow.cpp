#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setUpUI();
}

void MainWindow::setUpUI()
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    auto *videoLayout = new QHBoxLayout();
    auto *buttonLayout = new QHBoxLayout();
    resize(1000, 800);

    originalLabel = new QLabel("");
    processedLabel = new QLabel("");
    originalLabel->setMinimumSize(500, 350);
    processedLabel->setMinimumSize(500, 350);
    originalLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    processedLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    videoLayout->addWidget(originalLabel);
    videoLayout->addWidget(processedLabel);

    startBtn = new QPushButton("Старт");
    pauseBtn = new QPushButton("Пауза");
    stopBtn = new QPushButton("Стоп");
    resetBtn = new QPushButton("Сброс графика");
    motionCheck = new QCheckBox("Отображать движение");
    motionCheck->setChecked(true);
    sourceBox = new QComboBox();

    sourceBox->addItem("Из файла", 0);
    sourceBox->addItem("Камера", 1);

    threadSlider = new QSlider(Qt::Horizontal);
    threadSlider->setRange(1, QThreadPool::globalInstance()->maxThreadCount());
    threadSlider->setValue(QThreadPool::globalInstance()->maxThreadCount());
    threadLabel = new QLabel(QString("Потоков: %1").arg(threadSlider->value()));

    buttonLayout->addWidget(startBtn);
    buttonLayout->addWidget(pauseBtn);
    buttonLayout->addWidget(stopBtn);
    buttonLayout->addWidget(resetBtn);
    buttonLayout->addWidget(sourceBox);
    buttonLayout->addWidget(motionCheck);
    buttonLayout->addWidget(threadLabel);
    buttonLayout->addWidget(threadSlider);


    //Графики
    layout->addLayout(videoLayout);
    layout->addLayout(buttonLayout);
    setCentralWidget(central);

    fpsSeries = new QLineSeries();
    motionSeries = new QLineSeries();
    fpsSeries->setName("FPS");
    motionSeries->setName("Motion Intensity");

    QChart *chart = new QChart();
    chart->addSeries(fpsSeries);
    chart->addSeries(motionSeries);
    chart->createDefaultAxes();
    chart->setTitle("Показатели видео");

    chartView = new QChartView(chart);
    chartView->setMinimumHeight(300);

    layout->addWidget(chartView);
    setCentralWidget(central);


    connect(startBtn, &QPushButton::clicked, this, &MainWindow::start_video);
    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::stop_video);
    connect(pauseBtn, &QPushButton::clicked, this, &MainWindow::pause_video);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::reset_charts);
    connect(motionCheck, &QCheckBox::toggled, this, &MainWindow::toggle_motion_view);
    connect(sourceBox, &QComboBox::currentIndexChanged, this, &MainWindow::source_changed);
    connect(threadSlider, &QSlider::valueChanged, this, [this](int val)
    {
        threadLabel->setText(QString("Потоков: %1").arg(val));
        QThreadPool::globalInstance()->setMaxThreadCount(val);
    });
    connect(&frameTimer, &QTimer::timeout, this, &MainWindow::update_frame);
    connect(&chartTimer, &QTimer::timeout, this, &MainWindow::update_charts);

    chartTimer.start(500);
}

void MainWindow::start_video()
{
    if (!video.open("/Users/dmitry/Desktop/portfolio/videoEditor/Resources/test_video.mp4"))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть видео");
        return;
    }
    isRunning = true;
    isPaused = false;
    prev_frame.release();
    startBtn->setEnabled(false);
    stopBtn->setEnabled(true);
    frameTimer.start(33);
}

void MainWindow::stop_video()
{
    isRunning = false;
    isPaused = true;
    video.release();
    startBtn->setEnabled(true);
    stopBtn->setEnabled(false);

}

void MainWindow::source_changed(int i)
{
    switch (i)
    {
    case 0:

    }

}

void MainWindow::pause_video()
{
    isPaused.store(!isPaused.load());
    pauseBtn->setText(isPaused ? "Продолжить" : "Пауза");

}

void MainWindow::reset_charts()
{
    fpsSeries->clear();
    motionSeries->clear();
    fps_history.clear();
    motion_history.clear();

}

void MainWindow::toggle_motion_view(bool enabled)
{
    showMotion = enabled;
}

void MainWindow::update_frame()
{
    if (!isRunning.load() || isPaused.load()) return;

    cv::Mat frame;
    auto start = std::chrono::high_resolution_clock::now();

    if (!video.read(frame))
    {
        stop_video();
        return;
    }
    originalLabel->setPixmap(QPixmap::fromImage(matToImage(frame)).scaled(originalLabel->size(), Qt::KeepAspectRatio ));
    QtConcurrent::run([this, frame] {process_frame(frame);});

    auto end = std::chrono::high_resolution_clock::now();
    double fps = 1000 / std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    this->fps_history.push_back(fps);
    if (fps_history.size() > 600) fps_history.pop_front();


}

void MainWindow::process_frame(const cv::Mat &mat)
{
    cv::Mat gray, diff, thresh;
    cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray,cv::Size(5,5), 0);

    double motionLevel = 0.0;
    cv::Mat frameCopy = mat.clone();

    if (!prev_frame.empty())
    {
        cv::absdiff(gray, prev_frame, diff);
        cv::threshold(diff, thresh, 30, 255, cv::THRESH_BINARY);
        cv::erode(thresh, thresh, cv::Mat(), cv::Point(-1, -1), 2);
        cv::dilate(thresh, thresh, cv::Mat(), cv::Point(-1, -1), 2);

        motionLevel = cv::sum(thresh)[0] / (mat.cols * mat.rows * 255.0);

        if (showMotion && motionLevel > 0.03)
        {
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            for (const auto &contour : contours)
            {
                if (cv::contourArea(contour) < 500) continue;
                cv::Rect rect = cv::boundingRect(contour);
                cv::rectangle(frameCopy, rect, cv::Scalar(0, 255, 0), 2);
            }
        }
        QMetaObject::invokeMethod(this, [this, frameCopy]()
        {
            processedLabel->setPixmap(QPixmap::fromImage(matToImage(frameCopy).scaled(processedLabel->size(), Qt::KeepAspectRatio)));
        });
    }
    prev_frame = gray;
    motion_history.push_back(motionLevel);
    if (motion_history.size() > 500) motion_history.pop_front();

}


void MainWindow::update_charts()
{

}


QImage MainWindow::matToImage(const cv::Mat &mat)
{
    cv::Mat rgb;
    if (mat.channels() == 3)
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
    else if (mat.channels() == 1)
        cv::cvtColor(mat, rgb, cv::COLOR_GRAY2RGB);
    else return QImage();
    return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();

}

MainWindow::~MainWindow()
{

}

