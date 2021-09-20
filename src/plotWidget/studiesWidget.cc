

#include <QFontDatabase>
#include <cassert>

// uncomment to disable assert()
// #define NDEBUG
 
// Use (void) to silent unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))

#include "studiesWidget.h"

StudiesWidget::StudiesWidget(QWidget *parent, PlotWidget *chart) : QDialog(parent) { 
  initWidgetSize(parent);

  this -> chart = chart;

  QFile file("src/stylesheets/studiesWidget.qss");
  file.open(QFile::ReadOnly);
  QString stylesheet = QLatin1String(file.readAll());
  file.close();
  setStyleSheet(stylesheet);

  tree = new QTreeWidget(this);
  tree -> setObjectName("studiesTree");
  tree -> setHeaderHidden(true);
  tree -> setMaximumWidth(350);
  tree -> setColumnCount(2);
  tree -> header() -> resizeSection(0, 300);
  tree -> header() -> resizeSection(1, 50);

  // Set layout for the studieswidget.
  layout = new QHBoxLayout();
  layout -> setContentsMargins (0, 0, 0, 0);
  layout -> setSpacing(0);
  layout -> addWidget(tree);
  layout -> setAlignment(Qt::AlignLeft);
  setLayout(layout);

  indicatorNames = readIndicators("src/resources/indicatorNames.txt");
  indicatorAcronyms = readIndicators("src/resources/indicatorAcronyms.txt");

  // Add the root items.
  for (int idx = 0; idx < rootNames.length(); ++idx) {
    QTreeWidgetItem *item = new QTreeWidgetItem(tree, QStringList(QString(rootNames[idx])));
    item -> setExpanded(true);
    rootItems.append(item);
  }

  // Add the QTreeWidgetItems corresponding to the all indicators.
  for (int idx = 0; idx < indicatorNames.length(); ++idx) {
    QTreeWidgetItem *item = new QTreeWidgetItem(rootItems[1], QStringList(QString(indicatorNames[idx])));
    item -> setData(0, Qt::UserRole, indicatorAcronyms[idx]); 
    items.append(item);
  }

  // We jump between widget pages in the studies.
  connect(tree, &QTreeWidget::itemClicked, this, &StudiesWidget::onItemClicked);
}

QStringList StudiesWidget::readIndicators(const QString &path) {
  QStringList out;
  QFile file(path);

  if (file.open(QIODevice::ReadOnly)) {
    QTextStream in(&file);

    while (!in.atEnd()) {
      QString name =  in.readLine().trimmed();
      out.append(name);
    }
  }

  file.close();

  return out;
}

bool StudiesWidget::pressedActiveWidget(QTreeWidgetItem *item) {
  return true ? (activeWidgets.count(item) > 0) : false;
}

// The layout only contains one widget, meaning if more exist one active widget exist.
bool StudiesWidget::activeWidgetExist() {
  return true ? layout -> count() > 1 : false;
}

// The layout will maximum contain two widgets. The second one will be
// the active one, meaning we can remove it.
void StudiesWidget::deleteActiveWidget() {
  layout -> takeAt(1) -> widget() -> hide();
  delete layout -> takeAt(1);
}

void StudiesWidget::onItemClicked(QTreeWidgetItem *item, int) {

  // We press one of the active widgets.
  if (pressedActiveWidget(item)) {
    if (activeWidgetExist()) 
      deleteActiveWidget();

    // Then add the newly pressed.
    std::visit([&](auto w) {w -> addToLayout(layout);}, activeWidgets[item]);
  }

  // We press one of the indicators.
  else if (items.count(item) > 0) {

    QTreeWidgetItem *newWidgetItem = new QTreeWidgetItem(rootItems[0]);
    // Store reference to which we pressed, e.g., SMA, EMA...
    newWidgetItem -> setData(0, Qt::UserRole, item -> data(0, Qt::UserRole));

    std::variant<STUDY_WIDGETS> widget = getNewStudyWidget(this, chart, newWidgetItem);

    if (activeWidgetExist()) {
      deleteActiveWidget();
    }

    activeWidgets[newWidgetItem] = widget;

    // We add the widget to the layout and also showing it by using internal function.
    std::visit([&](auto widgetReference) {widgetReference -> addToLayout(layout);}, widget);

    // We store a reference of this newly created item in the study, since we need it when removing it.
    std::visit([&](auto widgetReference) {widgetReference -> treeItem = newWidgetItem;}, widget);

    // Hook up connection so we can remove the study. Keep reference to the item so we can remove it.
    CloseButton *closeButton = new CloseButton("X", this, newWidgetItem);
    connect(closeButton, &CloseButton::removeStudy, this, &StudiesWidget::onRemovedStudy);
    tree -> setItemWidget(newWidgetItem, 1, closeButton);
    tree -> setCurrentItem(newWidgetItem);

    // Check whether the widget is an overlay, i.e., graph on top of main chart or a separate subplot.
    bool isOverlay = std::visit([] (const bool &isOverlay) {return isOverlay;}, widget);

    if (isOverlay == true) {
      std::visit([&](auto w) {w -> initStudy();}, widget);
    }

    else {
    }

    chart -> replotChart();
  }
}


void StudiesWidget::updateStudies(QCPDataContainer<QCPFinancialData> *data, const bool newPoint) {
  for (auto const& [key, widget] : activeWidgets) {
    std::visit([&](auto study) {study -> updateData(data, newPoint);}, widget);
  }
}

void StudiesWidget::replaceStudies(QCPDataContainer<QCPFinancialData> *data) {
  for (auto const& [key, widget] : activeWidgets) {
    std::visit([&](auto study) {study -> replaceData(data);}, widget);
  }

  // Necessary in order to immediately get correct view of the new subplots. Otherwise
  // data won't rescale correctly before we move the mouse.
  //for (auto subplot : subplots) {
    //subplot -> rescaleGraph();
  //}
}

void StudiesWidget::onRemovedStudy(QTreeWidgetItem *item) {
  bool correctItem = false;
  QCPGraph *removeGraph;
  QList<QCPGraph *> graphs;
  int index = 0;

  for (auto const& [key, widget] : activeWidgets) {

    if (item == key) {
      // Get the graph so we can clear it.
      // THIS WILL NEED TO BE REDONE, SINCE E.G., BOLLINGER BANDS WILL HAVE MULTIPLE GRAPHS.
      // Better to return a list with graphs.
      //std::visit([&](auto study) {removeGraph = study -> graph;}, widget);
      std::visit([&](auto study) {graphs = study -> graphs;}, widget);
      for (auto &graph : graphs) {
        chart -> removeGraph(graph);
      }

      // Manually call the destructor where we delete this, in order to remove the object.
      std::visit([&](auto study) {study -> callDestructor();}, widget);
      //activeOverlayStudies.erase(activeOverlayStudies.begin() + index);
      activeWidgets.erase(key);

      delete item;
      break;
    }
  }

  // When removing a study, we highlight the active studies again.
  tree -> setCurrentItem(rootItems[0]);

  chart -> replotChart();
}

void StudiesWidget::initWidgetSize(QWidget *parent) {
  if (parent != NULL)
    setGeometry(parent -> x() + parent -> width() / 2 - widgetWidth / 2,
      parent -> y() + parent -> height() / 2 - widgetHeight / 2,
      widgetWidth, widgetHeight);
  else
    resize(widgetWidth, widgetHeight);
}


std::variant<STUDY_WIDGETS> StudiesWidget::getNewStudyWidget(QWidget *parent, PlotWidget *chart, QTreeWidgetItem *newItem){
  QVariant data = newItem -> data(0, Qt::UserRole);
  const QString indicator = data.toString();

  std::variant<STUDY_WIDGETS> widget;

  if (indicator == "SMA")
    widget = new SmaWidget(this, chart, newItem);

  else if (indicator == "EMA")
    widget = new EmaWidget(this, chart, newItem);

  else if (indicator == "BB")
    widget = new BbWidget(this, chart, newItem);

  else
    widget = new EmaWidget(this, chart, newItem);

  return widget;
}