#ifndef DEVICEPARAMETERSFORM_H
#define DEVICEPARAMETERSFORM_H

#include <QWidget>

#include <QTreeWidget>
#include <QDebug>

#include "calibrator.h"

#include "ParamsTreeModel.h"

namespace Ui {
  class DeviceParametersForm;
}

class Parameter2
{
public:
  Parameter2(QTreeWidgetItem* aItem, int aColumn = 1,
  int aPrecision = out_precision
  ):
    item(aItem),
    column(aColumn),
    precision(aPrecision)
  {
  }
  QTreeWidgetItem* getItem() const
  {
    return item;
  }
  void setMinMax(double aMinValue, double aMaxValue)
  {
    minValue = aMinValue;
    maxValue = aMaxValue;
  }

  bool isValid() const
  {
    QString s = item->text(column);

    bool ok = false;
    double value = s.toDouble(&ok);
    if (!ok) {
      return false;
    }
    if (minValue != maxValue) {
      return irs::is_in_range(value, minValue, maxValue);
    }
    return ok;
  }

  Parameter2& operator=(double value)
  {
    /*double v = value;
    if (minValue != maxValue) {
      v = irs::range(v, minValue, maxValue);
    }*/
    setValue(value);
    return *this;
  }
  operator double() const
  {
    return value();
  }
  void setValue(double value)
  {
    item->setText(column, QString("%L1").arg(value, 0, 'g', precision));
  }
  double value() const
  {
    QString s = item->text(column);

    bool ok = false;
    double value = s.toDouble(&ok);
    if (!ok) {
      value = 0;
    }
    if (minValue != maxValue) {
      value = irs::range(value, minValue, maxValue);
    }
    return value;
  }
private:
  QTreeWidgetItem* item;
  int column = 1;
  int precision = out_precision;
  double minValue = 0;
  double maxValue = 0;
};

class DeviceParametersForm : public QWidget
{
  Q_OBJECT

public:
  explicit DeviceParametersForm(QWidget *parent = 0);
  ~DeviceParametersForm();
  void setSetpoint(double setpoint);
private slots:

  void on_analogDevicePushButton_clicked(bool checked);

  void on_digitalDevicePushButton_clicked(bool checked);

  void on_paramsTreeWidget_itemChanged(QTreeWidgetItem *item, int column);

private:
  void updateIcons();
  Ui::DeviceParametersForm *ui;
  ParamsTreeModel* model;
};

#endif // DEVICEPARAMETERSFORM_H
