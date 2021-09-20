
#include "trend.h"
#include <numeric>

/*
Implementation of Simple Moving Average (SMA).
*/
QVector<double> sma(const QVector<double> &data, const int period) {
  int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());

  // Check leading NaNs and adjust calculation below. This is needed if arg data contains
  // leading NaNs, which will occur when calculating other indicators.
  int adjust_nan = 0;
  for (int idx = 0; idx < size; ++idx) {
    if (!std::isnan(data[idx]))
      break;

    ++adjust_nan;
  }

  double temp = 0.0;
  for (int idx = 0 + adjust_nan; idx < size; ++idx) {
    temp += data[idx];

    if (idx >= period + adjust_nan) {
      temp -= data[idx - period];
    }

    if (idx >= (period - 1 + adjust_nan)) {
      out[idx] = (double) temp / period;
    }
  }

  return out;
}

/*
Implementation of Simple Moving Average.
Returns latest value.
*/
double nextSma(const QVector<double> &data, const int period) {
  double out = 0.0;
  const int size = data.size();
  for (int idx = size - period; idx < size; ++idx) {
    out += data[idx];
  }

  return (double) out / period;
}

/**
 * Impelentation of Exponential Moving Average
 */
QVector<double> ema(const QVector<double> &data, const int period) {

  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());

  // Check leading NaNs and adjust calculation below. This is needed if arg data contains
  // leading NaNs, which will occur when calculating other indicators.
  int adjust_nan = 0;
  for (int idx = 0; idx < size; ++idx) {
    if (!std::isnan(data[idx]))
      break;

    ++adjust_nan;
  }

  // Start with sma for first data point.
  double value = std::accumulate(data.begin() + adjust_nan, data.begin() + period + adjust_nan, 0.0);
  value /= period;
  out[period - 1 + adjust_nan] = value;

  // Multiplier, i.e. 18.18% weight with period 10;
  const double multiplier = (double) 2.0 / (period + 1.0);
  for (int idx = period + adjust_nan; idx < size; ++idx) {
    value = (data[idx] - value) * multiplier + value; 
    out[idx] = value;
  }

  return out;
}

double nextEma(const double data, const double previousEma, const int period) {
  double multiplier = (double) 2.0 / (period + 1.0);
  return (data - previousEma) * multiplier + previousEma;
}


/**
 * Impelentation of Double Exponential Moving Average
 */
QVector<double> dema(const QVector<double> &data, const int period) {

  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());

  const QVector<double> ema1 = ema(data, period);
  const QVector<double> ema2 = ema(ema1, period);

  for (int idx = 2 * period - 2; idx < size; ++idx) {
    out[idx] = 2 * ema1[idx] - ema2[idx];
  }

  return out;
}

/**
 * Impelentation of Linear Weighted Moving Average
 */
QVector<double> lwma(const QVector<double> &data, const int period) {

  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());

  for (int idx = 0; idx < size - period + 1; ++idx) {
    double temp = 0.0;
    int W = 1;
    int W_sum = 0;
    for (int ii = idx; ii < period + idx; ++ii) {
      temp += (data[ii] * W);
      W_sum += W;
      ++W;
    }

    temp /= W_sum;
    out[idx + period - 1] = temp;
  }

  return out;
}


/**
 * Impelentation of T3. 
 */
QVector<double> t3(const QVector<double> &data, const int period, const double volume_factor) {

  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());

  QVector<double> ema1 = ema(data, period);
  QVector<double> ema2 = ema(ema1, period);
  QVector<double> ema3 = ema(ema2, period);
  QVector<double> ema4 = ema(ema3, period);
  QVector<double> ema5 = ema(ema4, period);
  QVector<double> ema6 = ema(ema5, period);
    
  const double c1 = -std::pow(volume_factor, 3);
  const double c2 = 3 * std::pow(volume_factor, 2) + 3 * std::pow(volume_factor, 3);
  const double c3 = - 6 * std::pow(volume_factor, 2) - 3 * volume_factor - 3 * std::pow(volume_factor, 3);
  const double c4 = 1 + 3 * volume_factor + std::pow(volume_factor, 3) + 3 * std::pow(volume_factor, 2);
    
  for (int idx = period * 5 - 1; idx < size; ++idx) {
    out[idx] = c1 * ema6[idx] + c2 * ema5[idx] + c3 * ema4[idx] + c4 * ema3[idx];
  }

  return out;
}

QVector<double> tma(const QVector<double> &data, const int period) {

  const int size = data.size();

  int first_period;
  int second_period;

  if (period % 2 == 0) {
    first_period = period / 2;
    second_period = (period / 2) + 1;
  }

  else {
    first_period = std::ceil((int) (period + 1) / 2);
    second_period = std::ceil((int) (period + 1) / 2);
  }

  QVector<double> smaVec = sma(data, first_period);
  QVector<double> out = sma(smaVec, second_period);

  return out;
}

QVector<double> wc(const QVector<double> &high, const QVector<double> &low, const QVector<double> &close) {

  const int size = high.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());

  for (int idx = 0; idx < size; ++idx) {
    out[idx] = ((close[idx] * 2) + high[idx] + low[idx]) / 4.0;
  }

  return out;
}

double nextWc(const double high, const double low, const double close) {
  return ((close * 2) + high + low) / 4.0;
}


QVector<double> smma(const QVector<double> &data, const int period) {

  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());
  
  // Start with SMA for first value.
  double value = std::accumulate(data.begin(), data.begin() + period, 0.0) / period;
  out[period - 1] = value;

  for (int idx = period; idx < size; ++idx) {
    value = (value * (period - 1) + data[idx]) / period;
    out[idx] = value;
  }

  return out;
}
