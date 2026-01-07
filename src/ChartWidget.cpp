#include "ChartWidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QDebug>

// ==================== ChartWidget ====================

ChartWidget::ChartWidget(QWidget* parent)
    : QWidget(parent)
    , m_chart(nullptr)
    , m_chartView(nullptr)
    , m_currentMode(RealTimeECG)
    , m_maxDataPoints(1000)
    , m_currentDataCount(0)
{
    qDebug() << "ChartWidget: Constructor - Start";
    
    try {
        qDebug() << "ChartWidget: Creating QChart...";
        m_chart = new QChart();
        qDebug() << "ChartWidget: QChart created successfully";
        
        qDebug() << "ChartWidget: Creating QChartView...";
        m_chartView = new QChartView(m_chart, this);
        qDebug() << "ChartWidget: QChartView created successfully";
        
        qDebug() << "ChartWidget: Calling initializeChart()...";
        initializeChart();
        qDebug() << "ChartWidget: initializeChart() completed";
        
        qDebug() << "ChartWidget: Creating layout...";
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(m_chartView);
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
        qDebug() << "ChartWidget: Constructor completed successfully";
    } catch (const std::exception& e) {
        qCritical() << "ChartWidget: Exception in constructor:" << e.what();
        throw;
    } catch (...) {
        qCritical() << "ChartWidget: Unknown exception in constructor";
        throw;
    }
}

ChartWidget::~ChartWidget() {
}

void ChartWidget::initializeChart() {
    qDebug() << "ChartWidget::initializeChart - Start";
    
    qDebug() << "ChartWidget::initializeChart - Setting chart title...";
    m_chart->setTitle("实时心电图");
    m_chart->setAnimationOptions(QChart::NoAnimation);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    
    qDebug() << "ChartWidget::initializeChart - Creating ECG series...";
    // 初始化ECG波形
    m_ecgSeries = new QLineSeries();
    m_ecgSeries->setName("ECG");
    
    qDebug() << "ChartWidget::initializeChart - Creating ECG axes...";
    m_ecgAxisX = new QValueAxis();
    m_ecgAxisX->setTitleText("时间 (s)");
    m_ecgAxisX->setRange(0, 10);
    
    m_ecgAxisY = new QValueAxis();
    m_ecgAxisY->setTitleText("幅值 (mV)");
    m_ecgAxisY->setRange(-2, 2);
    
    qDebug() << "ChartWidget::initializeChart - Creating trend series...";
    // 初始化趋势图序列
    m_temperatureSeries = new QSplineSeries();
    m_temperatureSeries->setName("体温");
    
    m_heartRateSeries = new QSplineSeries();
    m_heartRateSeries->setName("心率");
    
    m_oxygenSeries = new QSplineSeries();
    m_oxygenSeries->setName("血氧");
    
    qDebug() << "ChartWidget::initializeChart - Creating trend axes...";
    m_trendAxisX = new QDateTimeAxis();
    m_trendAxisX->setFormat("hh:mm:ss");
    m_trendAxisX->setTitleText("时间");
    
    m_trendAxisY = new QValueAxis();
    
    qDebug() << "ChartWidget::initializeChart - Calling setupECGChart...";
    setupECGChart();
    qDebug() << "ChartWidget::initializeChart - Complete";
}

void ChartWidget::setupECGChart() {
    qDebug() << "ChartWidget::setupECGChart - Start";
    
    qDebug() << "ChartWidget::setupECGChart - Removing all series...";
    m_chart->removeAllSeries();
    
    qDebug() << "ChartWidget::setupECGChart - Adding ECG series...";
    m_chart->addSeries(m_ecgSeries);
    
    qDebug() << "ChartWidget::setupECGChart - Setting axes...";
    m_chart->setAxisX(m_ecgAxisX, m_ecgSeries);
    m_chart->setAxisY(m_ecgAxisY, m_ecgSeries);
    
    qDebug() << "ChartWidget::setupECGChart - Setting title...";
    m_chart->setTitle("实时心电图波形");
    qDebug() << "ChartWidget::setupECGChart - Complete";
}

void ChartWidget::setupTrendChart() {
    m_chart->removeAllSeries();
    
    QAbstractSeries* series = nullptr;
    QString title;
    
    switch (m_currentMode) {
        case TemperatureTrend:
            series = m_temperatureSeries;
            title = "体温趋势";
            m_trendAxisY->setRange(35, 42);
            m_trendAxisY->setTitleText("温度 (°C)");
            break;
        case HeartRateTrend:
            series = m_heartRateSeries;
            title = "心率趋势";
            m_trendAxisY->setRange(40, 180);
            m_trendAxisY->setTitleText("心率 (bpm)");
            break;
        case OxygenTrend:
            series = m_oxygenSeries;
            title = "血氧趋势";
            m_trendAxisY->setRange(85, 100);
            m_trendAxisY->setTitleText("血氧 (%)");
            break;
        default:
            return;
    }
    
    m_chart->addSeries(series);
    m_chart->setAxisX(m_trendAxisX, series);
    m_chart->setAxisY(m_trendAxisY, series);
    m_chart->setTitle(title);
}

void ChartWidget::addECGPoint(double value) {
    if (m_currentMode != RealTimeECG) return;
    
    double timePoint = m_currentDataCount * 0.01; // 假设100Hz采样率
    m_ecgSeries->append(timePoint, value);
    
    // 限制数据点数量
    if (m_ecgSeries->count() > m_maxDataPoints) {
        m_ecgSeries->remove(0);
        
        // 更新X轴范围实现滚动效果
        double minX = m_ecgSeries->at(0).x();
        double maxX = m_ecgSeries->at(m_ecgSeries->count() - 1).x();
        m_ecgAxisX->setRange(minX, maxX);
    }
    
    m_currentDataCount++;
}

void ChartWidget::addTrendPoint(const VitalSignData& data) {
    qint64 timestamp = data.timestamp.toMSecsSinceEpoch();
    
    m_temperatureSeries->append(timestamp, data.temperature);
    m_heartRateSeries->append(timestamp, data.heartRate);
    m_oxygenSeries->append(timestamp, data.oxygenSaturation);
    
    // 限制数据点
    if (m_temperatureSeries->count() > m_maxDataPoints) {
        m_temperatureSeries->remove(0);
        m_heartRateSeries->remove(0);
        m_oxygenSeries->remove(0);
    }
    
    // 更新时间轴范围
    if (m_temperatureSeries->count() > 0) {
        qint64 minTime = m_temperatureSeries->at(0).x();
        qint64 maxTime = m_temperatureSeries->at(m_temperatureSeries->count() - 1).x();
        m_trendAxisX->setRange(QDateTime::fromMSecsSinceEpoch(minTime),
                               QDateTime::fromMSecsSinceEpoch(maxTime));
    }
}

void ChartWidget::loadHistoryData(const QVector<VitalSignData>& historyData) {
    clearData();
    
    for (const auto& data : historyData) {
        addTrendPoint(data);
    }
}

void ChartWidget::clearData() {
    m_ecgSeries->clear();
    m_temperatureSeries->clear();
    m_heartRateSeries->clear();
    m_oxygenSeries->clear();
    m_currentDataCount = 0;
}

void ChartWidget::setDisplayMode(DisplayMode mode) {
    m_currentMode = mode;
    
    if (mode == RealTimeECG) {
        setupECGChart();
    } else {
        setupTrendChart();
    }
}

void ChartWidget::setYAxisRange(double min, double max) {
    if (m_currentMode == RealTimeECG) {
        m_ecgAxisY->setRange(min, max);
    } else {
        m_trendAxisY->setRange(min, max);
    }
}

void ChartWidget::setTimeRange(int seconds) {
    if (m_currentMode == RealTimeECG) {
        double currentMax = m_ecgAxisX->max();
        m_ecgAxisX->setRange(currentMax - seconds, currentMax);
    }
}

void ChartWidget::updateChart() {
    m_chart->update();
}

// ==================== ECGWaveformWidget ====================

ECGWaveformWidget::ECGWaveformWidget(QWidget* parent)
    : QWidget(parent)
    , m_sweepSpeed(25.0)
    , m_gain(10.0)
    , m_isRunning(false)
    , m_currentPosition(0)
{
    setMinimumSize(800, 300);
    m_ecgBuffer.reserve(BUFFER_SIZE);
}

void ECGWaveformWidget::addECGData(const QVector<double>& ecgSignal) {
    for (double value : ecgSignal) {
        m_ecgBuffer.append(value);
        if (m_ecgBuffer.size() > BUFFER_SIZE) {
            m_ecgBuffer.removeFirst();
        }
    }
    
    if (m_isRunning) {
        update();
    }
}

void ECGWaveformWidget::setSweepSpeed(double speed) {
    m_sweepSpeed = speed;
}

void ECGWaveformWidget::setGain(double gain) {
    m_gain = gain;
}

void ECGWaveformWidget::start() {
    m_isRunning = true;
    update();
}

void ECGWaveformWidget::stop() {
    m_isRunning = false;
}

void ECGWaveformWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制背景网格
    painter.setPen(QPen(Qt::lightGray, 0.5));
    int gridSize = 20;
    for (int x = 0; x < width(); x += gridSize) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += gridSize) {
        painter.drawLine(0, y, width(), y);
    }
    
    // 绘制ECG波形
    if (m_ecgBuffer.isEmpty()) return;
    
    painter.setPen(QPen(Qt::green, 2));
    
    int centerY = height() / 2;
    double scaleX = width() / static_cast<double>(qMin(m_ecgBuffer.size(), width()));
    
    for (int i = 1; i < m_ecgBuffer.size() && i < width(); ++i) {
        double y1 = centerY - m_ecgBuffer[i - 1] * m_gain;
        double y2 = centerY - m_ecgBuffer[i] * m_gain;
        painter.drawLine(QPointF((i - 1) * scaleX, y1), QPointF(i * scaleX, y2));
    }
}

// ==================== VitalSignPanel ====================

VitalSignPanel::VitalSignPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void VitalSignPanel::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    m_temperatureLabel = new QLabel("体温: --", this);
    m_heartRateLabel = new QLabel("心率: --", this);
    m_oxygenLabel = new QLabel("血氧: --", this);
    m_alarmLabel = new QLabel("", this);
    
    QFont font("Arial", 24, QFont::Bold);
    m_temperatureLabel->setFont(font);
    m_heartRateLabel->setFont(font);
    m_oxygenLabel->setFont(font);
    
    QFont alarmFont("Arial", 16, QFont::Bold);
    m_alarmLabel->setFont(alarmFont);
    m_alarmLabel->setStyleSheet("QLabel { color: red; }");
    m_alarmLabel->setAlignment(Qt::AlignCenter);
    
    mainLayout->addWidget(m_temperatureLabel);
    mainLayout->addWidget(m_heartRateLabel);
    mainLayout->addWidget(m_oxygenLabel);
    mainLayout->addWidget(m_alarmLabel);
    mainLayout->addStretch();
    
    setLayout(mainLayout);
}

void VitalSignPanel::updateVitalSigns(const VitalSignData& data) {
    m_temperatureLabel->setText(QString("体温: %1 °C").arg(data.temperature, 0, 'f', 1));
    m_heartRateLabel->setText(QString("心率: %1 bpm").arg(data.heartRate));
    m_oxygenLabel->setText(QString("血氧: %1 %").arg(data.oxygenSaturation));
    
    // 根据数值设置颜色警告
    QString tempColor = (data.temperature < 36.0 || data.temperature > 38.0) ? "red" : "black";
    QString hrColor = (data.heartRate < 60 || data.heartRate > 100) ? "red" : "black";
    QString oxColor = (data.oxygenSaturation < 95) ? "red" : "black";
    
    m_temperatureLabel->setStyleSheet(QString("QLabel { color: %1; }").arg(tempColor));
    m_heartRateLabel->setStyleSheet(QString("QLabel { color: %1; }").arg(hrColor));
    m_oxygenLabel->setStyleSheet(QString("QLabel { color: %1; }").arg(oxColor));
}

void VitalSignPanel::showAlarm(const AlarmInfo& alarm) {
    m_alarmLabel->setText("⚠️ " + alarm.message);
    updateAlarmStyle(alarm.severity);
    
    // 可以添加闪烁效果或声音提示
}

void VitalSignPanel::updateAlarmStyle(int severity) {
    QString color;
    switch (severity) {
        case 1:
        case 2:
            color = "orange";
            break;
        case 3:
        case 4:
            color = "red";
            break;
        case 5:
            color = "darkred";
            break;
        default:
            color = "yellow";
    }
    
    m_alarmLabel->setStyleSheet(QString("QLabel { color: %1; font-weight: bold; }").arg(color));
}
