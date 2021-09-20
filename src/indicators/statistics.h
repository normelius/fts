#ifndef STATISTICS_CALCULATIONS_H
#define STATISTICS_CALCULATIONS_H

#include <QVector>

#include "trend.h"

QVector<double> standardDeviation(const QVector<double> &data, const int period, const bool normalize = true);

#endif