
/*
This is the main component for creating the financial chart.
*/

#include <iostream>
#include <QVector>
#include <QString>
#include <cmath>
#include <chrono>
#include <numeric>

#include "plotWidget.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

QString CustomDateAxis::getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision)
{
  Q_UNUSED(precision)
  Q_UNUSED(formatChar)

  // TODO: implement QMap instead for O(1) lookup. Mapping from 0, 1, 2 ... into real timestamps.
  // Need to know if we are getting tick values higher than our custom ticks.
  int size = customLabelTicks.size() - 1;
  int tickDiff = tick - size;

  if (tickDiff <= 0) {
    tick = customLabelTicks[tick];
  }

  else {
    tick = customLabelTicks[size] + (tickDiff * this -> chartResolution);
  }

  return locale.toString(keyToDateTime(tick).toTimeSpec(mDateTimeSpec), mDateTimeFormat);
}

void CustomDateAxis::setCustomTicksMapping(QVector<double> labelTicks) {
  this -> customLabelTicks = labelTicks;
}

PlotWidget::PlotWidget(QWidget *parent) : QCustomPlot(parent) {
  setObjectName("plotWidget");
  numberDataPoints = 0;
  horizontalViewOffset = 10;
  maxVisibleDataPoints = 1500;
  yAxis -> setVisible(false);
  yAxis2 -> setVisible(true);

  QBrush backgroundBrush = QBrush(QColor("#202226"));
  setBackground(backgroundBrush);

  setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

}

void PlotWidget::createChart() {
  // Use custom axis so set keys as 0, 1, 2, ...., thus binSize = 1.
  double binSize = 1;

  setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
  legend -> setVisible(false);

  chart = new QCPFinancial(xAxis, yAxis2);
  chart -> setChartStyle(QCPFinancial::csCandlestick);
  chart -> setWidth(binSize * 0.8);
  chart -> setTwoColored(true);
  chart -> setBrushPositive(QColor("#51A39A"));
  chart -> setBrushNegative(QColor("#DE5E56"));
  chart -> setPenPositive(QPen(QColor("#51A39A")));
  chart -> setPenNegative(QPen(QColor("#DE5E56")));

  yAxis2 -> setBasePen(QPen(Qt::NoPen));
  yAxis2 -> setTickPen(QPen(Qt::NoPen));
  yAxis2 -> setSubTickPen(QPen(Qt::NoPen));
  yAxis2 -> setTickLabelColor(QColor("#BCBCBC"));
  yAxis2 -> setNumberFormat("f");
  yAxis2 -> setNumberPrecision(5);
  QSharedPointer<QCPAxisTicker> customTicker = yAxis2 -> ticker();
  customTicker -> setTickCount(16);
  yAxis2 -> setTicker(customTicker);
  yAxis2 -> grid() -> setVisible(true);
  yAxis2 -> grid() -> setPen(QPen(QColor("#3A3B3B"), 0.4));

  // bring bottom and main axis rect closer together:
  plotLayout() -> setRowSpacing(0);

  crosshair = new Crosshair(xAxis, yAxis2);

  customLegend = new CustomLegend(this);
  // Start with good scale on the data.
  chart -> rescaleValueAxis(false, true, chartLowerScale, chartUpperScale);
  // Rescale volume bars in view. Some smart code here. PositiveVolume enlarge as much as possible,
  // even if the negativeVolume is greater. NegativeVolume only enlarges, and thus it will always work.
  //positiveVolume -> rescaleValueAxis(false, true, 1.0, 1.0);
  //negativeVolume -> rescaleValueAxis(true, true, 1.0, 8.0);
  replot();
}

void PlotWidget::setCustomAxis(QVector<double> time) {
  QSharedPointer<CustomDateAxis> customAxis(new CustomDateAxis);
  customAxis -> chartResolution = this -> chartResolution;
  customAxis -> setTickCount(10);
  customAxis -> setCustomTicksMapping(time);
  customAxis -> setDateTimeSpec(Qt::LocalTime);
  customAxis -> setDateTimeFormat("hh:mm");
  xAxis -> setTicker(customAxis);
  xAxis -> setBasePen(Qt::NoPen);
  xAxis -> setTickLabels(true);
  xAxis -> setTicks(true); // only want vertical grid in main axis rect, so hide xAxis backbone, ticks, and labels
  xAxis -> setTicker(customAxis);
  xAxis -> grid() -> setVisible(true);
  xAxis -> grid() -> setPen(QPen(QColor("#3A3B3B"), 0.4));
  xAxis -> setTickLabelColor(QColor("#BCBCBC"));
}

void PlotWidget::setNewData(FinancialDataContainer dataContainer) {
  this -> dataContainer = dataContainer;
  QVector<double> high = dataContainer.getHighVector();
  QVector<double> low = dataContainer.getLowVector();
  QVector<double> open = dataContainer.getOpenVector();
  QVector<double> close = dataContainer.getCloseVector();
  QVector<double> time = dataContainer.getTimeVector();
  QVector<double> keys;
  for (int idx = 0; idx < high.size(); ++idx) {
    keys.append(idx);
  }

  chart -> setData(keys, open, high, low, close, true);
  // We store how many data points we plot for reference.
  this -> numberDataPoints = close.size();
  setCustomAxis(time);
  xAxis -> setRange(numberDataPoints - 100, numberDataPoints + horizontalViewOffset);
}


void PlotWidget::showChart(bool visible) {
  chart -> setVisible(visible);
  replot();
}


int PlotWidget::getDataCount() {
  return chart -> dataCount();
}



QCPDataContainer<QCPFinancialData> *PlotWidget::getPointerData() {
  QSharedPointer<QCPFinancialDataContainer> data = chart -> data();
  QCPDataContainer<QCPFinancialData> *container = data.data();
  return container;
}


void PlotWidget::setChartType(const QString chartType) {
  if (chartType == "OHLC")
    chart -> setChartStyle(QCPFinancial::csOhlc);

  else if (chartType == "Candlestick")
    chart -> setChartStyle(QCPFinancial::csCandlestick);

  replot();
}


void PlotWidget::setLineChart(const bool areaChart) {
  lineChart = addGraph();
  QColor areaColor(40, 110, 255, 30);
  QColor lineColor(238, 238, 238);
  lineChart -> setPen(QPen(lineColor));
  if (areaChart == true)
    lineChart -> setBrush(QBrush(areaColor));

  lineChart -> setName("Line");
  lineChart -> setKeyAxis(xAxis);
  lineChart -> setValueAxis(yAxis2);

  QVector<double> closeArray(getDataCount());
  QVector<double> keys(getDataCount());
  QCPDataContainer<QCPFinancialData> *data = getPointerData();
  int idx = 0;
  for (QCPFinancialDataContainer::const_iterator it = data -> begin(); it != data -> end(); ++it) {
    keys[idx] = idx;
    closeArray[idx] = it -> close;
    ++idx;
  }

  lineChart -> setData(keys, closeArray);
  replotChart();
}


void PlotWidget::removeLineChart() {
  removeGraph(lineChart);
  replot();
}


void PlotWidget::addNewCandle(const double closePrice) {
  QSharedPointer<QCPFinancialDataContainer> data = chart -> data();
  QCPDataContainer<QCPFinancialData> *container = data.data();
  auto lastCandleIterator = container -> end() - 1;
  // We use the latest candlestick close price as the open price for the new timeframe.
  chart -> addData(getDataCount(), lastCandleIterator -> close, closePrice, closePrice, closePrice);
  //replotChart();
}


void PlotWidget::updateLatestCandle(const double closePrice) {
  QSharedPointer<QCPFinancialDataContainer> data = chart -> data();
  QCPDataContainer<QCPFinancialData> *container = data.data();
  auto lastCandleIterator = container -> end() - 1;
  lastCandleIterator -> close = closePrice;
  if (lastCandleIterator -> high < closePrice)
    lastCandleIterator -> high = closePrice;

  else if (lastCandleIterator -> low > closePrice)
    lastCandleIterator -> low = closePrice;

  //replotChart();
}


void PlotWidget::addNewDataPoint(const double closePrice) {
  lineChart -> addData(getDataCount(), closePrice);
  //replotChart();
}


void PlotWidget::updateLatestDataPoint(const double closePrice) {
  QSharedPointer<QCPDataContainer<QCPGraphData>> data = lineChart -> data();
  QCPDataContainer<QCPGraphData> *container = data.data();
  auto lastCandleIterator = container -> end() - 1;
  lastCandleIterator -> value = closePrice;
  //replotChart();
}


void PlotWidget::replotChart() {
  chart -> rescaleValueAxis(false, true, chartLowerScale, chartUpperScale);
  replot();
}

QCPGraph *PlotWidget::newGraph() {
  QCPGraph *graph = this -> addGraph();
  graph -> setKeyAxis(xAxis);
  graph -> setValueAxis(yAxis2);
  return graph;
}

void PlotWidget::wheelEvent(QWheelEvent *event) {
  // Data points in the view.
  int dataCount = chart -> keyAxis() -> range().size();

  if (dataCount <= maxVisibleDataPoints) {
    QCustomPlot::wheelEvent(event);
    double y = yAxis2 -> pixelToCoord(event -> position().y()); 
    crosshair -> setHorizontalPosition(y);
    chart -> rescaleValueAxis(false, true, chartLowerScale, chartUpperScale);

    if (dataCount < 200) {
      chart -> setWidth(0.5);
    }

    else if (dataCount >= 200 && dataCount < 400) {
      chart -> setWidth(0.4);
    }

    else if (dataCount >= 400 && dataCount < 600) {
      chart -> setWidth(0.3);
    }

    else if (dataCount >= 600 && dataCount < 800) {
      chart -> setWidth(0.2);
    }

    else {
      chart -> setWidth(0.1);
    }
  }

  // Max zoom, can only zoom in from here.
  bool zoomIn = (event -> angleDelta().y() > 0 ? true : false);
  if (dataCount > maxVisibleDataPoints && zoomIn == true)
    QCustomPlot::wheelEvent(event);
}

void PlotWidget::mousePressEvent(QMouseEvent *event) {
  QCustomPlot::mousePressEvent(event);
  mousePressed = true;
  if (event -> button() == Qt::RightButton) {
    qDebug() << "Right click";
  }
}

void PlotWidget::mouseReleaseEvent(QMouseEvent *event) {
  QCustomPlot::mouseReleaseEvent(event);
  mousePressed = false;
}

// BIG TODO:
// If one check, we call this multiple times for the vertical crosshair and vertical label.
// Keep track if change is actually needed, otherwise it is just unneccesary to replot.
void PlotWidget::mouseMoveEvent(QMouseEvent *event) {

  QCustomPlot::mouseMoveEvent(event);
  double yAxisIndex = yAxis2 -> pixelToCoord(event -> pos().y());
  int xAxisIndex = std::round(xAxis -> pixelToCoord(event -> pos().x()));

  // TODO FIX: This doens't work when moving mouse outside of data range.
  double time = dataContainer.getTimeValue(xAxisIndex);
  // We are outside the actual data boundary. Need this to plot axisLabels outside data.
  // TODO: This might break in the future. We don't update dataContainer with new values,
  // only append them to the ChartWidget, might be bad, might be ok, don't know now.
  if (time == 0.0) {
    double size = dataContainer.size();
    double upperUnixTime = dataContainer.getTimeValue(size - 1);
    time = upperUnixTime + (chartResolution * (xAxisIndex - size + 1));
  }

  emit sendXCoord(xAxisIndex, time);

  // Set crosshair position two times since we only want to replot once.
  if (mousePressed == true) {
    crosshair -> setHorizontalPosition(yAxisIndex);
    crosshair -> setVerticalPosition(xAxisIndex, time);
    chart -> rescaleValueAxis(false, true, chartLowerScale, chartUpperScale);
  }

  else {
    crosshair -> setHorizontalPosition(yAxisIndex);
    crosshair -> setVerticalPosition(xAxisIndex, time);
    replot();
  }
  
}

void PlotWidget::setChartResolution(const QString granularity) {
  this -> chartResolution = getResolutionFromGranularity(granularity);
}

const int PlotWidget::getChartResolution() {
  return chartResolution;
}

QVector<double> PlotWidget::getXAxis() {
  QVector<double> keys(getDataCount());
  std::iota(keys.begin(), keys.end(), 0);
  return keys;
}

void PlotWidget::setWatermark(const QString text) {
  //watermark = new QCPItemText(this);
  watermark -> setText(text);
  watermark -> setLayer("grid");
  QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  fixedFont.setPointSize(160);
  watermark -> setFont(fixedFont);
  watermark -> position -> setTypeY(QCPItemPosition::ptAxisRectRatio);
  watermark -> position -> setTypeX(QCPItemPosition::ptAxisRectRatio);
  watermark -> position -> setCoords(0.5, 0.5);
  QColor color = QColor("#51A39A");
  color.setAlphaF(0.03);
  watermark -> setColor(color);
}

void PlotWidget::addLegendItem(CustomLegendItem *item) {
  customLegend -> addItem(item);
}

void PlotWidget::removeLegendItem(CustomLegendItem *item) {
  customLegend -> removeItem(item);
}


SubPlot::SubPlot(PlotWidget *chart) {
  this -> mainChart = chart;
  rect = new QCPAxisRect(chart);
  chart -> plotLayout() -> addElement(1, 0, rect);
  chart -> plotLayout() -> setRowSpacing(0);
  rect -> setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
  rect -> setMargins(QMargins(0, 0, 0, 0));
  rect -> setMaximumSize(QSize(QWIDGETSIZE_MAX, 180));
  rect -> axis(QCPAxis::atBottom) -> setLayer("axes");
  rect -> axis(QCPAxis::atBottom) -> grid() -> setLayer("grid");

  // interconnect x axis ranges of main and bottom axis rects:
  connect(chart -> xAxis, SIGNAL(rangeChanged(QCPRange)), rect -> axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
  connect(rect -> axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), chart -> xAxis, SLOT(setRange(QCPRange)));

  // We hook up connection to the mainChart so we can receive x coordinate and update the crosshair accordingly.
  connect(chart, &PlotWidget::sendXCoord, this, &SubPlot::onReceivedVertical);

  // Want to rescale the graph when moving 
  connect(chart -> xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(rescaleGraph()));

  // We set the xAxis range from the mainChart since we want the plot to be shown accordingly
  // from the start. Otherwise it won't be visible until the we move the mouse.
  rect -> axis(QCPAxis::atBottom) -> setRange(chart -> xAxis -> range());

  // Remove tick labels, only want on the main graph (special I know).
  rect -> axis(QCPAxis::atBottom) -> setTickLabels(false);

  rect -> axis(QCPAxis::atBottom) -> grid() -> setPen(QPen(QColor("#3A3B3B"), 0.4));
  rect -> axis(QCPAxis::atBottom) -> setTickLabelColor(QColor("#BCBCBC"));

  rect -> axis(QCPAxis::atBottom) -> setTicker(chart->xAxis->ticker());
  rect -> axis(QCPAxis::atLeft) -> setVisible(false);
  rect -> axis(QCPAxis::atRight) -> setVisible(true);

  rect -> axis(QCPAxis::atRight) -> setBasePen(QPen(Qt::NoPen));
  rect -> axis(QCPAxis::atRight) -> setTickPen(QPen(Qt::NoPen));
  rect -> axis(QCPAxis::atRight) -> setSubTickPen(QPen(Qt::NoPen));
  rect -> axis(QCPAxis::atRight) -> setTickLabelColor(QColor("#BCBCBC"));
  rect -> axis(QCPAxis::atRight) -> setNumberFormat("f");
  rect -> axis(QCPAxis::atRight) -> setNumberPrecision(5);
  rect -> axis(QCPAxis::atRight) -> grid() -> setVisible(true);
  rect -> axis(QCPAxis::atRight) -> grid() -> setPen(QPen(QColor("#3A3B3B"), 0.4));

  rect -> setRangeDragAxes(rect -> axis(QCPAxis::atBottom), rect -> axis(QCPAxis::atRight));
  rect -> setRangeZoomAxes(rect -> axis(QCPAxis::atBottom), rect -> axis(QCPAxis::atRight));

  QCPMarginGroup *group = new QCPMarginGroup(chart);
  chart -> axisRect() -> setMarginGroup(QCP::msLeft|QCP::msRight, group);
  rect -> setMarginGroup(QCP::msLeft|QCP::msRight, group);

  //crosshair = new Crosshair(getXAxis(), getYAxis());
}

QCPAxis* SubPlot::getXAxis() {
  return rect -> axis(QCPAxis::atBottom);
}

QCPAxis* SubPlot::getYAxis() {
  return rect -> axis(QCPAxis::atRight);
}

void SubPlot::rescaleGraph() {
  graphToRescale -> rescaleValueAxis(false, true, 1.2, 1.2);
}

void SubPlot::onReceivedVertical(int xAxisIndex, double time) {
  //crosshair -> setVerticalPosition(xAxisIndex, time);
}

QCPGraph* SubPlot::initNewGraph(const QVector<double> newData, const QString color) {
  QCPGraph* graph = new QCPGraph(this -> getXAxis(), this -> getYAxis());
  graph -> setPen(QPen(QColor(color)));
  QVector<double> axis(newData.size());
  std::iota(std::begin(axis), std::end(axis), 0);
  graph -> setData(axis, newData);  
  return graph;
}

QCPBars* SubPlot::initNewBars(const QVector<double> newData, const QString color) {
  QCPBars *bars = new QCPBars(this -> getXAxis(), this -> getYAxis());
  bars -> setWidth(0.9);
  bars -> setPen(Qt::NoPen);
  QColor barColor = QColor(color);
  barColor.setAlpha(125);
  bars -> setBrush(barColor);
  QVector<double> axis(newData.size());
  std::iota(std::begin(axis), std::end(axis), 0);
  bars -> setData(axis, newData);
  return bars;
}

/*
Old code for creating box under.
Will be good when implementing custom indicators for separate boxes under graph.

  volumeRect = new QCPAxisRect(this);
  plotLayout() -> addElement(1, 0, volumeRect);
  volumeRect -> axis(QCPAxis::atRight) -> setVisible(true);
  volumeRect -> setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
  volumeRect -> axis(QCPAxis::atBottom) -> setLayer("axes");
  volumeRect -> axis(QCPAxis::atBottom) -> grid() -> setLayer("grid");
  volumeRect -> axis(QCPAxis::atBottom) -> grid() -> setVisible(false);
  volumeRect -> axis(QCPAxis::atBottom) -> setBasePen(QPen(QColor("#3A3B3B"), 1));
  volumeRect -> axis(QCPAxis::atBottom) -> setTickPen(QPen(QColor("#3A3B3B"), 1));
  volumeRect -> axis(QCPAxis::atBottom) -> setSubTickPen(QPen(QColor("#3A3B3B"), 1));
  volumeRect -> axis(QCPAxis::atBottom) -> setTickLabelColor(QColor("#BCBCBC"));

  volumeRect -> axis(QCPAxis::atLeft) -> setVisible(false);
  volumeRect -> axis(QCPAxis::atRight) -> grid() -> setVisible(true);
  volumeRect -> axis(QCPAxis::atRight) -> grid() -> setPen(QPen(QColor("#3A3B3B"), 0.4));
  volumeRect -> axis(QCPAxis::atRight) -> setBasePen(QPen(QColor("#3A3B3B"), 1));
  volumeRect -> axis(QCPAxis::atRight) -> setTickPen(QPen(QColor("#3A3B3B"), 1));
  volumeRect -> axis(QCPAxis::atRight) -> setSubTickPen(QPen(QColor("#3A3B3B"), 1));
  volumeRect -> axis(QCPAxis::atRight) -> setTickLabelColor(QColor("#BCBCBC"));
  volumeRect -> axis(QCPAxis::atRight) -> setNumberFormat("gb");
  volumeRect -> axis(QCPAxis::atRight) -> setNumberPrecision(0);

  Set margins.
  // CHANGED
  //volumeRect -> setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
  //volumeRect -> setMargins(QMargins(0, 0, 0, 0));

  // CHANGED
  //positiveVolume = new QCPBars(volumeRect -> axis(QCPAxis::atBottom), volumeRect -> axis(QCPAxis::atRight));
  //negativeVolume = new QCPBars(volumeRect -> axis(QCPAxis::atBottom), volumeRect -> axis(QCPAxis::atRight));

  // IMPORTANT. MOVE AXIS TOGETHER.
  // Main axis and volume axis move together.
  // CHANGED
  //connect(xAxis, SIGNAL(rangeChanged(QCPRange)), volumeRect -> axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
  //connect(volumeRect -> axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), xAxis, SLOT(setRange(QCPRange)));

  // SET DATETIME TO AXIS.
  //volumeRect -> axis(QCPAxis::atBottom) -> setTicker(customAxis);

  // ALIGNMENT AND MARGINS BETWEEN MAIN PLOT AND NEW RECT.
  //QCPMarginGroup *group = new QCPMarginGroup(this);
  //axisRect() -> setMarginGroup(QCP::msLeft|QCP::msRight, group);
  //volumeRect -> setMarginGroup(QCP::msLeft|QCP::msRight, group);


  //crosshair = new Crosshair(volumeRect -> axis(QCPAxis::atBottom), yAxis2);

  // CHANGED
  //volumeRect -> plottables()[0] -> rescaleValueAxis(false, true);
  //volumeRect -> plottables()[1] -> rescaleValueAxis(true, true);

*/