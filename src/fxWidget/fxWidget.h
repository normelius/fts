

#ifndef FX_WIDGET_H
#define FX_WIDGET_H

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "customFxWidget.h"

class FxWidget : public QWidget {
  Q_OBJECT

  public:
    FxWidget(QWidget *parent = 0);

  private:
    void setHeaderWidgets();
    void initStyle();
    CustomFxWidget *customFxWidget;
    QWidget *header;

};



#endif


