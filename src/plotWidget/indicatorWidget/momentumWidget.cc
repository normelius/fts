
#include <memory>
#include "momentumWidget.h"

/*
MacdWidget::MacdWidget(QWidget *parent, ActiveWidget *activeWidget) : QWidget(parent) {
  hide();
  this -> activeWidget = activeWidget;

  QVBoxLayout *layout = new QVBoxLayout();
  infoText = new QLabel(this);
  infoText -> setWordWrap(true);
  infoText -> setText("Moving Average Convergence Divergence");
  layout -> addWidget(infoText);
  layout -> addWidget(new HSeparator(this));

  QHBoxLayout *rowLayout = new QHBoxLayout();

  QVBoxLayout *fastPeriodLayout = new QVBoxLayout();
  fastPeriodInput = new QLineEdit(this);
  fastPeriodLayout -> addWidget(new QLabel("Fast Period"));
  fastPeriodLayout -> addWidget(fastPeriodInput);
  rowLayout -> addLayout(fastPeriodLayout);

  layout -> addLayout(rowLayout);
  layout -> addWidget(new HSeparator(this));
  button = new QPushButton("Add");
  connect(button, &QPushButton::clicked, this, &MacdWidget::addNewStudy);
  layout -> addWidget(button, 1, Qt::AlignRight);
  layout -> addStretch();
  this -> setLayout(layout);
}

void MacdWidget::addNewStudy() {
  CloseButton *closeButton = new CloseButton("X", this);
  QTreeWidgetItem *rootItem = new QTreeWidgetItem(activeWidget -> tree, QStringList(QString("MACD")));
  activeWidget -> tree -> setItemWidget(rootItem, 2, closeButton);
  // Save reference to the rootItem so we can delete it later.
  closeButton -> rootItem = rootItem;

  // Input for fastPeriod.
  QTreeWidgetItem *fastPeriodItem = new QTreeWidgetItem(rootItem, QStringList(QString("Fast period")));
  QLineEdit *periodEdit = new QLineEdit(fastPeriodInput -> text(), this);
  activeWidget -> tree -> setItemWidget(fastPeriodItem, 1, periodEdit);

  // Hook up the close button in to the active widget in order to remove a rootItem.
  connect(closeButton, &CloseButton::removeStudy, activeWidget, &ActiveWidget::onRemovedStudy);

  MacdIndicator *indicator = new MacdIndicator();
  indicator -> item = rootItem;
  //indicator -> fastPeriod = (fastPeriodInput -> text()).toInt();

  // Hook up connections with the inputs to the indicator.
  //connect(periodEdit, &QLineEdit::textEdited, indicator, &MacdIndicator::fastPeriodChanged);

  activeWidget -> onAddedStudy<MacdIndicator*>(indicator, false);
}




void MacdIndicator::fastPeriodChanged(QString text) {
  //qDebug() << text;
  //this -> fastPeriod = text.toInt();
}

void MacdIndicator::newSubplot(SubPlot *subplot, QCPDataContainer<QCPFinancialData> *data) {
  QVector<double> close = getCloseData(data);
  auto [values, _signals, histogram] = macd(close, slowPeriod, fastPeriod, signalSmoothing);
  auto [posHistogram, negHistogram] = getHistograms(histogram);
  this -> posMacdHistogram = subplot -> initNewBars(posHistogram, "#51A39A");
  this -> negMacdHistogram = subplot -> initNewBars(negHistogram, "#DE5E56");
  this -> macdGraph = subplot -> initNewGraph(values);
  // Subplot has connections that rescales this graph accordingly to the main graph.
  subplot -> graphToRescale = macdGraph;
  this -> macdSignalGraph = subplot -> initNewGraph(_signals, "#E16628");
}

std::tuple<QVector<double>, QVector<double>> MacdIndicator::getHistograms(QVector<double> histogram) {
  // Some lambda expressions for replacing negative/positive values in the copied vectors
  // This will be used in order to only get positive/negative values in the vectors.
  QVector<double> pos = histogram;
  QVector<double> neg = histogram;
  auto isNegative = [](double value) {return true ? value < 0.0 : false;};
  auto isPositive = [](double value) {return true ? value > 0.0 : false;};
  std::replace_if(pos.begin(), pos.end(), isNegative, 0.0);
  std::replace_if(neg.begin(), neg.end(), isPositive, 0.0);
  return std::tuple(pos, neg);
}


void MacdIndicator::updateData(QCPDataContainer<QCPFinancialData> *data, const bool newPoint) {
  QVector<double> close = getCloseData(data);
  auto [macdValue, macdSignal, macdHistogram] = macd(close, slowPeriod, fastPeriod, signalSmoothing);

  double latestMacdHistogramValue = macdHistogram.last();

  if (newPoint == true) {
    this -> macdGraph -> addData(this -> macdGraph -> dataCount(), macdValue.last());
    this -> macdSignalGraph -> addData(this -> macdSignalGraph -> dataCount(), macdSignal.last());

    // Add a negative bar. Make sure to also add 0.0 to the other since they should be the same length.
    if (latestMacdHistogramValue < 0.0) {
      this -> negMacdHistogram -> addData(this -> negMacdHistogram -> dataCount(), latestMacdHistogramValue);
      this -> posMacdHistogram -> addData(this -> posMacdHistogram -> dataCount(), 0.0);
    }

    // Add a new positive bar.
    else {
      this -> negMacdHistogram -> addData(this -> negMacdHistogram -> dataCount(), 0.0);
      this -> posMacdHistogram -> addData(this -> posMacdHistogram -> dataCount(), latestMacdHistogramValue);
    }
  }

  else {
    QCPDataContainer<QCPGraphData> *containerMacd = (macdGraph -> data()).data(); 
    QCPDataContainer<QCPGraphData> *containerSignal = (macdSignalGraph -> data()).data(); 
    auto itMacd = containerMacd -> end() - 1;
    auto itSignal = containerSignal -> end() - 1;
    itMacd -> value = macdValue.last();
    itSignal -> value = macdSignal.last();
    
    // Update the histogram data by replacing the last histogram bar.
    QCPDataContainer<QCPBarsData> *containerPosHist = (posMacdHistogram -> data()).data(); 
    QCPDataContainer<QCPBarsData> *containerNegHist = (negMacdHistogram -> data()).data(); 
    if (latestMacdHistogramValue < 0.0) {
      (containerNegHist -> end() - 1) -> value = latestMacdHistogramValue;
    }

    else {
      (containerPosHist -> end() - 1) -> value = latestMacdHistogramValue;
    }
  }
}

void MacdIndicator::replaceData(QCPDataContainer<QCPFinancialData> *data) {
  QVector<double> close = getCloseData(data);
  auto [values, _signals, histogram] = macd(close, slowPeriod, fastPeriod, signalSmoothing);
  auto [posHistogram, negHistogram] = getHistograms(histogram);
  QVector<double> axis(data -> size());
  std::iota(std::begin(axis), std::end(axis), 0);
  this -> macdGraph -> setData(axis, values);
  this -> macdSignalGraph -> setData(axis, _signals);
  this -> posMacdHistogram -> setData(axis, posHistogram);
  this -> negMacdHistogram -> setData(axis, negHistogram);
}

*/
