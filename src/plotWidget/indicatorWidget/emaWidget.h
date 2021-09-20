
#ifndef EMA_WIDGET_H 
#define EMA_WIDGET_H 

#include <QDebug>
#include <QWidget>
#include <QString>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QObject>
#include <QTreeWidget>
#include <QColorDialog>

#include "src/utilities.h"
#include "src/plotWidget/indicatorWidget/indicatorWidgetHelper.h"
#include "src/plotWidget/activeWidget.h"
#include "src/plotWidget/plotWidget.h"
#include "third-party/qcustomplot.h"
#include "src/streamdata.h"
#include "src/indicators/trend.h"
#include "src/plotWidget/indicatorWidget/customWidgets.h"
#include "src/plotWidget/customLegend.h"

class EmaWidget : public QWidget {
  Q_OBJECT

  public:
    EmaWidget(QWidget *parent = 0, PlotWidget *chart = 0, QTreeWidgetItem *item = 0);
    ~EmaWidget();
    void callDestructor() {this -> ~EmaWidget();}
    // Used in StudiesWidget to display the widget with it's components.
    void addToLayout(QLayout *layout) {layout -> addWidget(this); this -> show();}
    void initStudy();
    void updateData(QCPDataContainer<QCPFinancialData> *data, const bool newPoint = false);
    void replaceData(QCPDataContainer<QCPFinancialData> *data);
    // Returns the name that should show up under the Active Studies tab.
    const QString getActiveName();

    const bool isOverlay = true;
    int period = 14;
    QTreeWidgetItem *treeItem;
    QCPGraph *graph;
    QList<QCPGraph *> graphs;

  private:
    PlotWidget *chart;
    CustomColorWidget *customColorWidget;
    CustomInputWidget *customPeriodWidget;
    CustomSourceWidget *customSourceWidget;
    CustomLegendItem *customLegendItem;
    QString source = "Close";

  public slots:
    void periodChanged();
    void colorChanged(const QColor &color);
    void sourceChanged(const QString &source);
};


#endif