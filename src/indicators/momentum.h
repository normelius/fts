#ifndef MOMENTUM_INDICATORS_H
#define MOMENTUM_INDICATORS_H

#include <QVector>
#include <numeric>

#include "trend.h"

// APO
QVector<double> apo(const QVector<double> &data, const int period_slow = 26, const int period_fast = 12,
  const QString ma_type = "sma");
  
// PPO - Percentage Price Oscillator
QVector<double> ppo(const QVector<double> &data, const int period_slow = 26, const int period_fast = 12,
  const QString ma_type = "ema");

// AROON
QVector<double> aroon(const QVector<double> &high, const QVector<double> &low, const int period);

// Balance of Power
QVector<double> bop(const QVector<double> &high, const QVector<double> &low, 
  const QVector<double> &open, const QVector<double> &close);

// Commodity Channel Index 
QVector<double> cci(const QVector<double> &high, const QVector<double> &low, 
  const QVector<double> &close, const int period);

// Will'r - Williams R 
QVector<double> willr(const QVector<double> &high, const QVector<double> &low, 
  const QVector<double> &close, const int period);

// Chande Momentum Indicator 
QVector<double> cmo(const QVector<double> &data, const int period);

// RSI - Relative Strength Index
QVector<double> rsi(const QVector<double> &data, const int period, const QString rsi_type = "smoothed");

// Momentum Indicator 
QVector<double> mi(const QVector<double> &data, const int period);

// RoC - Rate of Change  
QVector<double> roc(const QVector<double> &data, const int period);

// MACD 
std::tuple<QVector<double>, QVector<double>, QVector<double>> macd(const QVector<double> &data, const int slowPeriod = 26, 
  const int fastPeriod = 12, const int signalSmoothing = 9);


#endif


