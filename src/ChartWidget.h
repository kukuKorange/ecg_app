#pragma once
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QLabel>
#include "VitalSignData.h"

class ChartWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChartWidget(QWidget* parent = nullptr);
    ~ChartWidget();

    // 添加实时ECG波形数据点
    void addECGPoint(double value);
    
    // 添加实时趋势数据
    void addTrendPoint(const VitalSignData& data);
    
    // 加载历史数据显示趋势图
    void loadHistoryData(const QVector<VitalSignData>& historyData);
    
    // 清除所有数据
    void clearData();
    
    // 设置显示模式
    enum DisplayMode {
        RealTimeECG,     // 实时心电图波形
        TemperatureTrend, // 体温趋势图
        HeartRateTrend,   // 心率趋势图
        OxygenTrend       // 血氧趋势图
    };
    void setDisplayMode(DisplayMode mode);
    
    // 设置Y轴范围
    void setYAxisRange(double min, double max);
    
    // 设置X轴时间范围（秒）
    void setTimeRange(int seconds);

signals:
    void chartClicked(const QPointF& point);

private:
    QChartView* m_chartView;
    QChart* m_chart;
    
    // ECG实时波形
    QLineSeries* m_ecgSeries;
    QValueAxis* m_ecgAxisX;
    QValueAxis* m_ecgAxisY;
    
    // 趋势图数据
    QSplineSeries* m_temperatureSeries;
    QSplineSeries* m_heartRateSeries;
    QSplineSeries* m_oxygenSeries;
    QDateTimeAxis* m_trendAxisX;
    QValueAxis* m_trendAxisY;
    
    DisplayMode m_currentMode;
    int m_maxDataPoints;
    int m_currentDataCount;
    
    // 初始化图表
    void initializeChart();
    void setupECGChart();
    void setupTrendChart();
    
    // 更新显示
    void updateChart();
};

// ECG波形显示组件
class ECGWaveformWidget : public QWidget {
    Q_OBJECT

public:
    explicit ECGWaveformWidget(QWidget* parent = nullptr);
    
    // 添加ECG数据
    void addECGData(const QVector<double>& ecgSignal);
    
    // 设置显示速度 (mm/s)
    void setSweepSpeed(double speed);
    
    // 设置增益
    void setGain(double gain);
    
    // 启动/停止显示
    void start();
    void stop();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVector<double> m_ecgBuffer;
    double m_sweepSpeed;
    double m_gain;
    bool m_isRunning;
    int m_currentPosition;
    
    static constexpr int BUFFER_SIZE = 5000;
};

// 多参数显示面板
class VitalSignPanel : public QWidget {
    Q_OBJECT

public:
    explicit VitalSignPanel(QWidget* parent = nullptr);
    
    // 更新显示值
    void updateVitalSigns(const VitalSignData& data);
    
    // 显示报警
    void showAlarm(const AlarmInfo& alarm);

private:
    QLabel* m_temperatureLabel;
    QLabel* m_heartRateLabel;
    QLabel* m_oxygenLabel;
    QLabel* m_alarmLabel;
    
    void setupUI();
    void updateAlarmStyle(int severity);
};
