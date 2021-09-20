
/*
This file inherits a QTableWidget and makes the backbone of the list with
forex prices.
*/

#ifndef CUSTOM_FX_WIDGET_H 
#define CUSTOM_FX_WIDGET_H 

#include <QTableWidget>
#include <QThread>
#include <QWidget>
#include <QHeaderView>
#include <QFile>
#include <QTimer>
#include <QSizeGrip>

#include "src/streamdata.h"
#include "src/resources/readPairs.h"

class CustomFxWidget : public QTableWidget {
  Q_OBJECT

  public:
    CustomFxWidget(QWidget *parent = 0);
    ~CustomFxWidget();

  private:
    void initDataStream(const QString instrument);
    void initFxNames();
    // Used in order to see what color should blink. 
    // If price increase, green otherwise red.
    bool isPriceIncrease(double closePrice, int rowIndex);
    // USD_SEK,EUR_USD,...
    QString availablePairs;
    // {USD/SEK, EUR/USD, ...}
    QStringList pairs;
    StreamDataWorker *streamDataWorker;
    QThread *streamDataWorkerThread = new QThread();
    // Keep track of fx pair and its corresponding index in the table.
    // Needed for when we receving data and needs to update the prices.
    QMap<QString, int> pairsIndex;
    QBrush blinkUpColor; QBrush blinkDownColor;

  public slots:
    // Receive a StreamData object from StreamDataWorker thread.
    void receiveStreamData(StreamData streamData);
    // Call this when changing background to a QTableWidgetItem to original background.
    void blinkSlot(int rowIndex);
    
  signals:
    void startDataStream(const QString instrument, const int chartResolution = 300);
};


#endif