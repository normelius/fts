


#include "crosshair.h"
#include <cmath>



Crosshair::Crosshair(QCPAxis *xAxis, QCPAxis *yAxis) {

  xAxis -> parentPlot() -> setCursor(Qt::CrossCursor);

  QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  fixedFont.setPointSize(12);

  hLine = new QCPItemTracer(yAxis -> parentPlot());
  hLine -> position -> setTypeX(QCPItemPosition::ptAxisRectRatio);
  hLine -> position -> setTypeY(QCPItemPosition::ptPlotCoords);
  hLine -> setStyle(QCPItemTracer::tsCrosshair);
  hLine -> setPen(QPen(QColor("#8E8E8E"), 0.4, Qt::DashLine));
  hLine -> position -> setAxisRect(yAxis -> axisRect());
  hLine -> position -> setAxes(0, yAxis);
  // Need to initialize this, otherwise hLineLabel won't show up, for some reason.
  hLine -> position -> setCoords(1, 0);

  vLine = new QCPItemTracer(xAxis -> parentPlot());
  vLine -> position -> setTypeX(QCPItemPosition::ptPlotCoords);
  vLine -> position -> setTypeY(QCPItemPosition::ptAxisRectRatio);
  vLine -> setStyle(QCPItemTracer::tsCrosshair);
  vLine -> setPen(QPen(QColor("#8E8E8E"), 0.4, Qt::DashLine));
  vLine -> position -> setAxisRect(xAxis -> axisRect());
  vLine -> position -> setAxes(xAxis, 0);
  vLine -> setLayer("overlay");
  vLine -> position -> setCoords(0, 1);

  hLineLabel = new QCPItemText(yAxis -> parentPlot());
  hLineLabel -> setLayer("overlay");
  hLineLabel -> setPadding(QMargins(5, 2, 5, 2));
  hLineLabel -> setBrush(QBrush("#61D399"));
  hLineLabel -> setFont(fixedFont);
  hLineLabel -> setClipToAxisRect(false);
  hLineLabel -> setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  hLineLabel -> position -> setParentAnchor(hLine -> position);

  vLineLabel = new QCPItemText(xAxis -> parentPlot());
  vLineLabel -> setLayer("overlay");
  vLineLabel -> setPadding(QMargins(5, 2, 5, 2));
  vLineLabel -> setBrush(QBrush("#61D399"));
  vLineLabel -> setFont(fixedFont);
  vLineLabel -> setClipToAxisRect(false);
  vLineLabel -> setPositionAlignment(Qt::AlignHCenter|Qt::AlignTop);
  vLineLabel -> position -> setParentAnchor(vLine -> position);
}


void Crosshair::setHorizontalPosition(const double yPosition) {
  hLine -> position -> setCoords(1, yPosition);
  setHorizontalLabel(yPosition);
}

void Crosshair::setVerticalPosition(int xPosition, const double time) {
  // Round to nearest integer since xAxis is based of integer values, 0, 1, 2 ...
  // This means we only snap vertical crosshair at the actual ticks.
  vLine -> position -> setCoords(xPosition, 1);
  setVerticalLabel(time);
}

void Crosshair::setHorizontalLabel(double yValue) {
  hLineLabel -> setText(QString::number(yValue, 'f', 5));
}

void Crosshair::setVerticalLabel(const double unixTime, const QString &format) {
  QDateTime datetime;
  datetime.setSecsSinceEpoch(unixTime);
  QString tickLabel = datetime.toString(format);
  
  vLineLabel -> setText(tickLabel);
}
