
#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QThread>
#include <atomic>

#include "src/resources/granularities.h"
#include "src/plotWidget/plotWidget.h"
#include "src/plotWidget/chartTopMenu.h"
#include "src/plotWidget/studiesWidget.h"
#include "src/tradingWidget/orderWidget.h"
#include "src/historicalData.h"
#include "src/streamdata.h"
#include "src/indicators/trend.h"
#include "src/utilities.h"

class ChartWidget : public QWidget {
  Q_OBJECT

  public:
    ChartWidget(QWidget *parent = 0, const QString instrument = "USD_SEK", 
      const QString granularity = "5M", OrderWidget *orderWidget = nullptr);
    ~ChartWidget();
    // Adds a Simple Moving Average line plot into the graph.
    void addSmaIndicator(const int period);
    void addOverlayIndicator(const QString name, const int period);
    //HistoricalData historicalData;
    FinancialDataContainer dataContainer;

  private:
    void createLayout();
    void addIndicator(QString name);
    void initHistoricalData(const QString instrument);
    // Initializes and starts a datastream with given instrument.
    void initDataStream(const QString instrument);
    // Requests interruption of the datastream in the thread and subsequent quit it.
    // This is needed e.g., when we changing instrument, and thus need to stop and start
    // a new data stream for the new instrument.
    void stopDataStream();
    // Initializes all connections related to the chart, mainly to all the dropdowns in the chartTopMenu.
    void initConnections();
    void initStyle();
    PlotWidget *chart;
    OrderWidget *orderWidget;
    ChartTopMenu *chartTopMenu;
    StudiesWidget *studies;
    QString instrument;
    int historicalResolutionCount;
    int oldStreamResolutionCount;
    QString currentChartType = "Candlestick";
    // Atomic boolean keeping track of whether we need to update/append full candle data,
    // or just a single close price. "Line" and "Area" only need to update the close price.
    std::atomic<bool> candleChartType = true;
    // Always good to keep track of what forex pair we currently have.
    QString forexPair = "";
    QString granularity = "";
    int chartResolution = 300;
    // Keeping track of which chart we currently highlighting. This is used so we can
    // switch what data is being send to the orderWidget.
    std::atomic<bool> selectedChart = true; 

    // Handle data stream
    // When calling this function, startDataStream in StreamDataWorker is called,
    // and streaming is starting.
    StreamDataWorker *streamDataWorker;
    HistoricalDataWorker *historicalDataWorker;
    QThread *streamDataWorkerThread;
    QThread *historicalDataWorkerThread = new QThread();

  public slots:
    // Receive a StreamData object from StreamDataWorker thread.
    void receiveStreamData(StreamData streamData);
    // Receive FinancialDataContainer object containing historical data.
    // Use this in order to initiate a new graph so we can start it in a separate thread.
    void receiveHistoricalData(FinancialDataContainer container);
    void chartTypeChanged(const QString chartType);
    // Slot called when changing forex pair in a chart. 
    void forexPairChanged(QString instrument);
    void resolutionChanged(QString granularity);
    void studiesButtonPressed();
    //void onNewOverlayGraph();
    
  signals:
    void startDataStream(const QString instrument, const int chartResolution);
    void startHistoricalDataRetrieval(const QString instrument, const QString granularity);
};


#endif