
/*
Helper function to get available pairs from text file in resources.
*/
#ifndef READ_PAIRS_H
#define READ_PAIRS_H

#include <QString>
#include <QFile>
#include <QDebug>

// Returns available pairs as QString(USD_SEK,EUR_USD,DKK_SEK,...)
QString getAvailablePairs();
// Returns available pairs as QStringList{USD/SEK, EUR/USD, DKK/SEK,...}
QStringList getAvaiablePairsList();


#endif