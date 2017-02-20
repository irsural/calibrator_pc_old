#ifndef MAKERESULTDIALOG_H
#define MAKERESULTDIALOG_H

#include <QDialog>

namespace Ui {
  class MakeResultDialog;
}

class MakeResultDialog : public QDialog
{
  Q_OBJECT

public:
  explicit MakeResultDialog(QWidget *parent = 0);
  ~MakeResultDialog();

private:
  Ui::MakeResultDialog *ui;
};

#endif // MAKERESULTDIALOG_H
