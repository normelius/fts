
#ifndef STREAMDATA_H
#define STREAMDATA_H

#include <QtDebug>
#include <QThread>


struct StreamData {
  QString type;
  // Unix timestamp.
  double time;
  // Resolution count is used to see when a new bar is needed.
  int resolutionCount;
  bool validData = false;
  int asksLiquidity = 0;
  double asksPrice = 0.0;
  int bidsLiquidity = 0;
  double bidsPrice = 0.0;
  double closePrice = 0.0;
  double closeoutBid = 0.0;
  double closeoutAsk = 0.0;
  bool tradeable;
  QString instrument = "";
  QString status = "";
};

class StreamDataWorker : public QObject {
  Q_OBJECT
  public:
    StreamDataWorker();

  private:
    void streamBuffer(std::streambuf *stream_buffer);
    int chartResolution;

  public slots:
    void startDataStream(const QString instrument, const int chartResolution = 300);

  signals:
    void emitStreamData(StreamData streamData);
};


#endif