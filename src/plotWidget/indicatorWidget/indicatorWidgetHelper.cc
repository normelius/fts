

#include "indicatorWidgetHelper.h"

void setGraphData(QCPGraph *graph, const QVector<double> &data) {
  QVector<double> axis(data.size());
  std::iota(std::begin(axis), std::end(axis), 0);
  graph -> setData(axis, data);
}

void updateGraphData(QCPGraph *graph, const double newData, const bool newPoint) {
  if (newPoint == true) {
    graph -> addData(graph -> dataCount(), newData);
  }

  else {
    QSharedPointer<QCPGraphDataContainer> dataPointer = graph -> data();
    QCPDataContainer<QCPGraphData> *container = dataPointer.data(); 
    (container -> end() - 1) -> value = newData;
  }
}