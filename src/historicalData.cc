
#include <string>

#include "historicalData.h"
#include "getData.h"

#include <nlohmann/json.hpp>
#include "third-party/httplib.h"

using json = nlohmann::json;


FinancialData::FinancialData(double high, double low, double open, double close,
  double volume, double time, int resolutionCount) {
  this -> high = high;
  this -> low = low;
  this -> open = open;
  this -> close = close;
  this -> volume = volume;
  this -> time = time;
  this -> resolutionCount = resolutionCount;
}

void FinancialDataContainer::add(FinancialData data) {
  container.append(data);
}

QVector<double> FinancialDataContainer::getTimeVector() {
  QVector<double> time;
  for (auto it = std::begin(container); it != std::end(container); ++it)
    time.append(it -> time);

  return time;
}

QVector<double> FinancialDataContainer::getHighVector() {
  QVector<double> high;
  for (auto it = std::begin(container); it != std::end(container); ++it)
    high.append(it -> high);

  return high;
}

QVector<double> FinancialDataContainer::getLowVector() {
  QVector<double> low;
  for (auto it = std::begin(container); it != std::end(container); ++it)
    low.append(it -> low);

  return low;
}

QVector<double> FinancialDataContainer::getOpenVector() {
  QVector<double> open;
  for (auto it = std::begin(container); it != std::end(container); ++it)
    open.append(it -> open);

  return open;
}

QVector<double> FinancialDataContainer::getCloseVector() {
  QVector<double> close;
  for (auto it = std::begin(container); it != std::end(container); ++it)
    close.append(it -> close);

  return close;
}

QVector<double> FinancialDataContainer::getVolumeVector() {
  QVector<double> volume;
  for (auto it = std::begin(container); it != std::end(container); ++it) 
    volume.append(it -> volume);

  return volume;
}

double FinancialDataContainer::getTimeValue(const int xAxisIndex) {
  if (container.size() > xAxisIndex && xAxisIndex >= 0)
    return container[xAxisIndex].time;

  return 0.0;
}

int FinancialDataContainer::getResolutionCount() {
  return container[size() - 1].resolutionCount;
}

int FinancialDataContainer::size() {
  return container.size();
}



/* 
---------------------------------------
*/

HistoricalDataWorker::HistoricalDataWorker() {
};

void HistoricalDataWorker::startDataRetrieval(const QString instrument, const QString granularity) {
  FinancialDataContainer container;

  //std::string granularity = "M5";
  // Number of datapoints, this is the maximum.
  std::string count = "5000";

  httplib::Headers header {
    {"Accept-Datetime-Format", "UNIX"}
  };

  std::string endpoint = "/v3/instruments/" + instrument.toStdString() + "/candles?";
  endpoint += "granularity=" + granularity.toStdString() + "&count=" + count;
  endpoint += "&Accept-Datetime-Format=UNIX";
  endpoint += "&smooth=true";

	json parsed = getData(endpoint.c_str(), header)["candles"];

  for (const auto& item : parsed.items()) {
    json point = item.value();
    double volume = point["volume"];
    double high = std::stod(point["mid"]["h"].get<std::string>());
    double low = std::stod(point["mid"]["l"].get<std::string>());
    double open = std::stod(point["mid"]["o"].get<std::string>());
    double close = std::stod(point["mid"]["c"].get<std::string>());
    double time = std::stod(point["time"].get<std::string>());

    int resolutionCount = time / getResolutionFromGranularity(granularity);
    FinancialData financialData(high, low, open, close, volume, time, resolutionCount);
    container.add(financialData);
  }

  emit emitHistoricalData(container);
}

