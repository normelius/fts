
/*
#ifndef ACTIVE_WIDGET_H
#define ACTIVE_WIDGET_H

#include <variant>
#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QDebug>
#include <QHeaderView>
#include <memory>
#include "third-party/qcustomplot.h"
#include "src/streamdata.h"
#include "src/plotWidget/plotWidget.h"
#include "src/plotWidget/indicatorWidget/trend.h"
#include "src/plotWidget/indicatorWidget/momentumWidget.h"


// Forward declaration of the study containers.
class SmaIndicator;
class EmaIndicator;
class MacdIndicator;

class ActiveWidget : public QWidget {
  Q_OBJECT

  public:
    ActiveWidget(QWidget *parent = 0, PlotWidget *chart = NULL);
    QTreeWidget *tree;
    // Here the bulk of the operations to add a new study is made.
    // How do we add a new indicator?
    // 1. Forward declare it above.
    // 2. Add the extern template in the bottom of activeWidget.cc
    template <typename T>
    void onAddedStudy(T indicatorContainer, const bool isOverlay = true);
    void updateStudies(QCPDataContainer<QCPFinancialData> *data, const bool newPoint = false);
    // When we are in need of replacing all of the studies, i.e., the data.
    void replaceStudies(QCPDataContainer<QCPFinancialData> *data);

  private:
    PlotWidget *chart;
    std::variant<MacdIndicator*> studyContainer;
    std::vector<std::variant<SmaIndicator*, EmaIndicator*, MacdIndicator*>> studyContainers;
    std::vector<SubPlot*> subplots;

  signals:
    void newOverlayGraph();

  public slots:
    void onRemovedStudy(QTreeWidgetItem *item);
};

#endif
*/