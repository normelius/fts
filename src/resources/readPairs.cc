

#include "readPairs.h"

QString getAvailablePairs() {
  QString availablePairs = "";
  QFile file("src/resources/pairs.txt");

  if (file.open(QIODevice::ReadOnly)) {
      QTextStream in(&file);
      while (!in.atEnd()) {
          availablePairs += ",";
          availablePairs += in.readLine().trimmed();
      }
  }

  return availablePairs;
}

QStringList getAvaiablePairsList() {
  QString availablePairs = getAvailablePairs();
  QStringList splitPairs = availablePairs.split(",");
  QStringList pairs = QStringList();

  for (int idx = 0; idx < splitPairs.length(); ++idx) {
    QString finalPair = splitPairs[idx].replace("_", "/");
    if (!finalPair.isEmpty())
      pairs.append(finalPair);
  }

  return pairs;
}
