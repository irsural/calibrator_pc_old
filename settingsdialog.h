#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <irsdefs.h>

#include <irsstrm.h>

#include <QDialog>

#include "settings.h"

#include <irsfinal.h>

namespace Ui {
  class SettingsDialog;
}

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;

  explicit SettingsDialog(QWidget *parent,
    settings_t* ap_settings);
  ~SettingsDialog();
public slots:
  virtual int	exec();
private slots:
  //void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

  //void on_categoryListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

  //void on_categoryListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

  void on_categoryListWidget_itemSelectionChanged();

  void on_applyPushButton_clicked();

  void on_okPushButton_clicked();

signals:
  void apply();

private:
  void tuneCategoryListWidgets();
  void read_settings_from_interface();
  void write_settings_to_interface();
  double normalizeStep(const QString& str, double def = 0);
  QString stepToStr(double step);
  Ui::SettingsDialog *ui;
  settings_t* settings;
};

#endif // SETTINGSDIALOG_H
