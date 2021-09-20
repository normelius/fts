
#ifndef ORDER_WIDGET_H
#define ORDER_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QLabel>

#include "src/streamdata.h"

/*
This serves as a widget for order, e.g., buy/sell market buttons,
input for number of contracts etc.
*/
class OrderWidget : public QWidget {

  public:
    OrderWidget(QWidget *parent = nullptr);
    void newData(StreamData streamData);

  private:
    QPushButton *buyMarketButton;
    QPushButton *sellMarketButton;
    QLineEdit *priceInput;
    QLabel *askPriceLabel;
    QLabel *bidPriceLabel;
};

#endif