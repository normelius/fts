
#ifndef BB_WIDGET_H 
#define BB_WIDGET_H 

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

#include "third-party/qcustomplot.h"

#include "src/utilities.h"
#include "src/plotWidget/indicatorWidget/indicatorWidgetHelper.h"
#include "src/plotWidget/activeWidget.h"
#include "src/plotWidget/plotWidget.h"
#include "src/streamdata.h"
#include "src/indicators/volatility.h"
#include "src/plotWidget/indicatorWidget/customWidgets.h"
#include "src/plotWidget/customLegend.h"

class BbWidget: public QWidget {
  Q_OBJECT

  public:
    BbWidget(QWidget *parent = 0, PlotWidget *chart = 0, QTreeWidgetItem *item = 0);
    ~BbWidget();
    void callDestructor() {this -> ~BbWidget();}
    // Used in StudiesWidget to display the widget with it's components.
    void addToLayout(QLayout *layout) {layout -> addWidget(this); this -> show();}
    void initStudy();
    void updateData(QCPDataContainer<QCPFinancialData> *data, const bool newPoint = false);
    void replaceData(QCPDataContainer<QCPFinancialData> *data);
    // Returns the name that should show up under the Active Studies tab.
    const QString getActiveName();

    const bool isOverlay = true;
    int period = 20;
    double stddev = 2.0;
    QTreeWidgetItem *treeItem;
    QCPGraph *graph;
    QCPGraph *graph_upper;
    QCPGraph *graph_lower;
    QList<QCPGraph *> graphs;

  private:
    PlotWidget *chart;
    CustomColorWidget *customColorWidget;
    CustomInputWidget *customPeriodWidget;
    CustomInputWidget *customDeviationWidget;
    CustomSourceWidget *customSourceWidget;
    CustomLegendItem *customLegendItem;
    QString source = "Close";

  public slots:
    void periodChanged();
    void stddevChanged();
    void colorChanged(const QColor &color);
    void sourceChanged(const QString &source);
};


#endif