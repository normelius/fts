

#include <memory>
#include "emaWidget.h"

EmaWidget::EmaWidget(QWidget *parent, PlotWidget *chart, QTreeWidgetItem *treeItem) : QWidget(parent) {
  hide();
  this -> chart = chart;
  this -> treeItem = treeItem;
  this -> setMaximumWidth(450);
  treeItem -> setText(0, getActiveName());
  QVBoxLayout *layout = new QVBoxLayout();
  QHBoxLayout *periodLayout = new QHBoxLayout();
  periodLayout -> setContentsMargins(0, 0, 0, 0);
  periodLayout -> setSpacing(10);

  QLabel *infoText = new QLabel(this);
  infoText -> setWordWrap(true);
  infoText -> setText("An exponential moving average (EMA) is a type of moving average (MA)"
    "that places a greater weight and significance on the most recent data points."
    "The exponential moving average is also referred to as the exponentially weighted"
    "moving average. An exponentially weighted moving average reacts more significantly"
    "to recent price changes than a simple moving average (SMA), which applies an equal"
    "weight to all observations in the period.");
  layout -> addWidget(infoText);
  layout -> addWidget(new HSeparator(this));

  customPeriodWidget = new CustomInputWidget(this, "Period");
  customPeriodWidget -> setInputText(QString::number(period));
  connect(customPeriodWidget -> input, &QLineEdit::returnPressed, this, &EmaWidget::periodChanged);
  periodLayout -> addWidget(customPeriodWidget);

  customSourceWidget = new CustomSourceWidget(this);
  connect(customSourceWidget -> combo, &QComboBox::currentTextChanged, this, &EmaWidget::sourceChanged);
  periodLayout -> addWidget(customSourceWidget);

  customColorWidget = new CustomColorWidget(this, "Line color");
  connect(customColorWidget -> colorDialog, &QColorDialog::currentColorChanged, this, &EmaWidget::colorChanged);
  periodLayout -> addWidget(customColorWidget);
  periodLayout -> addStretch();

  layout -> addLayout(periodLayout);
  layout -> addWidget(new HSeparator(this));
  layout -> addStretch();
  setLayout(layout);

  customLegendItem = new CustomLegendItem(chart, "EMA", QString::number(period));
  chart -> addLegendItem(customLegendItem);

  chart -> replotChart();
}

EmaWidget::~EmaWidget() {
  chart -> removeLegendItem(customLegendItem);
}

void EmaWidget::periodChanged() {
  period = customPeriodWidget -> getInputInt();
  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData(); 
  replaceData(data);
  treeItem -> setText(0, getActiveName());
  customLegendItem -> setPeriod(QString::number(period));
  chart -> replotChart();
}

void EmaWidget::colorChanged(const QColor &color) {
  graph -> setPen(QPen(color));
  chart -> replotChart();
}

void EmaWidget::sourceChanged(const QString &text) {
  this -> source = text;
  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData();
  QVector<double> rawData = getRawData(data, source);
  QVector<double> newData = ema(rawData, this -> period);
  setGraphData(graph, newData);
  chart -> replotChart();
}

void EmaWidget::initStudy() {
  this -> graph = chart -> newGraph();
  graphs.append(graph);
  graph -> setPen(QPen(customColorWidget -> getCurrentColor()));
  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData();
  QVector<double> close = getRawData(data, source);
  QVector<double> newData = ema(close, this -> period);
  setGraphData(graph, newData);
  customLegendItem -> setValue(QString::number(newData.last()));
  chart -> replotChart();
}

void EmaWidget::updateData(QCPDataContainer<QCPFinancialData> *data, const bool newPoint) {
  QVector<double> rawData = getRawData(data, source);
  // Want to redo this in order to facilitate streaming api.
  // Don't want to calculate whole vector again just because of one value.
  QVector<double> emaData = ema(rawData, this -> period);
  const double newData = emaData.last();
  updateGraphData(this -> graph, newData, newPoint);
  customLegendItem -> setValue(QString::number(newData));
  chart -> replotChart();
}

void EmaWidget::replaceData(QCPDataContainer<QCPFinancialData> *data) {
  QVector<double> rawData = getRawData(data, source);
  QVector<double> newData = ema(rawData, this -> period);
  setGraphData(this -> graph, newData);
  customLegendItem -> setValue(QString::number(newData.last()));
  chart -> replotChart();
}

const QString EmaWidget::getActiveName() {
  return QString("EMA") + QString(" (") + QString(QString::number(period)) + QString(")");
}