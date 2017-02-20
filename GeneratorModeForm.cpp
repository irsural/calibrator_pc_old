#include "GeneratorModeForm.h"
#include "ui_generatormodeform.h"

#include <irsfinal.h>

double qStringToDouble(const QString& str, bool* ok)
{
  return QLocale().toDouble(str, ok);
}

QString DoubleToQString(double num, int precision)
{
  return QString("%L1").arg(num, 0, 'g', precision);
}

GeneratorModeForm::GeneratorModeForm(QWidget *parent,
  calibrator_link_t* ap_calibratorLink,
  calibrator_t* ap_calibrator,
  settings_t* ap_settings
) :
  QWidget(parent),
  ui(new Ui::GeneratorModeForm),
  timer(),
  iconMovie(),
  calibratorLink(ap_calibratorLink),
  calibrator(ap_calibrator),
  settings(ap_settings),
  status(status_t::connection),
  syncNetvarsTimer(irs::make_cnt_s(0.1)),
  dcvSteps(),
  acvSteps(),
  dciSteps(),
  aciSteps(),
  dcvRange(),
  acvRange(),
  dciRange(),
  aciRange(),
  activeSteps(&dcvSteps),
  range(&dcvRange),
  m_fixed_step_buttons(),
  uiMode(generator_mode_dc_voltage)
{
  ui->setupUi(this);

  connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));
  timer.start(10);

  iconMovie.setScaledSize(QSize(30, 30));

  QPixmap pixmap(":/Images/Images/icon_check_green.png");
  ui->setpointErrorlabel->setPixmap(pixmap.scaled(QSize(24, 24),
    Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

  ui->stepLineEdit->setStyleSheet("");

  ui->stepLineEdit->installEventFilter(this);

  dcvSteps.fixedSteps.push_back(standard_fixed_step_t(10e-6, irst("10 мкВ")));
  dcvSteps.fixedSteps.push_back(standard_fixed_step_t(100e-6, irst("100 мкВ")));
  dcvSteps.fixedSteps.push_back(standard_fixed_step_t(0.001, irst("1 мВ")));
  dcvSteps.fixedSteps.push_back(standard_fixed_step_t(0.01, irst("10 мВ")));
  dcvSteps.fixedSteps.push_back(standard_fixed_step_t(0.1, irst("100 мВ")));
  dcvSteps.fixedSteps.push_back(standard_fixed_step_t(1, irst("1 В")));
  dcvSteps.fixedSteps.push_back(standard_fixed_step_t(10, irst("10 В")));
  dcvSteps.config = &settings->generator_mode.dcv_fixed_step;

  dcvRange.min = 0;
  dcvRange.max = dcv_max;
  dcvRange.stepMin = dcv_step_min;
  dcvRange.stepMax = dcv_step_max;
  dcvRange.steps = dcvSteps;

  acvSteps.fixedSteps.push_back(standard_fixed_step_t(100e-6, irst("100 мкВ")));
  acvSteps.fixedSteps.push_back(standard_fixed_step_t(0.001, irst("1 мВ")));
  acvSteps.fixedSteps.push_back(standard_fixed_step_t(0.01, irst("10 мВ")));
  acvSteps.fixedSteps.push_back(standard_fixed_step_t(0.1, irst("100 мВ")));
  acvSteps.fixedSteps.push_back(standard_fixed_step_t(1, irst("1 В")));
  acvSteps.fixedSteps.push_back(standard_fixed_step_t(10, irst(" 10 В")));
  acvSteps.fixedSteps.push_back(standard_fixed_step_t(100, irst("100 В")));
  acvSteps.config = &settings->generator_mode.acv_fixed_step;

  acvRange.min = 0;
  acvRange.max = acv_max;
  acvRange.stepMin = acv_step_min;
  acvRange.stepMax = acv_step_max;
  acvRange.steps = acvSteps;

  dciSteps.fixedSteps.push_back(standard_fixed_step_t(1e-6, irst("1 мкА")));
  dciSteps.fixedSteps.push_back(standard_fixed_step_t(10e-6, irst("10 мкА")));
  dciSteps.fixedSteps.push_back(standard_fixed_step_t(100e-6, irst("100 мкА")));
  dciSteps.fixedSteps.push_back(standard_fixed_step_t(0.001, irst("1 мА")));
  dciSteps.fixedSteps.push_back(standard_fixed_step_t(0.01, irst("10 мА")));
  dciSteps.fixedSteps.push_back(standard_fixed_step_t(0.1, irst("100 мА")));
  dciSteps.fixedSteps.push_back(standard_fixed_step_t(1, irst("1 А")));
  dciSteps.config = &settings->generator_mode.dci_fixed_step;

  dciRange.min = 0;
  dciRange.max = dci_max;
  dciRange.stepMin = dci_step_min;
  dciRange.stepMax = dci_step_max;
  dciRange.steps = dciSteps;

  aciSteps.fixedSteps.push_back(standard_fixed_step_t(10e-9, irst("10 нА")));
  aciSteps.fixedSteps.push_back(standard_fixed_step_t(100e-9, irst("100 нА")));
  aciSteps.fixedSteps.push_back(standard_fixed_step_t(1e-6, irst("1 мкА")));
  aciSteps.fixedSteps.push_back(standard_fixed_step_t(10e-6, irst("10 мкА")));
  aciSteps.fixedSteps.push_back(standard_fixed_step_t(100e-6, irst("100 мкА")));
  aciSteps.fixedSteps.push_back(standard_fixed_step_t(0.001, irst("1 мА")));
  aciSteps.fixedSteps.push_back(standard_fixed_step_t(0.01, irst("10 мА")));
  aciSteps.config = &settings->generator_mode.aci_fixed_step;

  aciRange.min = 0;
  aciRange.max = aci_max;
  aciRange.stepMin = aci_step_min;
  aciRange.stepMax = aci_step_max;
  aciRange.steps = aciSteps;

  m_fixed_step_buttons.push_back(ui->fixedStep1PushButton);
  m_fixed_step_buttons.push_back(ui->fixedStep2PushButton);
  m_fixed_step_buttons.push_back(ui->fixedStep3PushButton);
  m_fixed_step_buttons.push_back(ui->fixedStep4PushButton);
  m_fixed_step_buttons.push_back(ui->fixedStep5PushButton);
  m_fixed_step_buttons.push_back(ui->fixedStep6PushButton);
  m_fixed_step_buttons.push_back(ui->fixedStep7PushButton);

  /*for (size_type i = 0; i < m_fixed_step_buttons.size(); i++) {
    connect(m_fixed_step_buttons[i], SIGNAL(clicked()), this,
      SLOT(on_fixedStepPushButton_clicked()));
  }*/

  setGeneratorModeToUI(generator_mode_dc_voltage, true);

  updateStatus(true);
}

GeneratorModeForm::~GeneratorModeForm()
{
  delete ui;
}

void GeneratorModeForm::tick()
{
  if (syncNetvarsTimer.check()) {

    if (calibrator->connected()) {
      double setpoint = 0;
      generator_mode_t generator_mode = generator_mode_dc_voltage;
      calibrator->get_setpoint(&generator_mode, &setpoint);
      if (calibrator->is_setpoint_changed()) {
        setSetpointToUI(setpoint);
      }

      if ((generator_mode == generator_mode_ac_voltage) ||
          (generator_mode == generator_mode_ac_current)) {
        const double actual_value = calibrator->get_actual_value();
        ui->actualLabel->setText(
          irs::str_conv<QString>(string_type(
             irst("Уставка (Текущее значение: "))) +
          QString("%L1").arg(actual_value, 0, 'g', out_precision) +
          QString(")"));
      } else {
        ui->actualLabel->setText(
          irs::str_conv<QString>(string_type(irst("Уставка"))));
      }

      setGeneratorModeToUI(generator_mode);

      setSignalEnabledUI(isSignalEnabled());

      if (calibrator->is_frequency_changed()) {
        setFrequencyFromDeviceToUI();
      }

      ui->currentDirectionToolButton->setChecked(
        calibrator->get_reverse_enabled());
    }

    updateStatus();
  }
}

bool GeneratorModeForm::eventFilter(QObject *obj, QEvent *ev)
{
  if (obj == ui->stepLineEdit) {
    if (ev->type() == QEvent::FocusIn) {
      selectUserFixedStep();
      ui->stepLineEdit->setStyleSheet("");
    } else if (ev->type() == QEvent::FocusOut) {
      normalizeAndApplyUserStepValueFromUI();
    } else if (ev->type() == QEvent::Enter) {

    }
    return false;
  } else {
    // Пересылка сообщения родительскому классу
    return GeneratorModeForm::eventFilter(obj, ev);
  }
}

void GeneratorModeForm::wheelEvent (QWheelEvent * event)
{
  const int delta = event->delta();
  const int k = delta >= 0 ? 1 : -1;
  int numDegrees = abs(event->delta()/8);
  int numSteps = std::max(numDegrees/15, 1);
  numSteps *= k;

  const Qt::KeyboardModifiers keys = event->modifiers();
  if ((keys & Qt::ShiftModifier) && (keys & Qt::ControlModifier)) {
    changeSetpoint(numSteps*activeSteps->getStep());
  } else if (keys & Qt::ShiftModifier) {
    changeSetpoint(numSteps*settings->generator_mode.coarse_step);
  } else if (keys & Qt::ControlModifier) {
    changeSetpoint(numSteps*settings->generator_mode.normal_step);
  } else {
    changeSetpoint(numSteps*settings->generator_mode.fine_step);
  }

  event->accept();
}

void GeneratorModeForm::updateStatus(bool forced)
{
  generator_mode_t generator_mode = generator_mode_dc_voltage;
  double setpoint = 0;
  calibrator->get_setpoint(&generator_mode, &setpoint);

  status_t new_status;
  if (ui->startPushButton->isChecked()) {
    const generator_status_t generator_status = calibrator->get_status();

    switch (generator_status) {
      case generator_status_busy: {
        new_status = status_t::generator_busy;
      } break;
      case generator_status_ready: {
        if (setpoint) {
          new_status = status_t::generator_ready;
        } else {
          new_status = status_t::connected;
        }
      } break;
    }
  } else {
    const calibrator_link_t::status_t connection_status =
      calibratorLink->get_status();

    if (connection_status == calibrator_link_t::status_connected) {
      new_status = status_t::connected;
    } else {
      new_status = status_t::connection;
    }
  }

  if ((new_status != status) || forced) {
    setStatus(new_status);
  }
}

void GeneratorModeForm::setStatus(status_t newStatus)
{
  status = newStatus;
  switch (status) {
    case status_t::connection: {
      ui->generatorStatusLabel->setText(
        irs::str_conv<QString>(string_type(irst("Пытаюсь подключиться"))));
      iconMovie.stop();
      iconMovie.setFileName(":/Images/Images/connect.GIF");
      iconMovie.start();
      ui->generatorStatusIconlabel->setMovie(&iconMovie);
    } break;
    case status_t::connected: {
      ui->generatorStatusLabel->setText(
        irs::str_conv<QString>(string_type(irst("Сигнал выключен"))));
      QPixmap pixmap(":/Images/Images/blueStone.png");
      ui->generatorStatusIconlabel->setPixmap(pixmap.scaled(QSize(30, 30),
        Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } break;
    case status_t::generator_busy: {
      ui->generatorStatusLabel->setText(
        irs::str_conv<QString>(string_type(irst("Ожидание выхода на режим"))));
      iconMovie.stop();
      iconMovie.setFileName(":/Images/Images/process_red.GIF");
      iconMovie.start();
      ui->generatorStatusIconlabel->setMovie(&iconMovie);
    } break;
    case status_t::generator_ready: {
      ui->generatorStatusLabel->setText(
        irs::str_conv<QString>(string_type(irst("Готов"))));
      QPixmap pixmap(":/Images/Images/greenStone.png");
      ui->generatorStatusIconlabel->setPixmap(pixmap.scaled(QSize(30, 30),
        Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } break;
  }

}

generator_mode_t GeneratorModeForm::getGeneratorModeFromUI() const
{
  generator_mode_t generator_mode = generator_mode_dc_voltage;
  if (ui->modeDCVPushButton->isChecked()) {
    generator_mode = generator_mode_dc_voltage;
  } else if (ui->modeACVPushButton->isChecked()) {
    generator_mode = generator_mode_ac_voltage;
  } else if (ui->modeDCIPushButton->isChecked()) {
    generator_mode = generator_mode_dc_current;
  } else {
    generator_mode = generator_mode_ac_current;
  }
  return generator_mode;
}

void GeneratorModeForm::setGeneratorModeToUI(
  generator_mode_t a_mode, bool force)
{
  if ((uiMode != a_mode) || force) {
    uiMode = a_mode;
    ui->modeDCVPushButton->setChecked(false);
    ui->modeACVPushButton->setChecked(false);
    ui->modeDCIPushButton->setChecked(false);
    ui->modeACIPushButton->setChecked(false);
    switch (a_mode) {
      case generator_mode_dc_voltage: {
        ui->modeDCVPushButton->setChecked(true);
      } break;
      case generator_mode_ac_voltage: {
        ui->modeACVPushButton->setChecked(true);
      } break;
      case generator_mode_dc_current: {
        ui->modeDCIPushButton->setChecked(true);
      } break;
      case generator_mode_ac_current: {
        ui->modeACIPushButton->setChecked(true);
      } break;
    }
    updateRange();
    updateStepsUI();
    updateUIForGeneratorMode();
  }
}

void GeneratorModeForm::setFrequencyFromDeviceToUI()
{
  const double setpoint = calibrator->get_frequency();
  ui->frequencyDoubleSpinBox->setValue(setpoint);
}

void GeneratorModeForm::on_modeDCVPushButton_clicked()
{
  stop();
  generator_mode_t generator_mode = generator_mode_dc_voltage;
  setGeneratorModeToUI(generator_mode);
  double setpoint = 0;
  calibrator->set_setpoint(generator_mode, setpoint);
}

void GeneratorModeForm::on_modeACVPushButton_clicked()
{
  stop();
  generator_mode_t generator_mode = generator_mode_ac_voltage;
  setGeneratorModeToUI(generator_mode);
  double setpoint = 0;
  calibrator->set_setpoint(generator_mode, setpoint);
}

void GeneratorModeForm::on_modeDCIPushButton_clicked()
{
  stop();
  generator_mode_t generator_mode = generator_mode_dc_current;
  setGeneratorModeToUI(generator_mode);
  double setpoint = 0;
  calibrator->set_setpoint(generator_mode, setpoint);
}

void GeneratorModeForm::on_modeACIPushButton_clicked()
{
  stop();
  generator_mode_t generator_mode = generator_mode_ac_current;
  setGeneratorModeToUI(generator_mode);
  double setpoint = 0;
  calibrator->set_setpoint(generator_mode, setpoint);
}

void GeneratorModeForm::normalizeAndApplySetpointUI()
{
  QString str = ui->actualValueLineEdit->text();
  bool ok = false;
  double setpoint = qStringToDouble(ui->actualValueLineEdit->text(), &ok);
  if (!ok) {
    setpoint = settings->generator_mode.setpoint;
  }
  setpoint = normalizeSetpoint(setpoint);
  settings->generator_mode.setpoint = setpoint;

  ui->actualValueLineEdit->setText(QString("%L1").arg(
    setpoint, 0, 'g', out_precision));
}

double GeneratorModeForm::checkSetpoint(bool* ok)
{
  QString str = ui->actualValueLineEdit->text();
  double setpoint = qStringToDouble(ui->actualValueLineEdit->text(), ok);
  if (!*ok) {
    return 0;
  }
  *ok = irs::is_in_range(setpoint, range->min, range->max);
  if (!*ok) {
    return 0;
  }
  return setpoint;
}

double GeneratorModeForm::getSetpointFromUI()
{
  bool ok = false;
  double setpoint = qStringToDouble(ui->actualValueLineEdit->text(), &ok);
  if (!ok) {
    setpoint = settings->generator_mode.setpoint;
  }
  return setpoint;
}

void GeneratorModeForm::setSetpointToUI(double setpoint)
{
  ui->actualValueLineEdit->setText(QString("%L1").arg(
    setpoint, 0, 'g', out_precision));
  updateSetpointErrorIcon();
}

void GeneratorModeForm::setSetpointToDeviceFromUI()
{
  const double setpoint = getSetpointFromUI();
  const generator_mode_t generator_mode = getGeneratorModeFromUI();
  calibrator->set_setpoint(generator_mode, setpoint);
}

bool GeneratorModeForm::isSignalEnabled()
{
  return calibrator->get_signal_enabled();
}

void GeneratorModeForm::enableSignal()
{
  calibrator->set_signal_enabled(true);
}

void GeneratorModeForm::disableSignal()
{
  calibrator->set_signal_enabled(false);
}

void GeneratorModeForm::updateRange()
{
  switch (getGeneratorModeFromUI()) {
    case generator_mode_dc_voltage: {
      range = &dcvRange;
    } break;
    case generator_mode_ac_voltage: {
      range = &acvRange;
    } break;
    case generator_mode_dc_current: {
      range = &dciRange;
    } break;
    case generator_mode_ac_current: {
      range = &aciRange;
    } break;
  }
  activeSteps = &range->steps;
}

void GeneratorModeForm::updateStepsUI()
{
  const vector<standard_fixed_step_t>* fixedSteps = &activeSteps->fixedSteps;
  for (size_type i = 0; i < m_fixed_step_buttons.size(); i++) {
    if (i < fixedSteps->size()) {
      string_type str = (*fixedSteps)[i].name;
      m_fixed_step_buttons[i]->setText(
        irs::str_conv<QString>((*fixedSteps)[i].name));
      m_fixed_step_buttons[i]->show();
    } else {
      m_fixed_step_buttons[i]->hide();
    }
  }

  switch (getGeneratorModeFromUI()) {
    case generator_mode_dc_voltage:
    case generator_mode_ac_voltage: {
      ui->stepLabel->setText(irs::str_conv<QString>(string_type(irst("В"))));
    } break;
    case generator_mode_dc_current:
    case generator_mode_ac_current: {
      ui->stepLabel->setText(irs::str_conv<QString>(string_type(irst("А"))));
    } break;
  }

  updateSelectedStepUI();
  updateUserStepValueUI();
}

void GeneratorModeForm::updateUserStepValueUI()
{
  ui->stepLineEdit->setText(DoubleToQString(activeSteps->config->user_step));
}

void GeneratorModeForm::selectFixedStepButton(size_type a_index)
{
  for (size_type i = 0; i < m_fixed_step_buttons.size(); i++) {
    const bool checked = (i == a_index);
    m_fixed_step_buttons[i]->setChecked(checked);
  }
  activeSteps->config->selected_fixed_step = a_index;

  QColor c = QApplication::palette().color(QPalette::Active, QPalette::Window);
  QString sheet = QString::fromLatin1(
    "QLineEdit{background-color: %1;}"
    "QLineEdit:hover{border: 1px solid gray; background-color 1%;}").arg(
    c.name());

  ui->stepLineEdit->setStyleSheet(sheet);
}

void GeneratorModeForm::selectUserFixedStep()
{
  for (size_type i = 0; i < m_fixed_step_buttons.size(); i++) {
    m_fixed_step_buttons[i]->setChecked(false);
  }

  activeSteps->config->selected_fixed_step = noneSelectedFixedButton;

  ui->stepLineEdit->setStyleSheet("");
}

void GeneratorModeForm::updateSelectedStepUI()
{
  if (activeSteps->config->selected_fixed_step < m_fixed_step_buttons.size()) {
    selectFixedStepButton(activeSteps->config->selected_fixed_step);
  } else {
    selectUserFixedStep();
  }
}

void GeneratorModeForm::normalizeAndApplyUserStepValueFromUI()
{
  bool ok = false;
  double step = qStringToDouble(ui->stepLineEdit->text(), &ok);

  if (!ok) {
    step = activeSteps->config->user_step;
  }

  step = normalizeStep(step);
  activeSteps->config->user_step = step;

  ui->stepLineEdit->setText(QString("%L1").arg(
    step, 0, 'g', out_precision));
}

void GeneratorModeForm::setSignalEnabledUI(bool enable)
{
  ui->startPushButton->setChecked(enable);
  ui->stopPushButton->setChecked(!enable);
}

void GeneratorModeForm::updateUIForGeneratorMode()
{
  switch (getGeneratorModeFromUI()) {
    case generator_mode_dc_voltage:
    case generator_mode_dc_current: {
      ui->currentDirectionToolButton->setEnabled(true);
      ui->frequencyDoubleSpinBox->setEnabled(false);
    } break;
    case generator_mode_ac_voltage:
    case generator_mode_ac_current: {
      ui->currentDirectionToolButton->setEnabled(false);
      ui->frequencyDoubleSpinBox->setEnabled(true);
    } break;
  }
}

void GeneratorModeForm::setSetpointToDevice(double setpoint)
{
  const auto mode = getGeneratorModeFromUI();
  calibrator->set_setpoint(mode, setpoint);
}

void GeneratorModeForm::applySetpointFromUI()
{
  normalizeAndApplySetpointUI();
  setSetpointToDeviceFromUI();
  const double setpoint = getSetpointFromUI();

  if (isSignalEnabled()) {
    if (setpoint == 0) {
      stop();
    }
  } else {
    if (setpoint != 0) {
      start();
    }
  }
}

void GeneratorModeForm::applyFrequencyFromUI()
{
  const double setpoint = ui->frequencyDoubleSpinBox->value();
  calibrator->set_frequency(setpoint);
}

void GeneratorModeForm::updateSetpointErrorIcon()
{
  bool ok = false;
  const double setpointUI = checkSetpoint(&ok);
  const double setpoint = calibrator->get_setpoint();
  if (ok) {
    if (irs::equals_value == irs::compare_value(setpointUI, setpoint, 1e-13)) {
      QPixmap pixmap(":/Images/Images/icon_check_green.png");
      ui->setpointErrorlabel->setPixmap(pixmap.scaled(QSize(24, 24),
        Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
      QPixmap pixmap(":/Images/Images/icon_check_orange.png");
      ui->setpointErrorlabel->setPixmap(pixmap.scaled(QSize(24, 24),
        Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
  } else {
    QPixmap pixmap(":/Images/Images/error.png");
    ui->setpointErrorlabel->setPixmap(pixmap.scaled(QSize(24, 24),
      Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
  }
}

void GeneratorModeForm::increaseSetpoint(double a_step)
{
  changeSetpoint(a_step);
}

void GeneratorModeForm::reduceSetpoint(double a_step)
{
  changeSetpoint(-a_step);
}

void GeneratorModeForm::changeSetpoint(double step)
{
  generator_mode_t generator_mode = generator_mode_dc_voltage;
  double setpoint = 0;
  calibrator->get_setpoint(&generator_mode, &setpoint);

  double sign = (step >= 0) ? 1 : -1;
  double absolute_step = abs(setpoint*step);
  int exp = floor(log10(absolute_step));


  absolute_step /= pow(10., exp);
  double new_step = 0;

  if (absolute_step <= 2) {
    new_step = (absolute_step < sqrt(1*2)) ? 1 : 2;
  } else if (absolute_step < 5) {
    new_step = (absolute_step < sqrt(2*5)) ? 2 : 5;
  } else {
    new_step = (absolute_step < sqrt(5*10)) ? 5 : 10;
  }

  new_step *= pow(10., exp);

  setpoint += (new_step*sign);
  if (sign > 0) {
    setpoint = ceil(setpoint/new_step)*new_step;
  } else {
    setpoint = floor(setpoint/new_step)*new_step;
  }

  setpoint = normalizeSetpoint(setpoint);

  settings->generator_mode.setpoint = setpoint;

  setSetpointToDevice(setpoint);
  setSetpointToUI(setpoint);
}

void GeneratorModeForm::increaseSetpointFixedStep(double a_step)
{
  changeSetpointFixedStep(a_step);
}

void GeneratorModeForm::reduceSetpointFixedStep(double a_step)
{
  changeSetpointFixedStep(-a_step);
}

void GeneratorModeForm::changeSetpointFixedStep(double step)
{
  generator_mode_t generator_mode = generator_mode_dc_voltage;
  double setpoint = 0;
  calibrator->get_setpoint(&generator_mode, &setpoint);

  setpoint = normalizeSetpoint(setpoint + step);

  settings->generator_mode.setpoint = setpoint;

  setSetpointToDevice(setpoint);
  setSetpointToUI(setpoint);
}

double GeneratorModeForm::normalizeSetpoint(double setpoint)
{
  return irs::range(setpoint, range->min, range->max);
}

double GeneratorModeForm::normalizeStep(double step)
{
  return irs::range(step, range->stepMin, range->stepMax);
}

bool GeneratorModeForm::getSignalEnabledStatusFromUI() const
{
  return ui->startPushButton->isChecked();
}

void GeneratorModeForm::start()
{
  setSignalEnabledUI(true);
  setSetpointToDeviceFromUI();
  enableSignal();
}

void GeneratorModeForm::stop()
{
  setSignalEnabledUI(false);
  disableSignal();
}

void GeneratorModeForm::on_fixedStep1PushButton_clicked()
{
  selectFixedStepButton(0);
}

void GeneratorModeForm::on_fixedStep2PushButton_clicked()
{
  selectFixedStepButton(1);
}

void GeneratorModeForm::on_fixedStep3PushButton_clicked()
{
  selectFixedStepButton(2);
}

void GeneratorModeForm::on_fixedStep4PushButton_clicked()
{
  selectFixedStepButton(3);
}

void GeneratorModeForm::on_fixedStep5PushButton_clicked()
{
  selectFixedStepButton(4);
}

void GeneratorModeForm::on_fixedStep6PushButton_clicked()
{
  selectFixedStepButton(5);
}

void GeneratorModeForm::on_fixedStep7PushButton_clicked()
{
  selectFixedStepButton(6);
}

void GeneratorModeForm::on_coarseIncreasePushButton_clicked()
{
  increaseSetpoint(settings->generator_mode.coarse_step);
}

void GeneratorModeForm::on_coarseReducePushButton_clicked()
{
  reduceSetpoint(settings->generator_mode.coarse_step);
}

void GeneratorModeForm::on_normalIncreasePushButton_clicked()
{
  increaseSetpoint(settings->generator_mode.normal_step);
}

void GeneratorModeForm::on_normalReducePushButton_clicked()
{
  reduceSetpoint(settings->generator_mode.normal_step);
}

void GeneratorModeForm::on_fineIncreasePushButton_clicked()
{
  increaseSetpoint(settings->generator_mode.fine_step);
}

void GeneratorModeForm::on_fineReducePushButton_clicked()
{
  reduceSetpoint(settings->generator_mode.fine_step);
}

void GeneratorModeForm::on_fixedIncreasePushButton_clicked()
{
  increaseSetpointFixedStep(activeSteps->getStep());
}

void GeneratorModeForm::on_fixedReducePushButton_clicked()
{
  reduceSetpointFixedStep(activeSteps->getStep());
}

void GeneratorModeForm::on_actualValueLineEdit_returnPressed()
{
  applySetpointFromUI();
}

void GeneratorModeForm::on_stepLineEdit_returnPressed()
{
  normalizeAndApplyUserStepValueFromUI();
}

void GeneratorModeForm::on_startPushButton_clicked(bool checked)
{
  // Запрещаем отжатие кнопки
  if (!checked) {
    ui->startPushButton->setChecked(true);
    return;
  }

  start();
}

void GeneratorModeForm::on_stopPushButton_clicked(bool checked)
{
  // Запрещаем отжатие кнопки
  if (!checked) {
    ui->stopPushButton->setChecked(true);
    return;
  }
  stop();
}

void GeneratorModeForm::on_copyActualValuePushButton_2_clicked()
{
  QApplication::clipboard()->setText(ui->actualValueLineEdit->text());
}

void GeneratorModeForm::on_applySetpointPushButton_clicked()
{
  applySetpointFromUI();
}

void GeneratorModeForm::on_actualValueLineEdit_textChanged(const QString &arg1)
{
  updateSetpointErrorIcon();
}

void GeneratorModeForm::on_frequencyDoubleSpinBox_valueChanged(double arg1)
{
  applyFrequencyFromUI();
}

void GeneratorModeForm::on_currentDirectionToolButton_clicked()
{
  calibrator->set_reverse_enabled(ui->currentDirectionToolButton->isChecked());
}
