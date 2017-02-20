#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <irsfinal.h>

SettingsDialog::SettingsDialog(QWidget *parent, settings_t *ap_settings):
  QDialog(parent),
  ui(new Ui::SettingsDialog),
  settings(ap_settings)
{
  ui->setupUi(this);

  tuneCategoryListWidgets();

  connect(ui->okPushButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(ui->cancelPushButton, SIGNAL(clicked()), this, SLOT(close()));
}

void SettingsDialog::tuneCategoryListWidgets()
{
  QListWidgetItem* item = new QListWidgetItem(
    irs::str_conv<QString>(string_type(irst("Режим генератора"))),
    ui->categoryListWidget);
  item->setData(Qt::UserRole,
    ui->categoryStackedWidget->indexOf(ui->generatorModePage));

  /*item = new QListWidgetItem(
    irs::str_conv<QString>(string_type(irst("Интерфейс программы"))),
    ui->categoryListWidget);
  item->setData(Qt::UserRole,
    ui->categoryStackedWidget->indexOf(ui->programInterfacePage));

  item = new QListWidgetItem(
    QString(irs::str_conv<QString>(string_type(irst("USB HID")))),
    ui->categoryListWidget);
  item->setData(Qt::UserRole,
    ui->categoryStackedWidget->indexOf(ui->usbHIDPage));

  item = new QListWidgetItem(
    QString(irs::str_conv<QString>(string_type(irst("Modbus")))),
    ui->categoryListWidget);
  item->setData(Qt::UserRole,
    ui->categoryStackedWidget->indexOf(ui->modbusSettingsPage));
*/
  ui->categoryListWidget->setCurrentRow(0);
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}

int	SettingsDialog::exec()
{
  write_settings_to_interface();
  QDialog::exec();
}

/*void SettingsDialog::on_categoryListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{

}
*/

void SettingsDialog::on_categoryListWidget_itemSelectionChanged()
{
  const QListWidgetItem* item = ui->categoryListWidget->currentItem();
  const QVariant data = item->data(Qt::UserRole);
  const int page_index = data.toInt();
  if (page_index < ui->categoryStackedWidget->count()) {
    ui->categoryNameLabel->setText(item->text());
    ui->categoryStackedWidget->setCurrentIndex(page_index);
  }
}

void SettingsDialog::on_applyPushButton_clicked()
{
  read_settings_from_interface();
  settings->save();
  emit apply();
}


void SettingsDialog::on_okPushButton_clicked()
{
  read_settings_from_interface();
  settings->save();
}

void SettingsDialog::read_settings_from_interface()
{
  settings->calibrator_settings.modbus_channel =
    ui->modbusChannelSpinBox->value();

  usb_hid_settings_t* uh_settings = &settings->usb_hid_settings;

  uh_settings->vendor_id = ui->usbHIDVendorIDSpinBox->value();
  uh_settings->product_id = ui->usbHIDProductIDSpinBox->value();

  modbus_settings_t* mb_settings =
    &settings->calibrator_settings.modbus_settings;

  mb_settings->discret_inputs = ui->modbusDiscretInputsSpinBox->value();
  mb_settings->coils = ui->modbusCoilsSpinBox->value();
  mb_settings->holding_registers = ui->modbusHoldingRegistersSpinBox->value();
  mb_settings->input_registers = ui->modbusInputRegistersSpinBox->value();
  mb_settings->update_period = ui->modbusUpdatePeriodDoubleSpinBox->value();

  gen_mode_settings_t* gm = &settings->generator_mode;
  gm->fine_step = normalizeStep(ui->fineSteplineEdit->text(), gm->fine_step);
  gm->normal_step =
   normalizeStep(ui->normalStepLineEdit->text(), gm->normal_step);
  gm->coarse_step =
   normalizeStep(ui->coarseStepLineEdit->text(), gm->coarse_step);
}

void SettingsDialog::write_settings_to_interface()
{
  modbus_settings_t* mb_settings =
    &settings->calibrator_settings.modbus_settings;

  usb_hid_settings_t* uh_settings = &settings->usb_hid_settings;

  ui->usbHIDVendorIDSpinBox->setValue(uh_settings->vendor_id);
  ui->usbHIDProductIDSpinBox->setValue(uh_settings->product_id);

  ui->modbusChannelSpinBox->setValue(
    settings->calibrator_settings.modbus_channel);

  ui->modbusDiscretInputsSpinBox->setValue(mb_settings->discret_inputs);
  ui->modbusCoilsSpinBox->setValue(mb_settings->coils);
  ui->modbusHoldingRegistersSpinBox->setValue(mb_settings->holding_registers);
  ui->modbusInputRegistersSpinBox->setValue(mb_settings->input_registers);
  ui->modbusUpdatePeriodDoubleSpinBox->setValue(mb_settings->update_period);

  gen_mode_settings_t* gm = &settings->generator_mode;
  ui->fineSteplineEdit->setText(stepToStr(gm->fine_step));
  ui->normalStepLineEdit->setText(stepToStr(gm->normal_step));
  ui->coarseStepLineEdit->setText(stepToStr(gm->coarse_step));
}

double SettingsDialog::normalizeStep(const QString& str, double def)
{
  bool ok = false;
  double value = QLocale().toDouble(str, &ok);
  if (!ok) {
    return def;
  }
  // Преобразуем проценты в относительную величину
  value /= 100;
  value = irs::range<double>(value, 2e-5, 10);
  return value;
}

QString SettingsDialog::stepToStr(double step)
{
  return QString("%L1").arg(step*100, 0, 'g', out_precision);
}

