
#ifndef INDICATOR_WIDGET_HELPER_H
#define INDICATOR_WIDGET_HELPER_H

#include <QVector>
#include "third-party/qcustomplot.h"

/*
Sets new data to a graph by replacing it with the old.
Also creates correct axis, i.e., 0, 1, 2, 3, ...
*/
void setGraphData(QCPGraph *graph, const QVector<double> &data);

/*
Appends a new data point to specified graph or replaces the last current data point.
*/
void updateGraphData(QCPGraph *graph, const double newData, const bool newPoint);

#endif