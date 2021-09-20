
#include "mainwindow.h"
#include "src/streamdata.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setObjectName("mainWindow");
	//orderWidget = new OrderWidget(this);
	chartWidget = new ChartWidget(this, "USD_SEK", "M5", orderWidget);
	QThread::msleep(500);
	fxWidget = new FxWidget(this);
	createLayout();
	initStyle();
}

void MainWindow::createLayout() {
  resize(1600, 900);
	QHBoxLayout *layout = new QHBoxLayout;
  layout -> setContentsMargins (5, 5, 5, 5);
  layout -> setSpacing(10);
	layout -> addWidget(fxWidget, 18);

	QVBoxLayout *chartLayout = new QVBoxLayout();
	chartLayout -> addWidget(chartWidget);
	layout -> addLayout(chartLayout, 70);

	//layout -> addWidget(orderWidget, 12);
	mainWidget = new QWidget();
	mainWidget -> setLayout(layout);
	this -> setCentralWidget(mainWidget);
}

void MainWindow::initStyle() {
  QFile file("src/stylesheets/mainWindow.qss");
  file.open(QFile::ReadOnly);
  QString stylesheet = QLatin1String(file.readAll());
  setStyleSheet(stylesheet);
}