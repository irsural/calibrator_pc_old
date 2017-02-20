#include "edittemplatedialog.h"
#include "ui_edittemplatedialog.h"

EditTemplateDialog::EditTemplateDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EditTemplateDialog)
{
  ui->setupUi(this);
}

EditTemplateDialog::~EditTemplateDialog()
{
  delete ui;
}
