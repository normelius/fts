


#include "customFxWidget.h"


CustomFxWidget::CustomFxWidget(QWidget *parent) : QTableWidget(parent) {
  availablePairs = getAvailablePairs();
  pairs = getAvaiablePairsList();

  QColor upColor = QColor("#06FEC9");
  QColor downColor = QColor("#FE6767");
  upColor.setAlpha(35);
  downColor.setAlpha(35);
  blinkUpColor = QBrush(upColor);
  blinkDownColor = QBrush(downColor);

  setRowCount(pairs.length());
  setColumnCount(4);
  verticalHeader() -> setSectionResizeMode(QHeaderView::Fixed);
  verticalHeader() -> setDefaultSectionSize(20);

  // So that last column stretches in order to get a nice appearence.
  //horizontalHeader()->setStretchLastSection(true);
  setHorizontalHeaderItem(0, new QTableWidgetItem("Pair"));
  setHorizontalHeaderItem(1, new QTableWidgetItem("Latest"));
  setHorizontalHeaderItem(2, new QTableWidgetItem("Bid"));
  setHorizontalHeaderItem(3, new QTableWidgetItem("Ask"));
  // Remove vertical row numbering.
  verticalHeader() -> setVisible(false);
  horizontalHeader() -> setSectionResizeMode(QHeaderView::Stretch);
  horizontalHeaderItem(0) -> setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  horizontalHeaderItem(1) -> setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  horizontalHeaderItem(2) -> setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  horizontalHeaderItem(3) -> setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  // Customize gridlines in qss file instead with the help of borders.
  setShowGrid(true);
  // Remove possibility to edit a cell.
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  // When pressing a cell, whole row is highlighted.
  setSelectionBehavior(QAbstractItemView::SelectRows);
  initFxNames();
  initDataStream(availablePairs);
}

CustomFxWidget::~CustomFxWidget() {
	streamDataWorkerThread -> requestInterruption();
	streamDataWorkerThread -> quit();
	streamDataWorkerThread -> wait();
}

void CustomFxWidget::initFxNames() {
  //QStringList pairs = {"USD/JPY", "EUR/USD", "USD/SEK", "EUR/SEK", "EUR/DKK", "EUR/CAD", "EUR/HKD"};
  QFont cellFont;
  cellFont.setPointSize(12);
  for (int idx = 0; idx < pairs.length(); ++idx) {
    QTableWidgetItem *instrument = new QTableWidgetItem(pairs[idx]);
    instrument -> setFont(cellFont);
    QTableWidgetItem *latestPrice = new QTableWidgetItem("");
    latestPrice -> setFont(cellFont);
    QTableWidgetItem *asksPrice = new QTableWidgetItem("");
    asksPrice -> setFont(cellFont);
    QTableWidgetItem *bidsPrice = new QTableWidgetItem("");
    bidsPrice -> setFont(cellFont);
    setItem(idx, 0, instrument);
    setItem(idx, 1, latestPrice);
    setItem(idx, 2, asksPrice);
    setItem(idx, 3, bidsPrice);
    QString corrPairName = pairs[idx].replace("/", "_");
    pairsIndex[corrPairName] = idx;
  }
}

void CustomFxWidget::initDataStream(const QString instrument) {
  streamDataWorker = new StreamDataWorker();
	streamDataWorker -> moveToThread(streamDataWorkerThread);
	connect(streamDataWorkerThread, &QThread::finished, streamDataWorker, &QObject::deleteLater);
  connect(this, &CustomFxWidget::startDataStream, streamDataWorker, &StreamDataWorker::startDataStream);
	qRegisterMetaType<StreamData>("StreamData");
	connect(streamDataWorker, &StreamDataWorker::emitStreamData, this, &CustomFxWidget::receiveStreamData);
	streamDataWorkerThread -> start();
	startDataStream(instrument);
}

void CustomFxWidget::receiveStreamData(StreamData streamData) {
  QString receivingInstrument = streamData.instrument; 
  int rowIndex = pairsIndex[receivingInstrument];
  QString latestPrice = QString::number(streamData.closePrice);
  QString asksPrice = QString::number(streamData.asksPrice);
  QString bidsPrice = QString::number(streamData.bidsPrice);

  if (isPriceIncrease(streamData.closePrice, rowIndex))
    item(rowIndex, 1) -> setBackground(blinkUpColor);

  else
    item(rowIndex, 1) -> setBackground(blinkDownColor);

  QTimer::singleShot(400, this, std::bind(&CustomFxWidget::blinkSlot, this, rowIndex));
  item(rowIndex, 1) -> setText(latestPrice);
  item(rowIndex, 2) -> setText(asksPrice);
  item(rowIndex, 3) -> setText(bidsPrice);
}

bool CustomFxWidget::isPriceIncrease(double closePrice, int rowIndex) {
  QTableWidgetItem *latestPriceItem = item(rowIndex, 1);
  QString latestPriceStr = latestPriceItem -> text();
  double latest = latestPriceStr.toDouble();
  if (closePrice >= latest)
    return true;

  return false;
}

void CustomFxWidget::blinkSlot(int rowIndex) {
  item(rowIndex, 1) -> setBackground(QBrush(QColor("#202226")));
}