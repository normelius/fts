

#include "statistics.h"

QVector<double> standardDeviation(const QVector<double> &data, const int period, const bool normalize) {
  
  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());

  QVector<double> mean = sma(data, period);

  for (int i = 0; i < size - period + 1; ++i) {
    double temp = 0;

    for (int j = i; j < period + i; ++j) {
        temp += pow((data[j] - mean[i + period - 1]), 2);
    }

    if (normalize == true) {
        temp /= (period - 1);
    }

    else {
        temp /= period;
    }
  
    temp = sqrt(temp);
    out[i + period - 1] = temp;
  }

  return out;
}