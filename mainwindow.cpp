#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setUpUI();
    setUpProcessingThread();
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

    sourceBox->addItem("Камера", 0);
    sourceBox->addItem("Из файла", 1);


    fileWidget = new FileModeWidget();
    camWidget = new CameraModeWidget();
    stack = new QStackedWidget();
    stack->addWidget(camWidget);
    stack->addWidget(fileWidget);
    stack->setMinimumHeight(50);

    helpButtonsLayout->addWidget(resetBtn);
    helpButtonsLayout->addWidget(sourceBox);
    helpButtonsLayout->addWidget(motionCheck);


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

    connect(&frameTimer, &QTimer::timeout, this, &MainWindow::update_frame);
    connect(&chartTimer, &QTimer::timeout, this, &MainWindow::update_charts);

    connect(camWidget, &CameraModeWidget::startCamera, this, &MainWindow::start_camera_processing);
    connect(camWidget, &CameraModeWidget::stopCamera, this, &MainWindow::stop_video);
    connect(fileWidget, &FileModeWidget::startRequested, this, &MainWindow::start_video);
    connect(fileWidget, &FileModeWidget::stopRequested, this, &MainWindow::stop_video);
    connect(fileWidget, &FileModeWidget::fileSelected, this, &MainWindow::start_video_processing);
    connect(fileWidget, &FileModeWidget::pauseRequested, this, &MainWindow::pause_video);


    start_camera_processing();
    chartTimer.start(500);
}

void MainWindow::setUpProcessingThread()
{
    processingThread = new QThread();
    processor = new VideoProcessor();
    processor->moveToThread(processingThread);

    //Отправка кадра на обработку
    connect(this, &MainWindow::frameReady, processor, &VideoProcessor::processFrame);

    //Получение кадра обратно
    connect(processor, &VideoProcessor::frameProcessed, this, &MainWindow::updateProcessedFrame);

    //Обновление флага showMotion в потоке
    connect(this, &MainWindow::motionCheckToggled, processor, &VideoProcessor::setShowMotion);

    //Очистка потока
    connect(processingThread, &QThread::finished, processor, &VideoProcessor::deleteLater);

    processingThread->start();
}

void MainWindow::toggle_motion_view(bool enabled)
{
    //отправляем сигнал в рабочий поток
    emit motionCheckToggled(enabled);
}

void MainWindow::switchMode(int index)
{
    currentSourceMode = (index == 0) ? SourceType::File : SourceType::Camera;
    stack->setCurrentIndex(index);
    stop_video();
    this->originalLabel->setPixmap(QPixmap());
    this->processedLabel->setPixmap(QPixmap());
}

void MainWindow::start_video()
{
    isRunning = true;
    isPaused = false;
    //prev_frame.release();
    frameTimer.start(33);
}


void MainWindow::start_video_processing(const QString &path)
{
    if (currentSourceMode == SourceType::File)
    {
        source = FactoryUI::create(SourceType::File);
    }

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
        source->open(QString());
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

    originalLabel->setPixmap(QPixmap::fromImage(CVUtils::matToImage(frame)).scaled(originalLabel->size(), Qt::KeepAspectRatio ));

    //Отправка кадра в рабочий поток для обработки
    emit frameReady(frame.clone());

    auto end = std::chrono::high_resolution_clock::now();
    double fps = 1000 / std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    this->fps_history.push_back(fps);
    if (fps_history.size() > 600) fps_history.pop_front();
}

void MainWindow::updateProcessedFrame(const QImage &image, double motionLevel)
{
    processedLabel->setPixmap(QPixmap::fromImage(image).scaled(processedLabel->size(), Qt::KeepAspectRatio));
}

void MainWindow::update_charts()
{

}


MainWindow::~MainWindow()
{
    processingThread->quit();
    processingThread->wait();

}

