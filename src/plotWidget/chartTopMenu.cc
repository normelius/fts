

#include <QStyleFactory>
#include <QStyledItemDelegate>

#include "chartTopMenu.h"

ChartTopMenu::ChartTopMenu(QWidget *parent) : QWidget(parent) {
  setMaximumHeight(34);

  setResolutionButton();
  setChartTypeButton();
  setForexPairButton();
  setStudiesButton();
  QHBoxLayout *layout = new QHBoxLayout();
  layout -> setSpacing(6);
  layout -> setContentsMargins(10, 0, 0, 0);
  layout -> addWidget(forexPairButton);
  layout -> addWidget(resolutionButton);
  layout -> addWidget(chartTypeButton);
  layout -> addWidget(studiesButton);
  layout -> addStretch();

  // This serves as the actual header, which is needed in order to
  // specify e.g., a background color.
  QWidget *header = new QWidget;
  header -> setObjectName("chartTopMenu");
  header -> setLayout(layout);

  QHBoxLayout *newLayout = new QHBoxLayout();
  newLayout -> addWidget(header);
  newLayout -> setSpacing(0);
  newLayout -> setContentsMargins(0, 0, 0, 0);
  setLayout(newLayout);
}

void ChartTopMenu::setForexPairButton() {
  //QStringList commands = {"EUR_USD", "USD_SEK", "USD_JPY"};
  QStringList commands = getAvaiablePairsList();
  forexPairButton = new QComboBox(this);
  forexPairButton -> view() -> setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  forexPairButton -> setStyle(QStyleFactory::create("Windows"));
  forexPairButton -> setObjectName("forexButton");
  forexPairButton -> setMaximumWidth(90);
  forexPairButton -> setMaxVisibleItems(20);
  forexPairButton -> addItems(commands);
  // Align items text to center.
  for (int idx = 0; idx < forexPairButton -> count() ; ++idx) {
    forexPairButton -> setItemData(idx, Qt::AlignCenter, Qt::TextAlignmentRole);
  }
}


void ChartTopMenu::setResolutionButton() {
  QStringList commands = getGranularitiesList();
  resolutionButton = new QComboBox(this);
  resolutionButton -> setMaxVisibleItems(10);
  resolutionButton -> view() -> setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  resolutionButton -> setStyle(QStyleFactory::create("Windows"));
  resolutionButton -> setObjectName("resolutionButton");
  resolutionButton -> addItems(commands);
  // Align items text to center.
  for (int idx = 0; idx < resolutionButton -> count() ; ++idx) {
    resolutionButton -> setItemData(idx, Qt::AlignCenter, Qt::TextAlignmentRole);
  }
}

void ChartTopMenu::setChartTypeButton() {
  QStringList commands = {"Candlestick", "Line", "OHLC", "Area"};
  chartTypeButton = new QComboBox(this);
  chartTypeButton -> setStyle(QStyleFactory::create("Windows"));
  chartTypeButton -> setObjectName("chartTypeButton");
  chartTypeButton -> setMaximumWidth(90);
  chartTypeButton -> addItems(commands);
  // Align items text to center.
  for (int idx = 0; idx < chartTypeButton -> count() ; ++idx) {
    chartTypeButton -> setItemData(idx, Qt::AlignCenter, Qt::TextAlignmentRole);
  }
}

void ChartTopMenu::setStudiesButton() {
  studiesButton = new QPushButton("&Studies", this);
  studiesButton -> setObjectName("studiesButton");
}
