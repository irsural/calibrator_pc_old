#ifndef EDITTEMPLATEDIALOG_H
#define EDITTEMPLATEDIALOG_H

#include <QDialog>

namespace Ui {
  class EditTemplateDialog;
}

class EditTemplateDialog : public QDialog
{
  Q_OBJECT

public:
  explicit EditTemplateDialog(QWidget *parent = 0);
  ~EditTemplateDialog();

private:
  Ui::EditTemplateDialog *ui;
};

#endif // EDITTEMPLATEDIALOG_H
