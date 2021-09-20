

#include <memory>
#include "smaWidget.h"

SmaWidget::SmaWidget(QWidget *parent, PlotWidget *chart, QTreeWidgetItem *treeItem) : QWidget(parent) {
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
  infoText -> setText("A simple moving average is formed by computing the average price of a security over a specific number of periods");
  layout -> addWidget(infoText);
  layout -> addWidget(new HSeparator(this));

  customPeriodWidget = new CustomInputWidget(this, "Period");
  customPeriodWidget -> setInputText(QString::number(period));
  connect(customPeriodWidget -> input, &QLineEdit::returnPressed, this, &SmaWidget::periodChanged);
  periodLayout -> addWidget(customPeriodWidget);

  customSourceWidget = new CustomSourceWidget(this);
  connect(customSourceWidget -> combo, &QComboBox::currentTextChanged, this, &SmaWidget::sourceChanged);
  periodLayout -> addWidget(customSourceWidget);

  customColorWidget = new CustomColorWidget(this, "Line color");
  connect(customColorWidget -> colorDialog, &QColorDialog::currentColorChanged, this, &SmaWidget::colorChanged);
  periodLayout -> addWidget(customColorWidget);
  periodLayout -> addStretch();

  layout -> addLayout(periodLayout);
  layout -> addWidget(new HSeparator(this));
  layout -> addStretch();
  setLayout(layout);

  customLegendItem = new CustomLegendItem(chart, "SMA", QString::number(period));
  chart -> addLegendItem(customLegendItem);

  chart -> replotChart();
}

SmaWidget::~SmaWidget() {
  chart -> removeLegendItem(customLegendItem);
}

void SmaWidget::periodChanged() {
  period = customPeriodWidget -> getInputInt();
  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData(); 
  replaceData(data);
  treeItem -> setText(0, getActiveName());
  customLegendItem -> setPeriod(QString::number(period));
  chart -> replotChart();
}

void SmaWidget::colorChanged(const QColor &color) {
  graph -> setPen(QPen(color));
  chart -> replotChart();
}

void SmaWidget::sourceChanged(const QString &text) {
  this -> source = text;
  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData();
  QVector<double> rawData = getRawData(data, source);
  QVector<double> newData = sma(rawData, this -> period);
  setGraphData(graph, newData);
  chart -> replotChart();
}

void SmaWidget::initStudy() {
  this -> graph = chart -> newGraph();
  graphs.append(graph);
  graph -> setPen(QPen(customColorWidget -> getCurrentColor()));
  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData();
  QVector<double> close = getRawData(data, source);
  QVector<double> newData = sma(close, this -> period);
  setGraphData(graph, newData);
  customLegendItem -> setValue(QString::number(newData.last()));
  chart -> replotChart();
}

void SmaWidget::updateData(QCPDataContainer<QCPFinancialData> *data, const bool newPoint) {
  QVector<double> close = getRawData(data, source);
  const double newData = nextSma(close, this -> period);
  updateGraphData(this -> graph, newData, newPoint);
  customLegendItem -> setValue(QString::number(newData));
  chart -> replotChart();
}

void SmaWidget::replaceData(QCPDataContainer<QCPFinancialData> *data) {
  QVector<double> close = getRawData(data, source);
  QVector<double> newData = sma(close, this -> period);
  setGraphData(this -> graph, newData);
  customLegendItem -> setValue(QString::number(newData.last()));
  chart -> replotChart();
}

const QString SmaWidget::getActiveName() {
  return QString("SMA") + QString(" (") + QString(QString::number(period)) + QString(")");
}