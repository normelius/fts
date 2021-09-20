
#ifndef CUSTOM_LEGEND_H
#define CUSTOM_LEGEND_H

#include <QDebug>
#include <QVector>
#include "third-party/qcustomplot.h"

class CustomLegendItem;

/*
Custom legend that handles CustomLegendItems which will be displayed on top of the main chart.
These CustomLegendItems will usually be created from different indicators.
*/
class CustomLegend : public QCPItemRect {
  public:
    CustomLegend(QCustomPlot *parent = nullptr);
    void addItem(CustomLegendItem *item);
    void removeItem(CustomLegendItem *item);

    int numItems = 0; 

  private:
    QCustomPlot *chart;
    std::vector<CustomLegendItem *> items;
    int width = 150;
    int height = 100;
    int customLegendItemHeight = 30;
    const int margin = 25;
    int topLeftX;
    int topLeftY;
    int bottomRightX;
    int bottomRightY;
};



class CustomLegendItem : public QCPItemRect {
  public:
    CustomLegendItem(QCustomPlot *parent = nullptr, const QString &nameText = "",
      const QString &periodText = "");
    void setPosition(const int topLeftX, const int topLeftY, 
      const int bottomRightX, const int bottomRightY);
    void setName(const QString &text);
    void setPeriod(const QString &text);
    void setValue(const QString &text);

  private:
    QCustomPlot *chart;
    QCPItemText *name;
    QCPItemText *period;
    QCPItemText *value;
    int spaceBetweenElements = 35;
};


#endif