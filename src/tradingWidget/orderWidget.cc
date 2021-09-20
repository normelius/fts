

#include "orderWidget.h"

OrderWidget::OrderWidget(QWidget *parent) : QWidget(parent) {
  this -> setObjectName("OrderWidget");
  this -> setMaximumHeight(200);

  askPriceLabel = new QLabel(this);
  askPriceLabel -> setText("1.32123");
  bidPriceLabel = new QLabel(this);
  bidPriceLabel -> setText("1.32123");
  QHBoxLayout *horLayout = new QHBoxLayout();
  horLayout -> addWidget(askPriceLabel);
  horLayout -> addWidget(bidPriceLabel);

  buyMarketButton = new QPushButton("Buy Market", this);
  buyMarketButton -> setObjectName("buyMarketButton");
  sellMarketButton = new QPushButton("Sell Market", this);
  sellMarketButton -> setObjectName("sellMarketButton");
  priceInput = new QLineEdit("", this);
  priceInput -> setObjectName("priceInput");

  QHBoxLayout *horizontalLayout = new QHBoxLayout();
  horizontalLayout -> addWidget(buyMarketButton);
  horizontalLayout -> addWidget(sellMarketButton);

  QVBoxLayout *verticalLayout = new QVBoxLayout();
  verticalLayout -> addLayout(horLayout);
  verticalLayout -> addLayout(horizontalLayout);
  verticalLayout -> addWidget(priceInput);

  this -> setLayout(verticalLayout);

  QFile file("src/stylesheets/orderWidget.qss");
  file.open(QFile::ReadOnly);
  QString stylesheet = QLatin1String(file.readAll());
  file.close();
  setStyleSheet(stylesheet);
}

void OrderWidget::newData(StreamData streamData) {
  askPriceLabel -> setText(QString::number(streamData.asksPrice));
  bidPriceLabel -> setText(QString::number(streamData.bidsPrice));
}