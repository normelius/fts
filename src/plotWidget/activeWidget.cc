
/*

#include "activeWidget.h"

ActiveWidget::ActiveWidget(QWidget *parent, PlotWidget *chart) : QWidget(parent) {
  this -> chart = chart;
  setObjectName("activeWidget");
  hide();
  QVBoxLayout *layout = new QVBoxLayout();

  tree = new QTreeWidget(this);
  tree -> setMinimumWidth(400);
  tree -> header() -> resizeSection(0, 150);
  tree -> header() -> resizeSection(1, 250);
  tree -> header() -> resizeSection(2, 50);
  tree -> header() -> setStretchLastSection(false);
  tree -> resizeColumnToContents(3);
  tree -> setObjectName("activeWidgetTree");
  tree -> setHeaderHidden(true);
  tree -> setColumnCount(3);
  layout -> addWidget(tree);
  setLayout(layout);
}

void ActiveWidget::onRemovedStudy(QTreeWidgetItem *item) {
  bool correctItem = false;
  QCPGraph *removeGraph;
  int index = 0;
  for (auto &container : studyContainers) {
    // See if we have found correct item, i.e., QTreeWidgetItem.
    std::visit([&](auto study) {correctItem = study -> correctStudy(item);}, container);

    if (correctItem) {
      // Get the graph so we can clear it.
      std::visit([&](auto study) {removeGraph = study -> graph;}, container);
      chart -> removeGraph(removeGraph);
      // Manually call the destructor where we delete this, in order to remove the object.
      std::visit([&](auto study) {study -> callDestructor();}, container);
      studyContainers.erase(studyContainers.begin() + index);
      break;
    }

    ++index;
    assert(correctItem == false);
  }

  chart -> replotChart();
}

template <typename T>
void ActiveWidget::onAddedStudy(T indicatorContainer, const bool isOverlay) {
  if (isOverlay == true) {
    // Create a new chart and store it in the indicatorContainer.
    QCPGraph *graph = chart -> addGraph();
    graph -> setKeyAxis(chart -> xAxis);
    graph -> setValueAxis(chart -> yAxis2);
    indicatorContainer -> newGraph(graph, chart -> getPointerData());
  }

  else {
    SubPlot *subplot = new SubPlot(chart);
    subplots.push_back(subplot);
    indicatorContainer -> newSubplot(subplot, chart -> getPointerData());
    // Necessary in order to fit the newly added subplot correctly. Otherwise data won't
    // be visible until mouseMoveEvent is triggered.
    subplot -> rescaleGraph();
  }

  // Save it to the variant so we can store it in a vector.
  studyContainer = indicatorContainer;
  studyContainers.push_back(studyContainer);

  chart -> replotChart();
}

void ActiveWidget::updateStudies(QCPDataContainer<QCPFinancialData> *data, const bool newPoint) {
  for (auto &container : studyContainers) {
    std::visit([&](auto study) {study -> updateData(data, newPoint);}, container);
  }
}

void ActiveWidget::replaceStudies(QCPDataContainer<QCPFinancialData> *data) {
  for (auto &container : studyContainers) {
    std::visit([&](auto study) {study -> replaceData(data);}, container);
  }

  // Necessary in order to immediately get correct view of the new subplots. Otherwise
  // data won't rescale correctly before we move the mouse.
  for (auto subplot : subplots) {
    subplot -> rescaleGraph();
  }
}


// Remember these when creating new indicator containers.
template void ActiveWidget::onAddedStudy<MacdIndicator*>(MacdIndicator*, const bool);
*/