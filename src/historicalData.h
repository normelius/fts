#ifndef GETHISTORICALDATA_H 
#define GETHISTORICALDATA_H 

#include <QVector>
#include <QDateTime>
#include "third-party/qcustomplot.h"
#include "src/resources/granularities.h"

// ------------------------------------------
class FinancialData {
  public:
    FinancialData(double high, double low, double open, 
      double close, double volume, double time,
      int resolutionCount);

    double high;
    double low;
    double open;
    double close;
    double volume;
    // Unix time.
    double time;
    int resolutionCount;
};

// ------------------------------------------
class FinancialDataContainer {
  public:
    void add(FinancialData data);
    QVector<FinancialData> container;
    QVector<double> getTimeVector();
    QVector<double> getHighVector();
    QVector<double> getLowVector();
    QVector<double> getOpenVector();
    QVector<double> getCloseVector();
    QVector<double> getVolumeVector();
    // Returns the UNIX time from a given x-axis index point.
    double getTimeValue(const int xAxisIndex);
    // Returns the latest resolution count, needed in order to keep track when we change bars.
    int getResolutionCount();
    // Returns the size of the FinancialDataContainer. More precise, the size of QVector container.
    int size();

};



class HistoricalDataWorker : public QObject  {
  Q_OBJECT

  public:
    HistoricalDataWorker();

  public slots:
    void startDataRetrieval(const QString instrument, const QString granularity);

  signals:
    void emitHistoricalData(FinancialDataContainer container);
  
};

#endif