

#include "granularities.h"

int getResolutionFromGranularity(const QString granularity) {
  QMap<QString, int> granularities;
  granularities["S5"] = 5;
  granularities["S10"] = 10;
  granularities["S15"] = 15;
  granularities["S30"] = 30;
  granularities["M1"] = 60;
  granularities["M2"] = 60 * 2;
  granularities["M4"] = 60 * 4;
  granularities["M5"] = 60 * 5;
  granularities["M10"] = 60 * 10;
  granularities["M15"] = 60 * 15;
  granularities["M30"] = 60 * 30;
  granularities["H1"] = 3600;
  granularities["H2"] = 3600 * 2;
  granularities["H3"] = 3600 * 3;
  granularities["H4"] = 3600 * 4;
  granularities["H6"] = 3600 * 6;
  granularities["H8"] = 3600 * 8;
  granularities["H12"] = 3600 * 12;
  granularities["D"] = 3600 * 24;
  granularities["W"] = 3600 * 24 * 7;
  return granularities[granularity];
}

QStringList getGranularitiesList() {
  QStringList granularities = {"S5", "S10", "S15", "S30", "M1", "M2", "M4", "M5",
    "M10", "M15", "M30", "H1", "H2", "H3", "H4", "H6", "H8", "H12", "D", "W"};
  return granularities;
}