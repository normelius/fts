
#include <QFile>

#include "chartWidget.h"
#include <chrono>


using std::chrono::high_resolution_clock;
using std::chrono::duration;

ChartWidget::ChartWidget(QWidget *parent, const QString instrument, 
  const QString granularity, OrderWidget *orderWidget) : QWidget(parent) {
  setObjectName("chartWidget");
  chart = new PlotWidget(parent);
  chartTopMenu = new ChartTopMenu(parent);
  studies = new StudiesWidget(this, chart);
  this -> orderWidget = orderWidget;
  this -> instrument = instrument;
  this -> granularity = granularity;
  this -> chartResolution = getResolutionFromGranularity(granularity);
	chart -> createChart();
  chart -> setChartResolution(granularity);
  chart -> setWatermark(instrument);
  initConnections();
  createLayout();
  initStyle();
  initHistoricalData(instrument);
}

  // Need to cleanly stop the stream thread when exiting the application.
ChartWidget::~ChartWidget() {
	streamDataWorkerThread -> requestInterruption();
	streamDataWorkerThread -> quit();
  if (!streamDataWorkerThread -> wait(300)) {
    streamDataWorkerThread -> terminate();
    streamDataWorkerThread -> wait();
  };

  historicalDataWorkerThread -> quit();
  historicalDataWorkerThread -> wait();
}

void ChartWidget::initStyle() {
  QFile file("src/stylesheets/chartWidget.qss");
  file.open(QFile::ReadOnly);
  QString stylesheet = QLatin1String(file.readAll());
  setStyleSheet(stylesheet);
}

void ChartWidget::createLayout() {
  QVBoxLayout *layout = new QVBoxLayout();
  layout -> setContentsMargins(0, 0, 0, 0);
  layout -> setSpacing(0);
	layout -> addWidget(chartTopMenu);
	layout -> addWidget(chart);
  this -> setLayout(layout);
}

void ChartWidget::initConnections() {
  connect(chartTopMenu -> chartTypeButton, SIGNAL(currentTextChanged(QString)), this, SLOT(chartTypeChanged(QString)));
  connect(chartTopMenu -> forexPairButton, SIGNAL(currentTextChanged(QString)), this, SLOT(forexPairChanged(QString)));
  connect(chartTopMenu -> resolutionButton, SIGNAL(currentTextChanged(QString)), this, SLOT(resolutionChanged(QString)));
  connect(chartTopMenu -> studiesButton, SIGNAL(pressed()), this, SLOT(studiesButtonPressed()));
}

void ChartWidget::initHistoricalData(const QString instrument) {
  historicalDataWorker = new HistoricalDataWorker();
	historicalDataWorker -> moveToThread(historicalDataWorkerThread);
	connect(historicalDataWorkerThread, &QThread::finished, historicalDataWorker, &QObject::deleteLater);
  connect(this, &ChartWidget::startHistoricalDataRetrieval, historicalDataWorker, &HistoricalDataWorker::startDataRetrieval);
  qRegisterMetaType<FinancialDataContainer>("FinancialDataContainer");
	connect(historicalDataWorker, &HistoricalDataWorker::emitHistoricalData, this, &ChartWidget::receiveHistoricalData);
  historicalDataWorkerThread -> start();
  startHistoricalDataRetrieval(instrument, this -> granularity);
}

void ChartWidget::receiveHistoricalData(FinancialDataContainer container) {
  // What resolution count do we have when fetching the historical data when starting the application.
  //historicalResolutionCount = container.getValue<int>("resolutionCount", container.size() - 1);
  historicalResolutionCount = container.getResolutionCount();
  oldStreamResolutionCount = historicalResolutionCount;
  chart -> setNewData(container);
  initDataStream(this -> instrument);

  // When we have received historical data we should update the active indicators.
  // This is needed in case we e.g., change the resolution.
  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData();  
  studies -> replaceStudies(data);

  chart -> replotChart();
}

void ChartWidget::initDataStream(const QString instrument) {
  streamDataWorker = new StreamDataWorker();
  streamDataWorkerThread = new QThread();
	streamDataWorker -> moveToThread(streamDataWorkerThread);
	connect(streamDataWorkerThread, &QThread::finished, streamDataWorker, &QObject::deleteLater);
  connect(this, &ChartWidget::startDataStream, streamDataWorker, &StreamDataWorker::startDataStream);
	qRegisterMetaType<StreamData>("StreamData");
	connect(streamDataWorker, &StreamDataWorker::emitStreamData, this, &ChartWidget::receiveStreamData);
	streamDataWorkerThread -> start();
	startDataStream(instrument, chartResolution);
}

void ChartWidget::stopDataStream() {
  streamDataWorkerThread -> requestInterruption();
	streamDataWorkerThread -> quit();
  if (!streamDataWorkerThread -> wait(300)) {
    streamDataWorkerThread -> terminate();
    streamDataWorkerThread -> wait();
  };

  historicalDataWorkerThread -> quit();
  historicalDataWorkerThread -> wait();
  // We need a new worker thread when starting new data stream since we cannot 
  // undo requestInterruption which is needed when stopping a data stream.
  delete streamDataWorkerThread;
}

void ChartWidget::receiveStreamData(StreamData streamData) {
  // When receiving new data, the first thing we want to do is to update the orderWidget.
  orderWidget -> newData(streamData);

  // If we are on a timeframe and only need to update the latest candlestick with a new closing price.
  if (historicalResolutionCount == streamData.resolutionCount || oldStreamResolutionCount == streamData.resolutionCount) {
    if (candleChartType == true)
      chart -> updateLatestCandle(streamData.closePrice);
    
    // Only needs to update "Line" and "Area" data.
    else
      chart -> updateLatestDataPoint(streamData.closePrice);

    // Update all the studies with new prices.
    // We receive the original prices from the chart so we now everything is in sync.
    QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData(); 
    studies -> updateStudies(data, false);
  }

  // If a new timeframe has started and we need a new data point.
  else {
    if (candleChartType == true)
      chart -> addNewCandle(streamData.closePrice);

    else
      chart -> addNewDataPoint(streamData.closePrice);

    // Update all the studies with a new datapoint.
    // We receive the original prices from the chart so we now everything is in sync.
    QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData(); 
    studies -> updateStudies(data, true);
  }

  oldStreamResolutionCount = streamData.resolutionCount;
  chart -> replotChart();
}

void ChartWidget::chartTypeChanged(const QString chartType) {
  if (currentChartType != chartType) {

    if (currentChartType == "Line" || currentChartType == "Area") {
      chart -> removeLineChart();
    }

    if (chartType == "Line") {
      candleChartType = false;
      chart -> showChart(false);
      chart -> setLineChart();
    }

    else if (chartType == "Area") {
      candleChartType = false;
      chart -> showChart(false);
      chart -> setLineChart(true);
    }

    else if (chartType == "Candlestick" || chartType == "OHLC") {
      candleChartType = true;
      chart -> showChart(true);
      chart -> setChartType(chartType);
    }

    currentChartType = chartType;
  }
}

void ChartWidget::forexPairChanged(QString instrument) {
  this -> instrument = instrument.replace("/", "_");
  // Need to stop current data stream in order to create a new one.
  this -> stopDataStream();
  chartTypeChanged("Candlestick");
  chart -> setWatermark(instrument);
  initHistoricalData(instrument);
}

void ChartWidget::resolutionChanged(QString granularity) {
  this -> granularity = granularity;
  this -> chartResolution = getResolutionFromGranularity(granularity);
  chart -> setChartResolution(granularity);
  this -> stopDataStream();
  chartTypeChanged("Candlestick");
  initHistoricalData(instrument);
}

void ChartWidget::studiesButtonPressed() {
  studies -> show();
}


/*
void ChartWidget::onNewOverlayGraph(QTreeWidgetItem *) {
  qDebug() << "Adding new overlay graph";
  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData();
}
*/





// REDO
/*
void ChartWidget::addOverlayIndicator(const QString name, const int period) {
  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData();
  QVector<double> keys(chart -> getDataCount());
  QVector<double> indicatorData;

  if (name == "SMA") {
    indicatorData = getSMA(data, period);
  }

  QCPGraph *graph = chart -> addGraph();
  graph -> setName(name);
  graph -> setPen(QPen(QColor(40, 110, 255)));
  graph -> setKeyAxis(chart -> xAxis);
  graph -> setValueAxis(chart -> yAxis2);
  graph -> setData(keys, indicatorData);
}

void ChartWidget::addSmaIndicator(const int period) {
  QVector<double> sma(chart -> getDataCount(), std::numeric_limits<double>::quiet_NaN());
  QVector<double> keys(chart -> getDataCount());

  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData();
  double temp = 0.0;
  int idx = 0;
  for (QCPFinancialDataContainer::const_iterator it = data -> begin(); it != data -> end(); ++it) {
    temp += it -> close;
    keys[idx] = idx;

    if (idx >= period) {
      temp -= data -> at(idx - period) -> close;
    }

    if (idx >= (period - 1)) {
      sma[idx] = (double)temp / period;
    }

    idx++;
  }

  QCPGraph *graph = chart -> addGraph();
  graph -> setPen(QPen(QColor(40, 110, 255)));
  graph -> setKeyAxis(chart -> xAxis);
  graph -> setValueAxis(chart -> yAxis2);
  graph -> setData(keys, sma);
}
*/