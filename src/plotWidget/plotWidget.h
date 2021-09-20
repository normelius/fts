#ifndef PLOTWIDGET_H 
#define PLOTWIDGET_H 

#include <QWidget>
#include <QString>
#include <QLocale>
#include <QChar>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QBrush>
#include <QColor>
#include <QPen>
#include "third-party/qcustomplot.h"
#include "src/historicalData.h"
#include "src/plotWidget/crosshair.h"
#include "src/plotWidget/customLegend.h"

class CustomDateAxis : public QCPAxisTickerDateTime {
  public:
    void setCustomTicksMapping(QVector<double> labelTicks);
    QVector<double> customLabelTicks; 
    int chartResolution;

  protected:
    virtual QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) override;
};


class PlotWidget : public QCustomPlot {
    Q_OBJECT

    public:
      PlotWidget(QWidget *parent = 0);
      void createChart();
      // Sets the chart resolution in seconds. Standard is 5 minute data, i.e., 300 seconds.
      void setChartResolution(const QString granularity);
      // Creates a new candlestick, usually used when a timeperiod, e.g., 5 minutes has passed and
      // a new candle is needed.
      void addNewCandle(const double closePrice);
      // Updates the latest candlestick with live data.
      void updateLatestCandle(const double closePrice);
      // Adds a new closing price data point. Only used when having "Area" or "Line" chart type.
      void addNewDataPoint(const double closePrice);
      void updateLatestDataPoint(const double closePrice);
      void setWatermark(const QString watermark);
      // Replaces the current data in the chart with a new candlestick data.
      void setNewData(FinancialDataContainer dataContainer);
      // Rescales and replots the current chart, so we always fit the whole data in the view.
      void replotChart();
      void setIndicatorMA();
      // Returns the used chart resolution in seconds.
      const int getChartResolution();
      // Returns the number of datapoints in the current graph.
      int getDataCount();
      // Hide the candlestick/ohlc chart. Normally used when switching chart types.
      void showChart(bool visible);
      // Changes the chart type between "Line", "Candlestick" and "OHLC".
      void setChartType(const QString chartType);
      // Creates a Line chart/Area chart with closing prices.
      void setLineChart(const bool areaChart = false);
      void removeLineChart();
      QVector<double> getXAxis();
      QCPDataContainer<QCPFinancialData> * getPointerData();
      /*
      Creates a new QCPGraph to the main plot. Correctly adds the graph to the xAxis and yAxis2,
      i.e., right side of the chart. Returns the newly given graph, which in most cases is used 
      when creating new indicators.
      */
      QCPGraph *newGraph();
      void addLegendItem(CustomLegendItem *item);
      void removeLegendItem(CustomLegendItem *item);

      CustomLegend *customLegend;

    private:
      void setCustomAxis(QVector<double> time);
      QCPFinancial *chart;
      QCPAxisRect *volumeRect;
      Crosshair *crosshair;
      QCPItemText *watermark = new QCPItemText(this);
      FinancialDataContainer dataContainer; 
      QCPGraph *lineChart;
      // Have a custom axis in order to change the ticklabels.
      //QSharedPointer<CustomDateAxis> customAxis(new CustomDateAxis);

      // Offset to the right in the graph that is possible. We don't want to be able to move
      // to much so the data disappears to the right.
      int horizontalViewOffset; 
      int numberDataPoints;
      int chartResolution;
      // When dragging plot auto zoom y-axis should be enabled. Meaning we need to keep
      // track of when we do a drag so we can call rescaleValueAxis.
      bool mousePressed = false;
      int maxVisibleDataPoints;
      // Handles the space between the lowest visible data point and xAxis.
      double chartLowerScale = 1.1;
      // Handles the space between the highest visible data point and top axis.
      double chartUpperScale = 1.0;
      // Handles the space between the highest visible volume point and top axis..
      double volumeUpperScale = 8.0;

    protected:
      void wheelEvent(QWheelEvent *event) override;
      void mousePressEvent(QMouseEvent *event) override;
      void mouseReleaseEvent(QMouseEvent *event) override;
      void mouseMoveEvent(QMouseEvent *event) override;

    signals:
      // Used for sending the necessary data to subplots so we can update their crosshair positions.
      void sendXCoord(int xCoord, double time);

};


/*
This class is serves as the base for creating a new plot below the main Chart.
Normally this is used for non-overlay indicators such as RSI and MACD.
*/
class SubPlot : public QObject {
  Q_OBJECT
  public:
    SubPlot(PlotWidget *chart);
    QCPAxis* getXAxis();
    QCPAxis* getYAxis();
    QCPGraph* initNewGraph(const QVector<double> newData, const QString color = "#286EE1");
    QCPBars* initNewBars(const QVector<double> newData, const QString color = "#286EE1");
    QCPGraph *graphToRescale;

  private:
    PlotWidget *mainChart;
    QCPAxisRect *rect;
    Crosshair *crosshair;

  public slots:
    // Rescales the data in the subplot in order to always fit the data.
    void rescaleGraph();
    void onReceivedVertical(int xAxisIndex, double time);
};


#endif