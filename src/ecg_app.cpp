#include "ecg_app.h"

ecg_app::ecg_app(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui_ecg_app)
{
    ui->setupUi(this);
}

ecg_app::~ecg_app()
{
    delete ui; 
}