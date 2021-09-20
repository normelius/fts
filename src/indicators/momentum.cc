
#include "momentum.h"

QVector<double> apo(const QVector<double> &data, const int period_slow, const int period_fast,
  const QString ma_type) {

  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());
  QVector<double> ma_fast, ma_slow;

  if (ma_type == "sma") {
    ma_fast = sma(data, period_fast);
    ma_slow = sma(data, period_slow);
  }

  else if (ma_type == "ema") {
    ma_fast = ema(data, period_fast);
    ma_slow = ema(data, period_slow);
  }

  for (int idx = period_slow - 1; idx < size; ++idx) {
    out[idx] = ma_fast[idx] - ma_slow[idx];
  }

  return out;
}


QVector<double> ppo(const QVector<double> &data, const int period_slow, const int period_fast,
  const QString ma_type) {

  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());
  QVector<double> ma_fast, ma_slow;

  if (ma_type == "sma") {
    ma_fast = sma(data, period_fast);
    ma_slow = sma(data, period_slow);
  }

  else if (ma_type == "ema") {
    ma_fast = ema(data, period_fast);
    ma_slow = ema(data, period_slow);
  }

  for (int idx = period_slow - 1; idx < size; ++idx) {
    out[idx] = ((ma_fast[idx] - ma_slow[idx]) / ma_slow[idx]) * 100.0;
  }

  return out;
}

// NOT DONE. DOESN'T WORK.
QVector<double> aroon(const QVector<double> &high, const QVector<double> &low, const int period) {

  const int size = high.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());


  for (int idx = period; idx < size; ++idx) {
    int max = std::distance(high.begin(), std::max_element(high.begin() + idx - period, high.begin() + idx + 1));
    int min = std::distance(low.begin(), std::min_element(low.begin() + idx - period, low.begin() + idx+ 1));

    int days_up = idx - max;
    int days_down = idx - min;

    double aroon_up = ((double)(period - days_up) / period) * 100;
    double aroon_down = ((double)(period - days_down) / period) * 100;
    out[idx] = aroon_up - aroon_down;
  }

  return out;
}

QVector<double> bop(const QVector<double> &high, const QVector<double> &low, 
  const QVector<double> &open, const QVector<double> &close) {

  const int size = high.size();
  QVector<double> out(size, 0.0); 

  for (int idx = 0; idx < size; ++idx) {
    double numerator = high[idx] - low[idx];

    if (numerator > 0) {
      out[idx] = (close[idx] - open[idx]) / numerator;
    }
  }

  return out;
}


QVector<double> cci(const QVector<double> &high, const QVector<double> &low, 
  const QVector<double> &close, const int period) {

  const int size = high.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());
  QVector<double> typical_price(size, 0.0);

  for (int idx = 0; idx < size; ++idx) {
    typical_price[idx] = (close[idx] + high[idx] + low[idx]) / 3.0;
  }

  QVector<double> typical_price_sma = sma(typical_price, period);

  const double constant = 0.015;

  for (int idx = period - 1; idx < size; ++idx) {
    double mean_dev = 0.0;
    for (int idx1 = idx - period + 1; idx1 <= idx; ++idx1) {
      mean_dev += abs(typical_price_sma[idx] - typical_price[idx1]);
    }

    mean_dev /= period;
    out[idx] = (typical_price[idx] - typical_price_sma[idx]) / (constant * mean_dev);
  }

  return out;
}

QVector<double> willr(const QVector<double> &high, const QVector<double> &low, 
  const QVector<double> &close, const int period) {

  const int size = high.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());

  for (int idx = period - 1; idx < size; ++idx) {
    double max = *std::max_element(high.begin() + idx - period + 1, high.begin() + idx + 1);
    double min = *std::min_element(low.begin() + idx - period + 1, low.begin() + idx + 1);
    out[idx] = ((max - close[idx]) / (max - min)) * -100.0;
  }

  return out;
}

QVector<double> cmo(const QVector<double> &data, const int period) {
  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());
  QVector<double> diff_up(size, 0.0); 
  QVector<double> diff_down(size, 0.0);

  double cmo_down = 0.0;
  double cmo_up = 0.0;

  for (int idx = 1; idx < size; ++idx) {
    double diff = data[idx] - data[idx - 1];
    if (diff > 0.0) {
      diff_up[idx] = diff;
    }

    else if (diff < 0.0) {
      diff_down[idx] = diff * -1.0;
    }
        
    // Increment cmo_up/cmo_down with the differences. In case the stock rose one day,
    // the down diff will be zero, so both variables can be incremented at the same time.
    cmo_up += diff_up[idx];
    cmo_down += diff_down[idx];

    // Remove first value in each new period.
    if (idx > period) {
      cmo_down -= diff_down[idx - period];
      cmo_up -= diff_up[idx - period];
    }
        
    if (idx >= period) {
      out[idx] = ((cmo_up - cmo_down) / (cmo_up + cmo_down)) * 100;
    }

  }

  return out;
}


QVector<double> rsi(const QVector<double> &data, const int period, const QString rsi_type) {
  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());
  QVector<double> gains(size, 0.0); 
  QVector<double> losses(size, 0.0);

  // Calculate average gains and losses vector, size -1 compared to size of data.
  for (int idx = 1; idx < size; ++idx) {
    double diff = data[idx] - data[idx - 1];
    if (diff > 0) {
      gains[idx] = diff;
    }

    else if (diff < 0) {
      losses[idx] = diff * -1.0;
    }
  }
    
  // First average gain/loss.
  double AG = 0.0;
  double AL = 0.0;

  for (int idx = 1; idx <= period; ++idx) {
    AG += gains[idx];
    AL += losses[idx];
  }

  AG /= period;
  AL /= period;
  out[period] = 100.0 - (100.0 / (1.0 + (AG / AL)));

  for (int idx = period + 1; idx < size; ++idx) {
    if (rsi_type == "smoothed") {
      AG = ((AG * (period - 1)) + gains[idx]) / period;
      AL = ((AL * (period - 1)) + losses[idx]) / period;
    }
        
    else if (rsi_type == "standard") { 
      for (int idx1 = idx - period + 1; idx1 <= idx; ++idx1) {
        AG += gains[idx1];
        AL += losses[idx1];
      }

      AG /= period;
      AL /= period;
    }

    out[idx] = 100.0 - (100.0 / (1.0 + (AG / AL)));
  }

  return out;
}

QVector<double> mi(const QVector<double> &data, const int period) {

  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());

  for (int idx = period; idx < size; ++idx) {
    out[idx] = data[idx] - data[idx - period];
  }

  return out;
}

QVector<double> roc(const QVector<double> &data, const int period) {

  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());
    
  for (int idx = period; idx < size; ++idx) {
    out[idx] = ((data[idx] - data[idx - period]) / data[idx - period]) * 100.0;
  }

  return out;
}

std::tuple<QVector<double>, QVector<double>, QVector<double>> macd(const QVector<double> &data,
  const int slowPeriod, const int fastPeriod, const int signalSmoothing) {

  const int size = data.size();
  QVector<double> out(size, std::numeric_limits<double>::quiet_NaN());
  QVector<double> histogram(size, std::numeric_limits<double>::quiet_NaN());

  QVector<double> slowEma = ema(data, slowPeriod);
  QVector<double> fastEma = ema(data, fastPeriod);

  for (int idx = slowPeriod - 1; idx < size; ++idx) {
    out[idx] = fastEma[idx] - slowEma[idx];
  }
  
  QVector<double> signal = ema(out, signalSmoothing);

  for (int idx = slowPeriod - 1 + signalSmoothing; idx < size; ++idx) {
    histogram[idx] = out[idx] - signal[idx];
  }

  return std::tuple(out, signal, histogram);
}