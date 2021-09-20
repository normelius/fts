

#include "customWidgets.h"

CustomInputWidget::CustomInputWidget(QWidget *parent, const QString &text, const QString &type) : QWidget(parent) {
  this -> setMaximumWidth(85);
  QLabel *label = new QLabel(text, this);
  input = new QLineEdit(this);
  input -> setValidator(new QIntValidator(1, 400, this));

  QVBoxLayout *layout = new QVBoxLayout();
  layout -> setContentsMargins(0, 0, 0, 0);
  layout -> setSpacing(0);
  layout -> addWidget(label);
  layout -> addWidget(input);
  this -> setLayout(layout);

  if (type == "int") {
    input -> setValidator(new QIntValidator(1, 400, this));
  }

  else {
    input -> setValidator(new QDoubleValidator(0.0, 20.0, 2, this));
  }

}

void CustomInputWidget::setInputText(const QString &text) {
  input -> setText(text);
}

int CustomInputWidget::getInputInt() {
  return (input -> text()).toInt();
}

double CustomInputWidget::getInputDouble() {
  return (input -> text()).toDouble();
}


// CustomColorWidget
CustomColorWidget::CustomColorWidget(QWidget *parent, const QString &text) : QWidget(parent) {
  setMaximumWidth(100);
  colorButton = new QPushButton("", parent);
  colorDialog = new QColorDialog(this);
  colorDialog -> setOptions(QColorDialog::NoButtons | QColorDialog::DontUseNativeDialog); 
  colorDialog -> setCurrentColor(QColor("#1FF1DE"));

  QVBoxLayout *layout = new QVBoxLayout();
  layout -> setContentsMargins(0, 0, 0, 0);
  layout -> setSpacing(0);
  layout -> addWidget(new QLabel(text, this));
  layout -> addWidget(colorButton);
  setLayout(layout);

  connect(colorButton, &QPushButton::clicked, this, &CustomColorWidget::handleClicked);
  connect(colorDialog, &QColorDialog::currentColorChanged, this, &CustomColorWidget::colorChanged);
}

void CustomColorWidget::colorChanged(const QColor &color) {
  QPalette pal = colorButton -> palette();
  pal.setColor(QPalette::Button, color);
  colorButton -> setAutoFillBackground(true);
  colorButton -> setPalette(pal);
  colorButton -> update();
}

QColor CustomColorWidget::getCurrentColor() {
  return colorDialog -> currentColor();
}


// CustomSourceWidget
CustomSourceWidget::CustomSourceWidget(QWidget *parent) : QWidget(parent) {
  QLabel *label = new QLabel("Sources", this);

  QStringList sources = {"High", "Low", "Open", "Close", "HL2", "HLC3", "OHLC4", "HL2C4"};
  combo = new QComboBox();
  combo -> view() -> setMinimumHeight(80);
  combo -> setObjectName("customSourceCombo");
  combo -> view() -> setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  combo -> setStyle(QStyleFactory::create("Windows"));
  combo -> addItems(sources);
  // Align items text to center.
  for (int idx = 0; idx < combo -> count() ; ++idx) {
    combo -> setItemData(idx, Qt::AlignCenter, Qt::TextAlignmentRole);
  }

  combo -> setCurrentIndex(combo -> findText("Close"));

  QVBoxLayout *layout = new QVBoxLayout();
  layout -> setContentsMargins(0, 0, 0, 0);
  layout -> setSpacing(0);
  layout -> addWidget(label);
  layout -> addWidget(combo);
  setLayout(layout);
}