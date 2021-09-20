

#include "fxWidget.h"

FxWidget::FxWidget(QWidget *parent) : QWidget(parent) {
  header = new QWidget(this);
  header -> setObjectName("fxWidgetHeader");
  customFxWidget = new CustomFxWidget(this);
  QVBoxLayout *layout = new QVBoxLayout;
  layout -> setContentsMargins(0, 0, 0, 0);
  layout -> setSpacing(0);
  layout -> addWidget(header);
	layout -> addWidget(customFxWidget);
  setLayout(layout);
  setHeaderWidgets();
  initStyle();
}

void FxWidget::initStyle() {
  QFile file("src/stylesheets/fxWidget.qss");
  file.open(QFile::ReadOnly);
  QString stylesheet = QLatin1String(file.readAll());
  setStyleSheet(stylesheet);
}

void FxWidget::setHeaderWidgets() {
  QLabel *label = new QLabel("Available instruments");
  label -> setObjectName("fxWidgetHeaderLabel");
  QHBoxLayout *headerLayout = new QHBoxLayout();
  headerLayout -> addWidget(label);
  header -> setLayout(headerLayout);
}


