#ifndef CHARTTOPMENU_H
#define CHARTTOPMENU_H

#include <QWidget>
#include <QStringList>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QQuickView>
#include <QQmlContext>
#include <QUrl>
#include <QQuickItem>
#include <QListView>
#include <QLineEdit>

#include "src/resources/readPairs.h"
#include "src/resources/granularities.h"

class ChartTopMenu : public QWidget {
  Q_OBJECT

  public:
    ChartTopMenu(QWidget *parent = 0);
    QComboBox *chartTypeButton;
    QComboBox *forexPairButton;
    QPushButton *studiesButton;
    QComboBox *resolutionButton;

  private:
    // Creates resolution dropdown button.
    void setResolutionButton();
    // Creates dropdown button for specifying chart type, e.g., Candlestick or Line chart.
    void setChartTypeButton();
    // Creates dropdown button for specifying choice of forex.
    void setForexPairButton();
    // Sets the studies button.
    void setStudiesButton();



};

#endif