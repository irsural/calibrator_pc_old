#include "deviceparametersform.h"
#include "ui_deviceparametersform.h"

#include <QStyledItemDelegate>



#include <irsfinal.h>

class NoEditDelegate: public QStyledItemDelegate {
public:
  NoEditDelegate(QObject* parent=0): QStyledItemDelegate(parent) {}
  virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return 0;
  }
};

DeviceParametersForm::DeviceParametersForm(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::DeviceParametersForm)
{
  ui->setupUi(this);

  model = new ParamsTreeModel(this);
  ui->paramsTreeView->setModel(model);
  ui->paramsTreeView->setAlternatingRowColors(true);
  //ui->paramsTreeView->setRootIsDecorated(false);
  //qDebug() << "indentation = " << ui->paramsTreeView->indentation();
  //ui->paramsTreeView->set
  //ui->paramsTreeView->setIndentation(15);

  QModelIndex parentIndex = ui->paramsTreeView->rootIndex();
  for (int row = 0; row < model->rowCount(parentIndex); row++) {
    ui->paramsTreeView->setFirstColumnSpanned(row, parentIndex, true);
  }
  ui->paramsTreeView->header()->setSectionResizeMode(
    QHeaderView::ResizeToContents);
}

DeviceParametersForm::~DeviceParametersForm()
{
  delete ui;
}

void DeviceParametersForm::setSetpoint(double setpoint)
{
  /*if (markValue->isValid()) {
    const double markV = markValue->value();
    absoluteError->setValue(setpoint - markV);
    if (setpoint != 0) {
      relativeError->setValue(std::abs(setpoint - markV)/setpoint*100);
    } else {
      relativeError->setValue(0);
    }
    if (normalizingValue->isValid()) {
      const double normV = normalizingValue->value();
      reducedError->setValue((std::abs(setpoint - markV)/normV)*100);
    }
  }*/
}

void DeviceParametersForm::updateIcons()
{

}

void DeviceParametersForm::on_analogDevicePushButton_clicked(bool checked)
{
  // Запрещаем отжатие кнопки
  if (!checked) {
    ui->analogDevicePushButton->setChecked(true);
    return;
  }
  ui->digitalDevicePushButton->setChecked(false);

  model->setDeviceType(device_type_analog);
  //generalItem->setHidden(false);
}

void DeviceParametersForm::on_digitalDevicePushButton_clicked(bool checked)
{
  // Запрещаем отжатие кнопки
  if (!checked) {
    ui->digitalDevicePushButton->setChecked(true);
    return;
  }
  ui->analogDevicePushButton->setChecked(false);

  model->setDeviceType(device_type_digital);
  //generalItem->setHidden(true);
}

void DeviceParametersForm::on_paramsTreeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
  /*if (item == range->item()) {
    if (!maxMark->isValid() || ) {

    }
  }*/

  QString s = item->text(1);
  double d = s.toDouble();
  if (d > 100) {
    QString v = QString::number(100);
    item->setText(1, v);
  }
}
