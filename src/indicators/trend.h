#ifndef TREND_CALCULATIONS_H
#define TREND_CALCULATIONS_H
#include <QVector>

QVector<double> sma(const QVector<double> &data, const int period);
double nextSma(const QVector<double> &data, const int period);

QVector<double> ema(const QVector<double> &data, const int period);
double nextEma(const double close, const double prevEmaValue, const int period);

QVector<double> dema(const QVector<double> &data, const int period);
double nextDema(const double close, const double prevEmaValue, const int period);

QVector<double> lwma(const QVector<double> &data, const int period);

QVector<double> t3(const QVector<double> &data, const int period, const double volume_factor = 0.7);

QVector<double> tma(const QVector<double> &data, const int period);

QVector<double> wc(const QVector<double> &high, const QVector<double> &low, const QVector<double> &close);
double nextWc(const double high, const double low, const double close);

QVector<double> smma(const QVector<double> &data, const int period);


#endif