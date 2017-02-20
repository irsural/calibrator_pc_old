#ifndef GENERATORMODEFORM_H
#define GENERATORMODEFORM_H

#include <irsdefs.h>

#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QMovie>
#include <QClipboard>

#include "settings.h"
#include "calibrator.h"

#include <irsfinal.h>

namespace Ui {
  class GeneratorModeForm;
}

double qStringToDouble(const QString& str, bool* ok);

QString DoubleToQString(double num, int precision = out_precision);

class GeneratorModeForm : public QWidget
{
  Q_OBJECT

public:
  typedef size_t size_type;
  typedef irs::string_t string_type;
  explicit GeneratorModeForm(QWidget *parent,
    calibrator_link_t* ap_calibratorLink,
    calibrator_t* ap_calibrator,
    settings_t* ap_settings);
  ~GeneratorModeForm();

private slots:
  void tick();

  void on_modeDCVPushButton_clicked();

  void on_modeACVPushButton_clicked();

  void on_modeDCIPushButton_clicked();

  void on_modeACIPushButton_clicked();

  void on_fixedStep1PushButton_clicked();

  void on_fixedStep2PushButton_clicked();

  void on_fixedStep3PushButton_clicked();

  void on_fixedStep4PushButton_clicked();

  void on_fixedStep5PushButton_clicked();

  void on_fixedStep6PushButton_clicked();

  void on_fixedStep7PushButton_clicked();

  void on_coarseIncreasePushButton_clicked();

  void on_coarseReducePushButton_clicked();

  void on_normalIncreasePushButton_clicked();

  void on_normalReducePushButton_clicked();

  void on_fineIncreasePushButton_clicked();

  void on_fineReducePushButton_clicked();

  void on_fixedIncreasePushButton_clicked();

  void on_fixedReducePushButton_clicked();

  void on_actualValueLineEdit_returnPressed();

  void on_stepLineEdit_returnPressed();

  void on_startPushButton_clicked(bool checked);

  void on_stopPushButton_clicked(bool checked);

  void on_copyActualValuePushButton_2_clicked();

  void on_applySetpointPushButton_clicked();

  void on_actualValueLineEdit_textChanged(const QString &arg1);

  void on_frequencyDoubleSpinBox_valueChanged(double arg1);

  void on_currentDirectionToolButton_clicked();

protected:
  bool eventFilter(QObject *obj, QEvent *ev);
  void wheelEvent (QWheelEvent * event);
private:
  enum class status_t {
    connection,
    connected,
    generator_busy,
    generator_ready
  };

  void updateStatus(bool forced = false);
  void setStatus(status_t newStatus);
  generator_mode_t getGeneratorModeFromUI() const;
  void setGeneratorModeToUI(generator_mode_t a_mode, bool force = false);
  void setFrequencyFromDeviceToUI();

  void normalizeAndApplySetpointUI();
  double getSetpointFromUI();
  void setSetpointToUI(double setpoint);
  void setSetpointToDeviceFromUI();
  bool isSignalEnabled();
  void enableSignal();
  void disableSignal();

  void updateRange();

  // Функции для работы с элементами управления шагом
  void updateStepsUI();
  void updateUserStepValueUI();
  void selectFixedStepButton(size_type a_index);
  void selectUserFixedStep();
  void updateSelectedStepUI();
  void normalizeAndApplyUserStepValueFromUI();

  void setSignalEnabledUI(bool enable);

  //void updateSetpointMax();

  void updateUIForGeneratorMode();

  void setSetpointToDevice(double setpoint);

  void applySetpointFromUI();

  void applyFrequencyFromUI();

  void updateSetpointErrorIcon();

  void increaseSetpoint(double a_step);
  void reduceSetpoint(double a_step);
  void changeSetpoint(double step);
  void increaseSetpointFixedStep(double a_step);
  void reduceSetpointFixedStep(double a_step);
  void changeSetpointFixedStep(double step);

  double normalizeSetpoint(double setpoint);
  double normalizeStep(double step);
  double checkSetpoint(bool* ok);
  void normalizeStep(double* ap_value) const;
  bool getSignalEnabledStatusFromUI() const;

  void start();
  void stop();

  Ui::GeneratorModeForm *ui;
  QTimer timer;
  QMovie iconMovie;
  calibrator_link_t* calibratorLink;
  calibrator_t* calibrator;
  settings_t* settings;

  status_t status;
  irs::loop_timer_t syncNetvarsTimer;

  struct standard_fixed_step_t
  {
    double value;
    string_type name;
    standard_fixed_step_t(double a_value = 0,
      const string_type& a_name = string_type()
    ):
      value(a_value),
      name(a_name)
    {
    }
  };

  struct steps_t
  {
    vector<standard_fixed_step_t> fixedSteps;
    fixed_step_t* config;
    double getStep() const
    {
      if (config->selected_fixed_step < fixedSteps.size()) {
        return fixedSteps[config->selected_fixed_step].value;
      }
      return config->user_step;
    }
  };

  struct range_t
  {
    double min = 0;
    double max = 0;
    double stepMin = 0;
    double stepMax = 0;
    steps_t steps;
  };

  steps_t dcvSteps;
  steps_t acvSteps;
  steps_t dciSteps;
  steps_t aciSteps;

  range_t dcvRange;
  range_t acvRange;
  range_t dciRange;
  range_t aciRange;

  steps_t* activeSteps;

  range_t* range;

  vector<QPushButton*> m_fixed_step_buttons;

  const size_type noneSelectedFixedButton =
    std::numeric_limits<size_type>::max();

  generator_mode_t uiMode;
};

#endif // GENERATORMODEFORM_H
