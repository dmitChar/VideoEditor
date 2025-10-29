#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setUpUI();
}

void MainWindow::setUpUI()
{
    // layout'ы
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    auto *videoLayout = new QHBoxLayout();
    auto *buttonLayout = new QHBoxLayout();
    auto *helpButtonsLayout = new QHBoxLayout();
    resize(1000, 800);

    originalLabel = new QLabel("");
    processedLabel = new QLabel("");
    originalLabel->setMinimumSize(500, 350);
    processedLabel->setMinimumSize(500, 350);
    originalLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    processedLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    videoLayout->addWidget(originalLabel);
    videoLayout->addWidget(processedLabel);

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

    fileWidget = new FileModeWidget();
    camWidget = new CameraModeWidget();
    stack = new QStackedWidget();
    stack->addWidget(fileWidget);
    stack->addWidget(camWidget);
    stack->setMinimumHeight(50);

    helpButtonsLayout->addWidget(resetBtn);
    helpButtonsLayout->addWidget(sourceBox);
    helpButtonsLayout->addWidget(motionCheck);
    helpButtonsLayout->addWidget(threadLabel);
    helpButtonsLayout->addWidget(threadSlider);


    layout->addLayout(videoLayout);
    layout->addWidget(stack);
    layout->addLayout(buttonLayout);
    layout->addLayout(helpButtonsLayout);
    setCentralWidget(central);

    // --------Графики----------
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


    connect(sourceBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::switchMode);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::reset_charts);
    connect(motionCheck, &QCheckBox::toggled, this, &MainWindow::toggle_motion_view);
    connect(threadSlider, &QSlider::valueChanged, this, [this](int val)
    {
        threadLabel->setText(QString("Потоков: %1").arg(val));
        QThreadPool::globalInstance()->setMaxThreadCount(val);
    });
    connect(&frameTimer, &QTimer::timeout, this, &MainWindow::update_frame);
    connect(&chartTimer, &QTimer::timeout, this, &MainWindow::update_charts);

    connect(camWidget, &CameraModeWidget::startCamera, this, &MainWindow::start_camera_processing);
    connect(camWidget, &CameraModeWidget::stopCamera, this, &MainWindow::stop_video);
    connect(fileWidget, &FileModeWidget::startRequested, this, &MainWindow::start_video);
    connect(fileWidget, &FileModeWidget::stopRequested, this, &MainWindow::stop_video);
    connect(fileWidget, &FileModeWidget::fileSelected, this, &MainWindow::start_video_processing);
    connect(fileWidget, &FileModeWidget::pauseRequested, this, &MainWindow::pause_video);


    chartTimer.start(500);
}

void MainWindow::switchMode(int index)
{
    currentSourceMode = (index == 0) ? SourceType::File : SourceType::Camera;
    stack->setCurrentIndex(index);
    stop_video();
}

void MainWindow::start_video()
{
    isRunning = true;
    isPaused = false;
    prev_frame.release();
    frameTimer.start(33);
}


void MainWindow::start_video_processing(const QString &path)
{
    if (currentSourceMode == SourceType::File)
        source = FactoryUI::create(SourceType::File);

    if (!source->open(path))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть видеофайл");
    }
    start_video();
}

void MainWindow::start_camera_processing()
{
    if (currentSourceMode == SourceType::Camera)
    {
        source = FactoryUI::create(SourceType::Camera);
    }

    if (!source || !this->source->isOpened())
    {
        QMessageBox::warning(this, "Ошибка", "Источник не открыт");
        return;
    }

    start_video();
}

void MainWindow::stop_video()
{
    isRunning = false;
    isPaused = true;
}


void MainWindow::pause_video()
{
    isPaused.store(!isPaused.load());

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

    if (!source->read(frame))
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

