
#ifndef CUSTOM_WIDGETS_H
#define CUSTOM_WIDGETS_H

#include <QDebug>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QString>
#include <QVBoxLayout>
#include <QPushButton>
#include <QColorDialog>
#include <QComboBox>
#include <QStyleFactory>
#include <QAbstractItemView>
#include <QFile>
#include <QListView>

/*
Custom input widget that contains a header label and a input box.
*/
class CustomInputWidget : public QWidget {
  Q_OBJECT
  public:
    CustomInputWidget(QWidget *parent = nullptr, const QString &text = "", const QString &type = "int");
    void setInputText(const QString &text = "");
    int getInputInt();
    double getInputDouble();

    QLineEdit *input;
};


/*
Custom widget containing header label and button that trigger a QColorDialog.
*/
class CustomColorWidget : public QWidget {
  Q_OBJECT
  public:
    CustomColorWidget(QWidget *parent = nullptr, const QString &text = "");
    QColor getCurrentColor();

    QPushButton *colorButton;
    QColorDialog *colorDialog;
  
  private slots:
    void handleClicked() {
      colorDialog -> show();
    }
    void colorChanged(const QColor &color);
};

/*
Custom widget that serves as a dropdown menu for choosing source, i.e.,
specifying what the data should comprise of, e.g., close, high, low, median etc.
*/
class CustomSourceWidget : public QWidget {
  Q_OBJECT
  public:
    CustomSourceWidget(QWidget *parent = nullptr);
    QComboBox *combo;
};


#endif
