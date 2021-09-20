

#include "utilities.h"

QVector<double> getRawData(QCPDataContainer<QCPFinancialData> *data, const QString &source) {
  QVector<double> out;
  int idx = 0;
  for (auto it = data -> begin(); it != data -> end(); ++it) {
    if (source == "Close")
      out.append(it -> close);
    
    else if (source == "High")
      out.append(it -> high);

    else if (source == "Low")
      out.append(it -> low);

    else if (source == "Open")
      out.append(it -> open);

    // Might want to redo these down below, since it takes to much time.
    else if (source == "HL2")
      out.append((it -> high + it -> low) / 2.0);

    else if (source == "HLC3")
      out.append((it -> high + it -> low + it -> close) / 3.0);

    else if (source == "OHLC4")
      out.append((it -> high + it -> low + it -> open + it -> close) / 4.0);

    else if (source == "HL2C4")
      out.append((it -> high + it -> low + 2.0 * (it -> close)) / 4.0);

  }

  return out;
}



HSeparator::HSeparator(QWidget *parent, const int height, const QString color) : QWidget(parent) {
  this -> setFixedHeight(height);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setStyleSheet(QString("background-color:" + color));
}

CloseButton::CloseButton(const QString &text, QWidget *parent, QTreeWidgetItem *item) : QPushButton(text, parent) {
  this -> rootItem = item;
  this -> setObjectName("closeButton");
  QFile file("src/stylesheets/utilities.qss");
  file.open(QFile::ReadOnly);
  QString stylesheet = QLatin1String(file.readAll());
  setStyleSheet(stylesheet);
  connect(this, &CloseButton::clicked, this, &CloseButton::handleClicked);
}

