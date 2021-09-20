#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QThread>
#include <nlohmann/json.hpp>

#include "account.h"
#include "historicalData.h"
#include "streamdata.h"
#include "plotWidget/plotWidget.h"
#include "plotWidget/chartWidget.h"
#include "fxWidget/fxWidget.h"
#include "tradingWidget/orderWidget.h"

using json = nlohmann::json;

class MainWindow : public QMainWindow{
  Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);

  private:
    void createLayout();
    void initStyle();
  	Account account;
    ChartWidget *chartWidget;
    QWidget *mainWidget;
    FxWidget *fxWidget;
    OrderWidget *orderWidget;

    // Handle data stream
    // When calling this function, startDataStream in StreamDataWorker is called,
    // and streaming is starting.
    StreamDataWorker *streamDataWorker;
    QThread streamDataWorkerThread;

  signals:
    void startDataStream();
};


#endif