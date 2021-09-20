#ifndef GET_GRANULARITIES_H
#define GET_GRANULARITIES_H

#include <QString>
#include <QMap>

#include <QStringList>

// Granularity is on the form "M5", "M1", ...
int getResolutionFromGranularity(const QString granularity);
QStringList getGranularitiesList();

#endif