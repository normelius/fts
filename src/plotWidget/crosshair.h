

#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include <QObject>
#include <QDateTime>
#include "third-party/qcustomplot.h"

/*
Create a Crosshair object for displaying both vertical and horizontal crosshair.
The crosshair automatically displays both x- and y tags, i.e., labels over the axes.
*/
class Crosshair : QObject{
  Q_OBJECT 

  public:
    Crosshair(QCPAxis *xAxis, QCPAxis *yAxis);
    // Sets the horizontal position for the crosshair.
    // The position is the actual value displayed on the y-axis.
    void setHorizontalPosition(const double yPosition);

    // Sets the vertical position for the crosshair.
    // The position is the actual value displayed on the x-axis.
    // Unixtime is necessary to create correct axis overlay labels.
    void setVerticalPosition(const int xPosition, const double unixtime);

  private:
    void setHorizontalLabel(double yValue);
    void setVerticalLabel(const double unixTime, const QString &format = "dd MMMM yy hh:mm");
    QCPItemTracer *hLine;
    QCPItemTracer *vLine;
    QCPItemText *hLineLabel;
    QCPItemText *vLineLabel;
  
};


#endif