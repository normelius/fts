
#ifndef UTILITIES_H
#define UTILITIES_H

#include "third-party/qcustomplot.h"
#include <QWidget>
#include <QPushButton>
#include <QFile>
#include <QTreeWidgetItem> 
#include <QColorDialog>

/*
Helper function to retrieve the raw data from a QCPDataContainer.
We can specify source e.g., open, high, low, close. Default is "Close".
*/
QVector<double> getRawData(QCPDataContainer<QCPFinancialData> *data, const QString &source = "Close");

// Horizontal separator used in widgets. 
class HSeparator : public QWidget {
  public:
    HSeparator(QWidget *parent = 0, const int height = 1, const QString color = "#545A68");
};

/*
Button used in ActiveStudies for closing studies.
We create custom signals and slots so we can send the root 
QTreeWidgetItem which then is deleted. The signal clicked() is 
connected to handleClick(), which then emits a signal removeStudy()
which we can hook up to when creating the button. 
*/
class CloseButton : public QPushButton {
  Q_OBJECT
  public:
    CloseButton(const QString &text, QWidget *parent = nullptr, QTreeWidgetItem *item = nullptr);
    QTreeWidgetItem *rootItem;

  signals:
    void removeStudy(QTreeWidgetItem *item);

  private slots:
    void handleClicked() {
      emit removeStudy(rootItem);
    };
};



#endif