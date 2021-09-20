

#include "volatility.h"


std::tuple<QVector<double>, QVector<double>, QVector<double>> bb(const QVector<double> &data, const int period, 
  const int stddev) {

  const int size = data.size();
  QVector<double> upper(size, std::numeric_limits<double>::quiet_NaN());
  QVector<double> middle(size, std::numeric_limits<double>::quiet_NaN());
  QVector<double> lower(size, std::numeric_limits<double>::quiet_NaN());

  QVector<double> ma = sma(data, period);

  QVector<double> stdvals = standardDeviation(data, period, false);

  for (int i = period - 1; i < size; ++i) {
    middle[i] = ma[i];
    lower[i] = ma[i] - (stddev * stdvals[i]);
    upper[i] = ma[i] + (stddev * stdvals[i]);
  }

  return std::tuple(upper, middle, lower);
}