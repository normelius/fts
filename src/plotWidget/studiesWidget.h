
#ifndef STUDIESWIDGET_H
#define STUDIESWIDGET_H

#include <any>
#include <QDialog>
#include <QDebug>
#include <QWidget>
#include <QScrollArea>
#include <QTreeWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QProxyStyle>
#include <QStyleOption>
#include <QTabBar>
#include <QSize>
#include <QStyleFactory>
#include <QStylePainter>
#include <QHeaderView>
#include <variant>

#include "src/plotWidget/plotWidget.h"
#include "indicatorWidget/smaWidget.h"
#include "indicatorWidget/emaWidget.h"
#include "indicatorWidget/bbWidget.h"

/*
In order to add new studywwidgets to the program:
1. Declare them below in the STUDY_WIDGETS.
2. Add compatibility in the method getNewStudyWidget.
3. Add indicators to resorces/indicatorAcronyms.txt and resources/indicatorNames.txt.
4. Done.
*/
#define STUDY_WIDGETS SmaWidget*, EmaWidget*, BbWidget*

class StudiesWidget : public QDialog {
  Q_OBJECT

  public:
    StudiesWidget(QWidget *parent = 0, PlotWidget *chart = NULL);
    // Updates the active indicators with new data or creates new data point.
    void updateStudies(QCPDataContainer<QCPFinancialData> *data, const bool newPoint = false);
    // Replaces all of the data which is used when changing currency pair or 
    void replaceStudies(QCPDataContainer<QCPFinancialData> *data);

  private:
    // Methods
    void initWidgetSize(QWidget *parent);
    QStringList readIndicators(const QString &path);

    /* 
    This method looks through all available indicators and returns a widget with the correct one.
    We place this in it's own method since it will comprise of very many if-statements. Want to
    redo this in the future to something better.
    */
    std::variant<STUDY_WIDGETS> getNewStudyWidget(QWidget *parent = 0, PlotWidget *chart = 0, QTreeWidgetItem *item = 0);
    bool pressedActiveWidget(QTreeWidgetItem *item);
    // Checks if any active widget exist.
    bool activeWidgetExist();
    // Removes the active widget from the layout.
    void deleteActiveWidget();

    // Attributes 
    std::map<QTreeWidgetItem*, std::variant<STUDY_WIDGETS>> activeWidgets;
    QList<QTreeWidgetItem *> rootItems;
    QList<QTreeWidgetItem *> items;
    QStringList rootNames = {"Active studies", "All Studies", "Custom"};
    QStringList indicatorNames;
    QStringList indicatorAcronyms;
    PlotWidget *chart;
    QTreeWidget *tree;
    QHBoxLayout *layout;
    std::map<QTreeWidgetItem *, QWidget *> map;
    const int widgetWidth = 800;
    const int widgetHeight = 800;


  public slots:
    void onItemClicked(QTreeWidgetItem *item, int column);
    void onRemovedStudy(QTreeWidgetItem *item);
};





#endif