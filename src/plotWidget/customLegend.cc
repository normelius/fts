

#include "customLegend.h"

CustomLegend::CustomLegend(QCustomPlot *chart) : QCPItemRect(chart) {
  this -> chart = chart;
  this -> topLeftX = margin;
  this -> topLeftY = margin;
  this -> bottomRightX = margin + width;
  this -> bottomRightY = margin + height;
  topLeft -> setType(QCPItemPosition::ptAbsolute);
  bottomRight -> setType(QCPItemPosition::ptAbsolute);
  topLeft -> setCoords(topLeftX, topLeftY);
  bottomRight -> setCoords(bottomRightX, bottomRightY);
  // No borders.
  setPen(QPen(QColor(0, 0, 0, 0)));
}

void CustomLegend::addItem(CustomLegendItem *item) {
  numItems++;
  items.push_back(item);
  int topY = margin + (customLegendItemHeight * (numItems - 1));
  int bottomY = margin + customLegendItemHeight * numItems;
  item -> setPosition(topLeftX, topY, bottomRightX, bottomY);
}

void CustomLegend::removeItem(CustomLegendItem *item) { 
  numItems--;
  items.erase(std::remove(items.begin(), items.end(), item), items.end());
  chart -> removeItem(item);
  // Reposition the elements so we don't get gaps.
  for (int idx = 0; idx < items.size(); ++idx) {
    int topY = margin + (customLegendItemHeight * (idx));
    int bottomY = margin + customLegendItemHeight * (idx + 1);
    items[idx] -> setPosition(topLeftX, topY, bottomRightX, bottomY);
  }
}



// CustomLegendItem
CustomLegendItem::CustomLegendItem(QCustomPlot *chart, const QString &nameText,
    const QString &periodText) : QCPItemRect(chart) {
  this -> chart = chart;
  topLeft -> setType(QCPItemPosition::ptAbsolute);
  bottomRight -> setType(QCPItemPosition::ptAbsolute);
  QColor bgColor = QColor("#EEEEEE");
  bgColor.setAlpha(10);
  setBrush(QBrush(bgColor));
  setPen(QPen(QColor(0, 0, 0, 0)));

  // Indicator name
  name = new QCPItemText(chart);
  name -> setColor(QColor("#EEEEEE"));
  name -> setFont(QFont("Roboto", 15, QFont::Bold));
  name -> setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  name -> setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  setName(nameText);

  // Period if available.
  period = new QCPItemText(chart);
  period -> setColor(QColor("#EEEEEE"));
  period -> setFont(QFont("Roboto", 14));
  period -> setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  period -> setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  setPeriod(periodText);

  // Value.
  value = new QCPItemText(chart);
  value -> setColor(QColor("#EEEEEE"));
  value -> setFont(QFont("Roboto", 14));
  value -> setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  value -> setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
}

void CustomLegendItem::setName(const QString &text) {
  name -> setText(text);
}

void CustomLegendItem::setPeriod(const QString &text) {
  period -> setText(text);
}

void CustomLegendItem::setValue(const QString &text) {
  value-> setText(text);
}

void CustomLegendItem::setPosition(const int topLeftX, const int topLeftY, 
    const int bottomRightX, const int bottomRightY) {

  // Places the legenditem so we perhaps can click on it in the future.
  topLeft -> setCoords(topLeftX, topLeftY);
  bottomRight -> setCoords(bottomRightX, bottomRightY);    

  // Anchors the name to the CustomLegendItem, 
  name -> position -> setParentAnchor(this -> left);
  name -> position -> setCoords(10, 0);

  // Anchors the period.
  period -> position -> setParentAnchor(this -> left);
  period -> position -> setCoords(10 + spaceBetweenElements, 0);

  // Anchors the value.
  value -> position -> setParentAnchor(this -> left);
  value -> position -> setCoords(10 + spaceBetweenElements * 2, 0);
}

