

#include <memory>
#include "bbWidget.h"

BbWidget::BbWidget(QWidget *parent, PlotWidget *chart, QTreeWidgetItem *treeItem) : QWidget(parent) {
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
  infoText -> setText("BOLLINGER BANDS");
  layout -> addWidget(infoText);
  layout -> addWidget(new HSeparator(this));

  customPeriodWidget = new CustomInputWidget(this, "Period");
  customPeriodWidget -> setInputText(QString::number(period));
  connect(customPeriodWidget -> input, &QLineEdit::returnPressed, this, &BbWidget::periodChanged);
  periodLayout -> addWidget(customPeriodWidget);

  customDeviationWidget = new CustomInputWidget(this, "Deviations", "double");
  customDeviationWidget -> setInputText(QString::number(stddev));
  connect(customDeviationWidget -> input, &QLineEdit::returnPressed, this, &BbWidget::stddevChanged);
  periodLayout -> addWidget(customDeviationWidget);

  customSourceWidget = new CustomSourceWidget(this);
  connect(customSourceWidget -> combo, &QComboBox::currentTextChanged, this, &BbWidget::sourceChanged);
  periodLayout -> addWidget(customSourceWidget);

  customColorWidget = new CustomColorWidget(this, "Line color");
  connect(customColorWidget -> colorDialog, &QColorDialog::currentColorChanged, this, &BbWidget::colorChanged);
  periodLayout -> addWidget(customColorWidget);
  periodLayout -> addStretch();

  layout -> addLayout(periodLayout);
  layout -> addWidget(new HSeparator(this));
  layout -> addStretch();
  setLayout(layout);

  customLegendItem = new CustomLegendItem(chart, "BB", QString::number(period));
  chart -> addLegendItem(customLegendItem);

  chart -> replotChart();
}

BbWidget::~BbWidget() {
  chart -> removeLegendItem(customLegendItem);
}

void BbWidget::periodChanged() {
  period = customPeriodWidget -> getInputInt();
  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData(); 
  replaceData(data);
  treeItem -> setText(0, getActiveName());
  customLegendItem -> setPeriod(QString::number(period));
  chart -> replotChart();
}

void BbWidget::stddevChanged() {
  stddev = customDeviationWidget -> getInputInt();
  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData(); 
  replaceData(data);
  chart -> replotChart();
}

void BbWidget::colorChanged(const QColor &color) {
  graph -> setPen(QPen(color));
  graph_upper -> setPen(QPen(color));
  graph_lower -> setPen(QPen(color));
  QColor brushColor = color;
  brushColor.setAlpha(5);
  graph_lower -> setBrush(brushColor);
  chart -> replotChart();
}

void BbWidget::sourceChanged(const QString &text) {
  this -> source = text;
  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData();
  QVector<double> rawData = getRawData(data, source);
  auto [bb_upper, bb_middle, bb_lower] = bb(rawData, period, stddev);
  setGraphData(graph, bb_middle);
  setGraphData(graph_upper, bb_upper);
  setGraphData(graph_lower, bb_lower);
  customLegendItem -> setValue(QString::number(bb_middle.last()));
  chart -> replotChart();
}

void BbWidget::initStudy() {
  this -> graph = chart -> newGraph();
  graphs.append(graph);
  this -> graph_upper = chart -> newGraph();
  graphs.append(graph_upper);
  this -> graph_lower = chart -> newGraph();
  graphs.append(graph_lower);
  QColor color = customColorWidget -> getCurrentColor();
  this -> graph_lower -> setChannelFillGraph(this -> graph_upper);
  graph -> setPen(QPen(color));
  graph_upper -> setPen(QPen(color));
  graph_lower -> setPen(QPen(color));
  color.setAlpha(5);
  this -> graph_lower -> setBrush(color);

  QCPDataContainer<QCPFinancialData> *data = chart -> getPointerData();
  QVector<double> rawData = getRawData(data, source);
  auto [bb_upper, bb_middle, bb_lower] = bb(rawData, period, stddev);
  setGraphData(graph, bb_middle);
  setGraphData(graph_upper, bb_upper);
  setGraphData(graph_lower, bb_lower);
  customLegendItem -> setValue(QString::number(bb_lower.last()));
  chart -> replotChart();
}

void BbWidget::updateData(QCPDataContainer<QCPFinancialData> *data, const bool newPoint) {
  QVector<double> rawData = getRawData(data, source);
  auto [bb_upper, bb_middle, bb_lower] = bb(rawData, period, stddev);
  const double upper_new = bb_upper.last();
  const double middle_new = bb_middle.last();
  const double lower_new = bb_lower.last();
  updateGraphData(this -> graph, middle_new, newPoint);
  updateGraphData(this -> graph_upper, upper_new, newPoint);
  updateGraphData(this -> graph_lower, lower_new, newPoint);
  customLegendItem -> setValue(QString::number(middle_new));
  chart -> replotChart();
}

void BbWidget::replaceData(QCPDataContainer<QCPFinancialData> *data) {
  QVector<double> rawData = getRawData(data, source);
  auto [bb_upper, bb_middle, bb_lower] = bb(rawData, period, stddev);
  setGraphData(this -> graph, bb_middle);
  setGraphData(this -> graph_upper, bb_upper);
  setGraphData(this -> graph_lower, bb_lower);
  customLegendItem -> setValue(QString::number(bb_middle.last()));
  chart -> replotChart();
}

const QString BbWidget::getActiveName() {
  return QString("BB") + QString(" (") + QString(QString::number(period)) + QString(")");
}