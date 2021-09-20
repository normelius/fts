/*
#ifndef MOMENTUM_WIDGET_H 
#define MOMENTUM_WIDGET_H

#include <QDebug>
#include <QWidget>
#include <QString>
#include <QVBoxLayout>
#include <QLabel>
#include <QFontDatabase>
#include <QFrame>
#include <QLineEdit>
#include <QPushButton>
#include <QObject>
#include <QTreeWidget>

#include "src/utilities.h"
#include "src/plotWidget/activeWidget.h"
#include "third-party/qcustomplot.h"
#include "src/streamdata.h"
#include "src/indicators/trend.h"
#include "src/indicators/momentum.h"
#include "src/plotWidget/indicatorWidget/baseOverlay.h"
#include "src/plotWidget/plotWidget.h"

class ActiveWidget;

class MacdWidget : public QWidget {
  Q_OBJECT
  public:
    MacdWidget(QWidget *parent = 0, ActiveWidget *activeWidget = 0);
    QPushButton *button;
  private:
    QLineEdit *fastPeriodInput;
    QLabel *infoText;
    ActiveWidget *activeWidget;
  public slots:
    void addNewStudy();
};

class MacdIndicator : public QObject, public BaseOverlayIndicator {
  Q_OBJECT
  public:
    void newGraph(QCPGraph *graph, QCPDataContainer<QCPFinancialData> *data){};
    void newSubplot(SubPlot *subplot, QCPDataContainer<QCPFinancialData> *data);
    void updateData(QCPDataContainer<QCPFinancialData> *data, const bool newPoint = false);
    void replaceData(QCPDataContainer<QCPFinancialData> *data);
    // Takes the macd histogram and returns the positive/negative vectors with histograms. 
    std::tuple<QVector<double>, QVector<double>> getHistograms(QVector<double> histogram);
    int fastPeriod = 12;
    int slowPeriod = 26;
    int signalSmoothing = 9;

  private:
    QCPGraph *macdGraph;
    QCPGraph *macdSignalGraph;
    QCPBars *posMacdHistogram;
    QCPBars *negMacdHistogram;

  public slots:
    void fastPeriodChanged(QString text);

};

#endif
*/