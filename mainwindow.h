#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <irsdefs.h>

#include <irsstrm.h>

#include "settings.h"
#include "settingsdialog.h"
#include "edittemplatedialog.h"
#include "makeresultdialog.h"
#include "calibrator.h"
#include "deviceparametersform.h"

#include <QMainWindow>
#include <QTimer>
#include <QFileDialog>
#include <QClipboard>
#include <QAbstractItemModel>
#include <QDateTime>
#include <QAxObject>
#include <QMovie>

#include <irsfinal.h>

#define CLBPC_DEBUG 0
#define CLBPC_RELEASE !CLBPC_DEBUG

#define DEVICE_PARAMETERS_FORM_ENABLED CLBPC_DEBUG
#define MENU_ITEMS_ENABLED CLBPC_DEBUG

namespace Ui {
  class MainWindow;
}

struct MeasuredPoint
{
  QDateTime dateTime;
  double refValue;
  double value;
  MeasuredPoint():
    dateTime(QDateTime::currentDateTime()),
    refValue(0),
    value(0)
  {
    std::wstring s = irs::str_conv<std::wstring>(dateTime.toString());
  }
};

typedef std::vector<MeasuredPoint> MeasuredPoints;

class TreeModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  TreeModel(QObject *parent = 0);
  ~TreeModel();

  QVariant data(const QModelIndex &index, int role) const;
  /*Qt::ItemFlags flags(const QModelIndex &index) const;
  */QVariant headerData(int section, Qt::Orientation orientation,
    int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column,
    const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  void setPoints(const MeasuredPoints& measuredPoints);
  MeasuredPoint getPoint(const QModelIndex &index);
private:
  MeasuredPoints measuredPoints;
};

class TableModel: public QAbstractTableModel
{
public:
  TableModel(QObject* parent = 0);
  QVariant headerData(int section, Qt::Orientation orientation,
    int role = Qt::DisplayRole) const;
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  //QSize	span(const QModelIndex & index) const;
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void tick();
  void on_settingsAction_triggered();
  //void on_modeListWidget1_currentItemChanged();
  void on_pushButton_clicked();

  void on_pushButton_4_clicked();

  //void on_modeListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

  void on_modeListWidget_itemSelectionChanged();
  void on_settingsDialog_apply();

  void on_connectPushButton_clicked();

  void on_disconnectPushButton_clicked();

  void on_referencePointLineEdit_returnPressed();

  void on_setSetpoinPushButton_clicked();

  void on_stopPushButton_clicked();

  void on_modeDCVPushButton_clicked();

  void on_modeACVPushButton_clicked();

  void on_modeDCIPushButton_clicked();

  void on_modeACIPushButton_clicked();

  void on_referencePointLineEdit_editingFinished();

  void on_copyActualValuePushButton_clicked();

  void on_reduceSetpointPushButton_clicked();

  void on_showNavigationAction_triggered(bool checked);

  void on_showLogAction_triggered(bool checked);

  void on_showMeasuredPointsTreeViewAction_triggered(bool checked);

  void on_hideNeedlessAction_triggered(bool checked);

  void on_actualValueLineEdit_returnPressed();

  void on_coarseIncreasePushButton_clicked();

  void on_coarseReducePushButton_clicked();

  void on_normalIncreasePushButton_clicked();

  void on_normalReducePushButton_clicked();

  void on_fineIncreasePushButton_clicked();

  void on_fineReducePushButton_clicked();

  void on_fixedStepPushButton_clicked();
  void on_fixedIncreasePushButton_clicked();

  void on_fixedReducePushButton_clicked();

  void on_stepLineEdit_selectionChanged();

  void on_pushButton_22_clicked();

  void on_pushButton_28_clicked();

  void on_pushButton_53_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_6_clicked();

  void on_pushButton_7_clicked();

  void on_pushButton_8_clicked();

  void on_autoConnectAnyDevicecheckBox_clicked(bool checked);

  void on_syncTimePushButton_clicked();

  void on_directContrloModeAction_triggered();

  void on_statusAction_triggered();

protected:
  //void focusInEvent(QFocusEvent * event);
  bool eventFilter(QObject *obj, QEvent *ev);
private:
  void wheelEvent (QWheelEvent * event);
  //void keyPressEvent(QKeyEvent *event);
  void tuneActions();
  void tuneModeListWidgets();
  void autoConnect();
  void tryConnect();
  void updateDeviceList();
  void autoConnectAnyDevice();
  void updateConnectPushButton();
  void changeSetpoint();
  generator_mode_t getGeneratorModeFromUI() const;
  void setGeneralModeToUI(generator_mode_t a_mode);

  void hideAllUnnecessary(bool enable);

  double getReferencePointFromUI() const;
  double getActualValueFromUI() const;
  void setSetpointToCalibratorFromRefPointUI();
  void setSetpointToCalibratorFromActualValueUI();
  void setSetpointToCalibrator(double a_setpoint);
  double getMaxSetpoint() const;
  void changeSetpoint(double a_step);
  void increaseSetpoint(double a_step);
  void reduceSetpoint(double a_step);
  void normalizeSetpoint(double* ap_setpoint) const;
  void normalizeStep(double* ap_value) const;
  void updateFixedSteps();
  double getFixedStep() const;
  void setFixedStepButtonsChecked(size_type a_index);
  void uncheckFixedStepButtons();
  Ui::MainWindow *ui;
  settings_t* mp_settings;
  irs::union_streambuf m_stream_buf;
  irs::handle_t <ofstream> mp_log_stream;
  enum { plain_text_edit_buf_size = 500 };
  irs::handle_t<irs::plain_text_edit_buf> mp_plain_text_edit_buf;
  QTimer m_timer;
  usb_hid_device_path_map_t m_usb_hid_device_path_map;
  calibrator_link_t m_calibrator_link;
  calibrator_t m_calibrator;
  bool m_calibrator_enabled;
  SettingsDialog settingsDialog;
  TreeModel* model;
  TableModel* tableModel;
  QMovie* iconMovie;
  const double dcv_max;
  const double dci_max;
  const double acv_max;
  const double aci_max;
  const double coarse_step;
  const double normal_step;
  const double fine_step;
  double m_setpoint;

  struct fixed_step_t
  {
    double value;
    string_type name;
    fixed_step_t(double a_value = 0,
      const string_type& a_name = string_type()
    ):
      value(a_value),
      name(a_name)
    {
    }
    /*fixed_step_t(const fixed_step_t& a_fixed_step):
      value(a_fixed_step.value),
      name(a_fixed_step.name)
    {
    }*/
  };

  vector<fixed_step_t> m_dcv_steps;
  vector<fixed_step_t> m_acv_steps;
  vector<fixed_step_t> m_dci_steps;
  vector<fixed_step_t> m_aci_steps;
  vector<fixed_step_t>* mp_active_steps;

  vector<QPushButton*> m_fixed_step_buttons;

  #if DEVICE_PARAMETERS_FORM_ENABLED
  DeviceParametersForm* deviceParametersForm;
  #endif // DEVICE_PARAMETERS_FORM_ENABLED
};

#endif // MAINWINDOW_H
