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

    originalLabel = new QLabel("Оригинал");
    processedLabel = new QLabel("Обработано");
    originalLabel->setMinimumSize(320, 240);
    processedLabel->setMinimumSize(320, 240);
    videoLayout->addWidget(originalLabel);
    videoLayout->addWidget(processedLabel);

    startBtn = new QPushButton("Старт");
    pauseBtn = new QPushButton("Пауза");
    stopBtn = new QPushButton("Стоп");
    resetBtn = new QPushButton("Сброс графика");
    motionCheck = new QCheckBox("Отображать движение");
    motionCheck->setChecked(true);

    threadSlider = new QSlider(Qt::Horizontal);
    threadSlider->setRange(1, QThreadPool::globalInstance()->maxThreadCount());
    threadSlider->setValue(QThreadPool::globalInstance()->maxThreadCount());
    threadLabel = new QLabel(QString("Потоков: %1").arg(threadSlider->value()));

    buttonLayout->addWidget(startBtn);
    buttonLayout->addWidget(pauseBtn);
    buttonLayout->addWidget(stopBtn);
    buttonLayout->addWidget(resetBtn);
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
    if (!video.open("video.mp4"))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть видео");
        return;
    }
    isRunning = true;
    isPaused = false;
    prev_frame.release();
    startBtn->setEnabled(false);
    stopBtn->setEnabled(true);
    frameTimer.start(30);
}

void MainWindow::stop_video()
{
    isRunning = false;
    isPaused = true;
    video.release();
    startBtn->setEnabled(true);
    stopBtn->setEnabled(false);

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

