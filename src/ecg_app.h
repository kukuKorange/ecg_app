#pragma once
#include "ui_ecg_app.h"
#include <QMainWindow>

class ecg_app : public QMainWindow {
    Q_OBJECT
    
public:
    ecg_app(QWidget* parent = nullptr);
    ~ecg_app();

private:
    Ui_ecg_app* ui;
};