
#ifndef VOLATILITY_CALCULATIONS_H
#define VOLATILITY_CALCULATIONS_H

#include <QVector>

#include "trend.h"
#include "statistics.h"

std::tuple<QVector<double>, QVector<double>, QVector<double>> bb(const QVector<double> &data, const int period = 20, 
  const int stddev = 2);


#endif