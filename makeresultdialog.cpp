#include "makeresultdialog.h"
#include "ui_makeresultdialog.h"

MakeResultDialog::MakeResultDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::MakeResultDialog)
{
  ui->setupUi(this);
}

MakeResultDialog::~MakeResultDialog()
{
  delete ui;
}
