#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAxObject>
#include <QDebug>

#include <regex>

#include "GeneratorModeForm.h"


#include "msword.h"

#include <irs_codecvt.h>

#include <irsfinal.h>

TreeModel::TreeModel(QObject *parent)
  : QAbstractItemModel(parent),
  measuredPoints()
{
  MeasuredPoint point;
  point.dateTime = QDateTime::currentDateTime();
  point.refValue = 1;
  point.value = 1.01;
  measuredPoints.push_back(point);
  point.refValue = 2;
  point.value = 2.003;
  measuredPoints.push_back(point);
  point.refValue = 3;
  point.value = 2.991;
  measuredPoints.push_back(point);
  point.refValue = 4;
  point.value = 3.992;
  measuredPoints.push_back(point);
  point.refValue = 5;
  point.value = 5.02;
  measuredPoints.push_back(point);
}

TreeModel::~TreeModel()
{
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
  const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid()) {
    const MeasuredPoint* ptr = &measuredPoints[row];
    return createIndex(row, column, const_cast<MeasuredPoint*>(ptr));
  }

  return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex & /*index*/) const
{
  return QModelIndex();
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }

  return measuredPoints.size();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return 8;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
     return QVariant();

  if (role != Qt::DisplayRole)
      return QVariant();

  const MeasuredPoint* ptr =
    static_cast<MeasuredPoint*>(index.internalPointer());

  switch (index.column()) {
    case 0: {
      return irs::str_conv<QString>(std::wstring(L"U~"));
    }
    case 1: {
      return irs::str_conv<QString>(std::wstring(L"\x2197"));
    }
    case 2: {
      return QString::number(ptr->refValue);

    }
    case 3: {
      return QString::number(ptr->value);
    }
    case 6: {
      return irs::str_conv<QString>(std::wstring(L"Прошел"));
    }
    case 7: {
      return ptr->dateTime.time().toString();
    }
    default: {
      break;
    }
  }
  return QVariant();
}

MeasuredPoint TreeModel::getPoint(const QModelIndex &index)
{
  if (!index.isValid())
    return MeasuredPoint();
  const MeasuredPoint* ptr =
    static_cast<MeasuredPoint*>(index.internalPointer());
  return *ptr;
}

/*Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
     return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}*/

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0: return irs::str_conv<QString>(std::wstring(L"Тип"));
      case 1: return irs::str_conv<QString>(std::wstring(L"Режим"));
      case 2: return irs::str_conv<QString>(std::wstring(L"Метка"));
      case 3: return irs::str_conv<QString>(std::wstring(L"Отклонение"));
      case 4: return irs::str_conv<QString>(std::wstring(L"Ошибка, %"));
      case 5: return irs::str_conv<QString>(std::wstring(L"Приведенная, %"));
      case 6: return irs::str_conv<QString>(std::wstring(L"Статус"));
      case 7: return irs::str_conv<QString>(std::wstring(L"Дата"));
    }

    /*if (section == 0) {
      return irs::str_conv<QString>(std::wstring(L"Дата"));
    } else if (section == 1) {
      return irs::str_conv<QString>(std::wstring(L"Метка"));
    } else if (section == 2) {
      return irs::str_conv<QString>(std::wstring(L"Значение"));
    } else {
      return irs::str_conv<QString>(std::wstring(L"Абсолютная погрешность"));
    } else {

    }*/
  }
  return QVariant();
}

void TreeModel::setPoints(const MeasuredPoints& measuredPoints)
{
  this->measuredPoints = measuredPoints;
  //updateDisplayedPeople();
}

TableModel::TableModel(QObject* parent):
  QAbstractTableModel(parent)
{
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation,
  int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0: return irs::str_conv<QString>(std::wstring(L"Описание"));
      case 1: return irs::str_conv<QString>(std::wstring(L"Режим поверки"));
      case 2: return irs::str_conv<QString>(std::wstring(L"Номинал"));
      case 3: return irs::str_conv<QString>(std::wstring(L"Нижний лимит"));
      case 4: return irs::str_conv<QString>(std::wstring(L"Верхний лимит"));
      case 5: return irs::str_conv<QString>(std::wstring(L"Значение"));
      case 6: return irs::str_conv<QString>(std::wstring(L"Отклонение"));
      case 7: return irs::str_conv<QString>(std::wstring(L"Приведенная"));
    }
  } else if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
    return QVariant(QString::number(section));
  }
  return QVariant();
}

int TableModel::rowCount(const QModelIndex &parent) const
{
  return 5;
}

int TableModel::columnCount(const QModelIndex &parent) const
{
  return 8;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{


  if (role == Qt::DisplayRole) {
    QString str;

    if (index.row() == 0) {
      if (index.column() == 0) {
        str = irs::str_conv<QString>(std::wstring(L"DCV"));
      } else if (index.column() == 1) {
        std::wstring wstr = L"На поверяемом приборе переключите режим измерений на "
          L"постоянное напряжение и нажмите кнопку \"Ок, вперед\" "
          L"для начала измерений";
        str = irs::str_conv<QString>(wstr);
      }
    } else if (index.row() == 1) {
      if (index.column() == 1) {
        return irs::str_conv<QString>(std::wstring(L"DCV+ подход снизу"));
      }
    } else {
      str = QString("row = ") + QString::number(index.row()) + "  col = " +
        QString::number(index.column());
    }

    // строкой выше мы формируем ответ. QString::number преобразует число в текст
    return QVariant(str);
  } else if (role == Qt::BackgroundRole && index.row() == 0 && index.column() == 1) {
    QTableView* pLW = new QTableView;
    //QColor ColorBgrnd =palette().color(QPalette::Background);
    //QColor ColorBgrnd = pLW->palette().color(QPalette::Background);
    QColor ColorTxt =pLW->palette().color(QPalette::Disabled, QPalette::Background);
    delete pLW;
    return QVariant(ColorTxt);
  }
  return QVariant();
}

/*QSize	TableModel::span(const QModelIndex & index) const
{
  int a = 0;
  return QSize();
}*/

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  mp_settings(settings_t::instance()),
  m_stream_buf(100),
  mp_log_stream(new ofstream("hfftp.log", ios::out | ios::app)),
  mp_plain_text_edit_buf(),
  m_timer(),
  m_usb_hid_device_path_map(),
  m_calibrator_link(calibrator_settings_t()),
  m_calibrator(),
  m_calibrator_enabled(false),
  settingsDialog(0, mp_settings),
  model(NULL),
  tableModel(NULL),
  iconMovie(NULL),
  dcv_max(610),
  dci_max(11),
  acv_max(610),
  aci_max(11),
  coarse_step(0.005),
  normal_step(0.0005),
  fine_step(0.00005),
  m_setpoint(0),
  m_dcv_steps(),
  m_acv_steps(),
  m_dci_steps(),
  m_aci_steps(),
  mp_active_steps(&m_dcv_steps),
  m_fixed_step_buttons()
{
  ui->setupUi(this);

  std::string s2 = "\u30c6\u30b9\u30c8";
  std::string s0 = "\u0422\u0435\u0441\u0442\u043e\u0432\u044b\u0439";
  //ui->listWidget->font.setPixelSize(12);
  /*QFont font = ui->lineEdit_6->font();
  font.setPixelSize(24);
  ui->lineEdit_6->setFont(font);
  ui->listWidget->setFont(font);*/


  /*typedef codecvt<wchar_t, char, mbstate_t> convert_t;
  const convert_t& cdcvt = use_facet<convert_t>(locale());
  codecvt_base::result convert_result = cdcvt.out(state, from, from_end, from_next,
    to, to_end, to_next);*/
  //wchar_t d;
  /*std::locale wchar_cp1251(
    std::locale(""), new irs::codecvt_cp1251_t<wchar_t, char, std::mbstate_t>);
  std::locale cp1251_wchar(
    std::locale(""), new irs::codecvt_cp1251_t<char, wchar_t, std::mbstate_t>);*/
  /*std::stringstream ostr_in_wchar;
  ostr_in_wchar.imbue(wchar_cp1251)
  std::stringstream ostr_in_wchar;
  ostr_in_wchar.imbue(cp1251_wchar)*/

  //irs::codecvt_cp1251_t<wchar_t, char, std::mbstate_t> codecvt_wchar_cp1251;
  //irs::codecvt_cp1251_t<char, wchar_t, std::mbstate_t> codecvt_cp1251_wchar;
  //std::codecvt<char, wchar_t, mbstate_t> c1;

  /*mbstate_t state;
  char s4[] = "Тест";
  char s5[5];
  wchar_t s6[5];
  wchar_t s7[] = L"Тест";
  const wchar_t* p_instr_next = s7;
  char* p_outstr_next = s5;*/
  //std::codecvt_base::result convert_result = codecvt_wchar_cp1251.in(
    //state, s5, s5+5, p_instr_next, s6, s6+5, p_outstr_next);

  //std::codecvt_base::result convert_result = codecvt_cp1251_wchar.in(
    //state, s7, s7+5, p_instr_next, s5, s5+5, p_outstr_next);

  connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

  irs::loc();
  setlocale(LC_ALL, "Russian_Russia.1251");

  mp_plain_text_edit_buf.reset(new irs::plain_text_edit_buf(
    ui->logPlainTextEdit, plain_text_edit_buf_size));

  irs::mlog().rdbuf(&m_stream_buf);
  irs::mlog() << boolalpha;
  m_stream_buf.insert(mp_log_stream->rdbuf());
  m_stream_buf.insert(mp_plain_text_edit_buf.get());
  m_timer.start(10);

  IRS_DBG_MSG("Start");

  //ui->modeStackedWidget->setVisible(false);

  iconMovie = new QMovie(this);
  iconMovie->setFileName(":/Images/Images/process_red.GIF");
  iconMovie->setScaledSize(QSize(30, 30));
  iconMovie->start();

  ui->generatorStatusIconlabel->setAttribute(Qt::WA_NoSystemBackground);
  QPixmap pixmap(":/Images/Images/blueStone.png");
  ui->generatorStatusIconlabel->setPixmap(pixmap.scaled(QSize(30, 30),
    Qt::IgnoreAspectRatio, Qt::SmoothTransformation));


  QTreeWidgetItem *cities = new QTreeWidgetItem(ui->treeWidget);
  //cities->setText(0, tr("Cities"));
  //QTreeWidgetItem *osloItem = new QTreeWidgetItem(cities);
  cities->setText(0, tr("Oslo"));
  Qt::ItemFlags flags = cities->flags();
  flags |= Qt::ItemIsEditable;
  cities->setFlags(flags);
  cities->setText(1, tr("Yes"));

  mp_settings->load();

  m_calibrator_link.set_settings(mp_settings->calibrator_settings);
  m_calibrator_link.calibrator(&m_calibrator);

  connect(&settingsDialog, SIGNAL(apply()), this,
    SLOT(on_settingsDialog_apply()));

  /*fixed_step_t fs(0.001, irst("1 мВ"));
  fs.name = irst("1 мВ");
  string_type st = irst("1 мВ");*/

  //int s1 = fs.name.size();
  m_dcv_steps.push_back(fixed_step_t(0.001, irst("1 мВ")));
  //int s = m_dcv_steps[0].name.size();
  m_dcv_steps.push_back(fixed_step_t(0.01, irst("10 мВ")));
  m_dcv_steps.push_back(fixed_step_t(0.1, irst("100 мВ")));
  m_dcv_steps.push_back(fixed_step_t(1, irst("1 В")));
  m_dcv_steps.push_back(fixed_step_t(10, irst("10 В")));
  m_dcv_steps.push_back(fixed_step_t(20, irst("20 В")));
  m_dcv_steps.push_back(fixed_step_t(100, irst("100 В")));

  /*for (size_type i = 0; i < m_dcv_steps.size(); i++) {
    string_type str = m_dcv_steps[i].name;
  }*/

  m_acv_steps = m_dcv_steps;

  m_dci_steps.push_back(fixed_step_t(1e-6, irst("1 мкА")));
  m_dci_steps.push_back(fixed_step_t(10e-6, irst("10 мкА")));
  m_dci_steps.push_back(fixed_step_t(100e-6, irst("100 мкА")));
  m_dci_steps.push_back(fixed_step_t(0.001, irst("1 мА")));
  m_dci_steps.push_back(fixed_step_t(0.01, irst("10 мА")));
  m_dci_steps.push_back(fixed_step_t(0.1, irst("100 мА")));
  m_dci_steps.push_back(fixed_step_t(1, irst("1 А")));

  m_aci_steps.push_back(fixed_step_t(0.001, irst("1 мА")));
  m_aci_steps.push_back(fixed_step_t(0.005, irst("5 мА")));
  m_aci_steps.push_back(fixed_step_t(0.01, irst("10 мА")));
  m_aci_steps.push_back(fixed_step_t(0.05, irst("50 мА")));
  m_aci_steps.push_back(fixed_step_t(0.1, irst("100 мА")));
  m_aci_steps.push_back(fixed_step_t(0.5, irst("500 мА")));
  m_aci_steps.push_back(fixed_step_t(1, irst("1 А")));

  m_fixed_step_buttons.push_back(ui->fixedStep1PushButton);
  m_fixed_step_buttons.push_back(ui->fixedStep2PushButton);
  m_fixed_step_buttons.push_back(ui->fixedStep3PushButton);
  m_fixed_step_buttons.push_back(ui->fixedStep4PushButton);
  m_fixed_step_buttons.push_back(ui->fixedStep5PushButton);
  m_fixed_step_buttons.push_back(ui->fixedStep6PushButton);
  m_fixed_step_buttons.push_back(ui->fixedStep7PushButton);

  for (size_type i = 0; i < m_fixed_step_buttons.size(); i++) {
    connect(m_fixed_step_buttons[i], SIGNAL(clicked()), this,
      SLOT(on_fixedStepPushButton_clicked()));
  }

  tuneActions();
  tuneModeListWidgets();

  QPalette palette = ui->stepLineEdit->palette();
  //palette.setColor(QPalette::Window, Qt::black/*QApplication::palette().color(QPalette::Dark)*/);
  /*QColor c = QApplication::palette().color(QPalette::Active, QPalette::Highlight);
  palette.setColor(QPalette::Base, c);
  palette.setColor(QPalette::AlternateBase, c);
  palette.setColor(QPalette::ToolTipBase, c);
  palette.setColor(QPalette::ToolTipText, c);
  palette.setColor(QPalette::BrightText, c);
  palette.setColor(QPalette::Background, c);
  palette.setColor(QPalette::AlternateBase, c);
  ui->stepLineEdit->setPalette(palette);*/

  //QString sr = ui->stepLineEdit->styleSheet();
  //ui->stepLineEdit->setStyleSheet("background-color: WhiteSmoke");
  //ui->stepLineEdit->setStyleSheet(
  //  "QLineEdit:focus{border: 2px solid #006080;}");
  //ui->stepLineEdit->setStyleSheet("QLineEdit:focus{background-color: WhiteSmoke}");
  //ui->stepLineEdit->setStyleSheet("background-color: WhiteSmoke");
  //ui->stepLineEdit->setStyleSheet("selection-color: WhiteSmoke");

  //QColor c = QApplication::palette().color(QPalette::Active, QPalette::Highlight);
  QColor c = QApplication::palette().color(QPalette::Active, QPalette::Window);
  QString sheet = QString::fromLatin1(
    "QLineEdit{background-color: %1;}"
    "QLineEdit:hover{border: 1px solid gray; background-color 1%;}").arg(c.name());
  QString sheet_def = ui->stepLineEdit->styleSheet();
  ui->stepLineEdit->setStyleSheet(sheet);
  ui->stepLineEdit->setStyleSheet("");
  ui->stepLineEdit->installEventFilter(this);

  //ui->stepLineEdit->setStyleSheet(sheet_def);

  /*ui->stepLineEdit->setStyleSheet(
    "QLineEdit{background-color: WhiteSmoke;}"
    "QLineEdit:hover{border: 1px solid gray; background-color WhiteSmoke;}");
*/


  /*QColor c = ui->stepLineEdit->palette().color(QPalette::Background);
  QColor c1 = QPalette::Highlight;
  ui->stepLineEdit->setStyleSheet();
  //QApplication::palette();
  //ui->stepLineEdit->setStyleSheet("background-color: WhiteSmoke");
  QColor c2 = ui->stepLineEdit->palette().color(QPalette::Background);*/
  //ui->actualSetpointLineEdit->setStyleSheet("background-color: WhiteSmoke ");

  hideAllUnnecessary(true);

  ui->PointParametersTreeView->setRootIsDecorated(false);
  ui->PointParametersTreeView->setAlternatingRowColors(true);

  model = new TreeModel(this);
  ui->measuredPointsTreeView->setRootIsDecorated(false);
  ui->measuredPointsTreeView->setAlternatingRowColors(true);
  ui->measuredPointsTreeView->setModel(model);

  tableModel = new TableModel(this);
  ui->tableView->setModel(tableModel);
  ui->tableView->setSpan(0,1,1,8);



  //QAbstractItemModel* m = ui->tableView->model();
  //m->setData(m->index(0, 1), QBrush( Qt::red ), Qt::BackgroundRole);


  //ui->tableView->resizeRowToContents(2);
  //ui->tableView->resizeRowsToContents();

  //QList<QAbstractButton *> list = ui->buttonGroup_4->buttons();
  //qApp->

  #if DISABLE_MENU_ITEMS
  ui->showNavigationAction->setDisabled(true);
  ui->showMeasuredPointsTreeViewAction->setDisabled(true);
  ui->showLogAction->setDisabled(true);
  ui->hideNeedlessAction->setDisabled(true);
  ui->action_2->setDisabled(true);
  ui->action_3->setDisabled(true);
  ui->action_4->setDisabled(true);
  #endif // DISABLE_MENU_ITEMS
}

MainWindow::~MainWindow()
{
  mp_settings->save();
  delete ui;
}

void MainWindow::tick()
{
  for (int i = 0; i < 100; i++) { /// Для проверки
    m_usb_hid_device_path_map.tick();
    m_calibrator_link.tick();
  }

  updateDeviceList();
  autoConnectAnyDevice();
  autoConnect();

  const calibrator_link_t::status_t connection_status =
    m_calibrator_link.get_status();
  switch (connection_status) {
    case calibrator_link_t::status_disabled: {
      ui->connectionStatusLineEdit->setText(
        irs::str_conv<QString>(string_type(irst("Отключено"))));
      ui->connectionStatusLineEdit->
        setStyleSheet("background-color: WhiteSmoke ");
    } break;
    case calibrator_link_t::status_connected: {
      ui->connectionStatusLineEdit->setText(
        irs::str_conv<QString>(string_type(irst("Соединение установлено"))));
      ui->connectionStatusLineEdit->
        setStyleSheet("background-color: GreenYellow");
    } break;
    case calibrator_link_t::status_busy: {
      ui->connectionStatusLineEdit->setText(
        irs::str_conv<QString>(string_type(irst("Ожидание соединения"))));
      ui->connectionStatusLineEdit->setStyleSheet("background-color: yellow");
    } break;
    case calibrator_link_t::status_error: {
      ui->connectionStatusLineEdit->setText(
        irs::str_conv<QString>(string_type(irst("Ошибка соединения"))));
      ui->connectionStatusLineEdit->setStyleSheet("background-color: red");
    } break;
    default: {
      ui->connectionStatusLineEdit->setText(QString());
      ui->connectionStatusLineEdit->
        setStyleSheet("background-color: WhiteSmoke ");
    }
  }

  generator_mode_t generator_mode = generator_mode_dc_voltage;
  double setpoint = 0;
  m_calibrator.get_setpoint(&generator_mode, &setpoint);
  if (focusWidget() != ui->actualValueLineEdit) {

    string_type str;
    //number_to_string(m_calibrator.get_actual_value(), &str, 6, irs::num_mode_general);
    ui->actualValueLineEdit->setText(QString("%L1").arg(
      m_calibrator.get_actual_value(), 0, 'g', 6));
    //ui->actualValueLineEdit->setText(irs::str_conv<QString>(str));
  }
  ui->actualLabel->setText(
    irs::str_conv<QString>(string_type(irst("Текущее значение (Уставка: "))) +
    QString("%L1").arg(setpoint, 0, 'g', 6) + QString(")"));

  #if DEVICE_PARAMETERS_FORM_ENABLED
  deviceParametersForm->setSetpoint(setpoint);
  #endif // DEVICE_PARAMETERS_FORM_ENABLED


  const QListWidgetItem* item = ui->modeListWidget->currentItem();
  if (ui->modeStackedWidget->currentWidget() == ui->statePage) {
    // Установка не поддерживает часовые пояса, поэтому функция выдает локальное
    // время
    time_t device_time = m_calibrator.get_time();
    char buff[22];
    strftime(buff, IRS_ARRAYSIZE(buff), "%H:%M:%S   %d.%m.%Y",
      gmtime(&device_time));
    std::string s(buff);
    ui->deviceTimeLineEdit->setText(irs::str_conv<QString>(s));

    time_t now = time(NULL);
    strftime(buff, IRS_ARRAYSIZE(buff), "%H:%M:%S   %d.%m.%Y", localtime(&now));
    s = buff;
    ui->pcTimeLineEdit->setText(irs::str_conv<QString>(s));

    double bat_voltage = m_calibrator.get_battery_voltage();
    /*double battery_v_max = 3.5;
    double battery_v_min = 1.7;
    double bat = (bat_voltage - battery_v_min)/
      (battery_v_max - battery_v_min);
    int bat_percent = irs::round<double, int>(100*bat_percent);
    bat_percent = irs::range(bat_percent, 0, 100);*/
    const int precision = 4;
    ui->batteryValueLineEdit->setText(QString("%L1").arg(
      bat_voltage, 0, 'g', precision));
  }
  /*ui->modeStackedWidget->currentIndex()

  ui->statePage*/


  if (m_calibrator_enabled) {


    setGeneralModeToUI(generator_mode);


    //ui->referencePointLineEdit->setText(QString("%1").arg(setpoint));

    //ui->actualSetpointLineEdit->setText(QString("%1").arg(setpoint));
    /*ui->actualSetpointLabel->setText(
      irs::str_conv<QString>(std::wstring(L"Текущая уставка ")) +
      QString("%1").arg(setpoint));*/


    const generator_status_t generator_status = m_calibrator.get_status();
    switch (generator_status) {
      case generator_status_busy: {
        ui->generatorStatusLineEdit->setText(
          irs::str_conv<QString>(string_type(irst("Ожидание выхода на режим"))));
        ui->generatorStatusIconlabel->setMovie(iconMovie);
        //ui->generatorStatusLineEdit->
          //setStyleSheet("background-color: yellow");
      } break;
      case generator_status_ready: {
        if (setpoint) {
          ui->generatorStatusLineEdit->setText(
            irs::str_conv<QString>(string_type(irst("Вышел на режим"))));
          QPixmap pixmap(":/Images/Images/greenStone.png");
          ui->generatorStatusIconlabel->setPixmap(pixmap.scaled(QSize(30, 30)));

          //ui->generatorStatusLineEdit->
            //setStyleSheet("background-color: GreenYellow");
        } else {
          ui->generatorStatusLineEdit->setText(
            irs::str_conv<QString>(string_type(irst("Выключен"))));
          QPixmap pixmap(":/Images/Images/blueStone.png");
          ui->generatorStatusIconlabel->setPixmap(pixmap.scaled(QSize(30, 30)));
          //ui->generatorStatusLineEdit->
            //setStyleSheet("background-color: WhiteSmoke");
        }
      } break;
    }
  } else {
    if (connection_status == calibrator_link_t::status_connected) {
      ui->generatorStatusLineEdit->setText(
        irs::str_conv<QString>(string_type(irst("Подключен"))));
    } else {
      ui->generatorStatusLineEdit->setText(
        irs::str_conv<QString>(string_type(irst("Отключен"))));
    }
    ui->generatorStatusLineEdit->
      setStyleSheet("background-color: WhiteSmoke");
  }
  /*if (m_synchronization_setpoint_enabled) {
    ui->synchronizationSetpointStatusLabel->setText(irs::str_conv<QString>(
      string_type(irst("Синхронизация включена"))));
    ui->synchronizationSetpointStatusLabel->setStyleSheet("color: Green");

  } else {
    ui->synchronizationSetpointStatusLabel->setText(irs::str_conv<QString>(
      string_type(irst("Синхронизация выключена"))));
    ui->synchronizationSetpointStatusLabel->setStyleSheet("color: rgb(50, 0, 70)");
  }*/
}

void MainWindow::on_settingsAction_triggered()
{
  //SettingsDialog dialog(0, mp_settings);
  settingsDialog.exec();
}

void MainWindow::on_pushButton_clicked()
{
  /*QAxObject* WordApplication=new QAxObject("Word.Application"); // Создаю интерфейс к MSWord
  QAxObject* WordDocuments = WordApplication->querySubObject( "Documents()" ); // Получаю интерфейсы к его подобъекту "коллекция открытых документов":
  QAxObject* NewDocument = WordDocuments->querySubObject( "Add()" ); // Создаю новый документ
  WordApplication->setProperty("Visible", true); // Делаем Word видимым
  int i = 0;*/
}

/*void MainWindow::focusInEvent(QFocusEvent * event)
{
  ;
}*/

bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
  if (obj == ui->stepLineEdit) {
      if (ev->type() == QEvent::FocusIn) {
        uncheckFixedStepButtons();
        ui->stepLineEdit->setStyleSheet("");
      }
      return false;
  } else {
      // Пересылка сообщения родительскому классу
      return MainWindow::eventFilter(obj, ev);
  }
}


void MainWindow::wheelEvent (QWheelEvent * event)
{
  const int delta = event->delta();
  const int k = delta >= 0 ? 1 : -1;
  int numDegrees = abs(event->delta()/8);
  int numSteps = std::max(numDegrees/15, 1);
  numSteps *= k;

  const Qt::KeyboardModifiers keys = event->modifiers();
  if (keys & Qt::ShiftModifier) {
    changeSetpoint(numSteps*coarse_step);
  } else if (keys & Qt::ControlModifier) {
    changeSetpoint(numSteps*fine_step);
  } else {
    changeSetpoint(numSteps*normal_step);
  }
  /*switch(event->modifiers()) {
    case Qt::ShiftModifier: {

    } break;
    case Qt::ControlModifier: {

    } break;
    default:
  }*/
  event->accept();
}

/*void MainWindow::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
    case Qt::Key_Home:

    case Qt::Key_End:

    default:
      QWidget::keyPressEvent(event);
  }
}*/

void MainWindow::tuneActions()
{
  //connect(ui->settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
}

void MainWindow::tuneModeListWidgets()
{
  QListWidgetItem* item = new QListWidgetItem(
    QString(irs::str_conv<QString>(string_type(irst("Режим генератора")))),
    ui->modeListWidget);
  item->setData(Qt::UserRole,
    ui->modeStackedWidget->indexOf(ui->generatorModePage));

  item = new QListWidgetItem(
      QString(irs::str_conv<QString>(string_type(irst("Поверка без шаблона")))),
      ui->modeListWidget);
  item->setData(Qt::UserRole,
    ui->modeStackedWidget->indexOf(ui->verificationWithoutTemplate));

  item = new QListWidgetItem(
      QString(irs::str_conv<QString>(string_type(irst("Поверка без шаблона2")))),
      ui->modeListWidget);
  item->setData(Qt::UserRole,
    ui->modeStackedWidget->indexOf(ui->manualModePage2));

  item = new QListWidgetItem(
    QString(irs::str_conv<QString>(
    string_type(irst("Поверка по шаблону")))),
    ui->modeListWidget);
  item->setData(Qt::UserRole,
    ui->modeStackedWidget->indexOf(ui->VerificationDevicesPage));

  item = new QListWidgetItem(
    QString(irs::str_conv<QString>(string_type(irst("Шаблоны поверки")))),
    ui->modeListWidget);
  item->setData(Qt::UserRole,
    ui->modeStackedWidget->indexOf(ui->templatesVerificationPage));

  item = new QListWidgetItem(
    QString(irs::str_conv<QString>(string_type(irst("Результаты поверки")))),
    ui->modeListWidget);
  item->setData(Qt::UserRole,
    ui->modeStackedWidget->indexOf(ui->page));

  item = new QListWidgetItem(
    QString(irs::str_conv<QString>(string_type(irst("Состояние устройства")))),
    ui->modeListWidget);
  item->setData(Qt::UserRole,
    ui->modeStackedWidget->indexOf(ui->statePage));

  item = new QListWidgetItem(
    QString(irs::str_conv<QString>(string_type(irst("Тест1")))),
    ui->modeListWidget);
  item->setData(Qt::UserRole,
    ui->modeStackedWidget->indexOf(ui->page_2));

  item = new QListWidgetItem(
    QString(irs::str_conv<QString>(string_type(irst("Тест2")))),
    ui->modeListWidget);
  item->setData(Qt::UserRole,
    ui->modeStackedWidget->indexOf(ui->generatorModePage2));

  ui->modeListWidget->setCurrentRow(0);

  QWidget* gen = new GeneratorModeForm(this, &m_calibrator_link,
    &m_calibrator, mp_settings);

  #if DEVICE_PARAMETERS_FORM_ENABLED
  deviceParametersForm = new DeviceParametersForm(this);
  #endif // DEVICE_PARAMETERS_FORM_ENABLED

  QWidget* points = new QTreeWidget;

  QVBoxLayout *vLayout = new QVBoxLayout;
  #if DEVICE_PARAMETERS_FORM_ENABLED
  vLayout->addWidget(deviceParametersForm);
  vLayout->addWidget(points);
  #endif // DEVICE_PARAMETERS_FORM_ENABLED

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(gen);
  #if DEVICE_PARAMETERS_FORM_ENABLED
  layout->addLayout(vLayout);
  #endif // DEVICE_PARAMETERS_FORM_ENABLED
  //layout->addWidget(gen);
  //layout->addWidget(dev);
  ui->generatorModePage->setLayout(layout);
  //ui->page_3->layout()->addWidget(gen);
  //ui->page_3->layout()->addWidget(dev);
}

void MainWindow::autoConnect()
{
  // Если включено подключение к любому устройсву, то выходим
  if (ui->autoConnectAnyDevicecheckBox->isChecked()) {
    return;
  }
  if (!ui->autoconnectCheckBox->isChecked()) {
    return;
  }
  tryConnect();
}

void MainWindow::tryConnect()
{
  if (ui->devicesComboBox->currentIndex() < 0) {
    return;
  }
  if (m_calibrator_link.enabled()) {
    return;
  }
  string_type path = irs::str_conv<string_type>(
    ui->devicesComboBox->currentData(Qt::UserRole).toString());
  mp_settings->calibrator_settings.device_path = path;
  m_calibrator_link.set_settings(mp_settings->calibrator_settings);
  m_calibrator_link.enabled(true);
  ui->connectPushButton->setEnabled(false);
  ui->disconnectPushButton->setEnabled(true);
}

void MainWindow::updateDeviceList()
{
  if (m_usb_hid_device_path_map.changed()) {
    const std::map<string_type, string_type> devs =
      m_usb_hid_device_path_map.get_map();
    std::map<string_type, string_type>::const_iterator it =
      devs.begin();
    ui->devicesComboBox->clear();
    while (it != devs.end()) {
      const QString name = irs::str_conv<QString>(it->first);
      const QString path = irs::str_conv<QString>(it->second);
      ui->devicesComboBox->addItem(name, path);
      ++it;
    }
    updateConnectPushButton();
  }
}

void MainWindow::autoConnectAnyDevice()
{
  if (!ui->autoConnectAnyDevicecheckBox->isChecked()) {
    return;
  }
  tryConnect();
  /*if (m_calibrator_link.enabled()) {
    return;
  }
  if (ui->devicesComboBox->currentIndex() < 0) {
    return;
  }

  const string_type path = irs::str_conv<string_type>(
    ui->devicesComboBox->currentData(Qt::UserRole).toString());
  mp_settings->calibrator_settings.device_path = path;
  m_calibrator_link.set_settings(mp_settings->calibrator_settings);
  m_calibrator_link.enabled(true);
  ui->connectPushButton->setEnabled(false);
  ui->disconnectPushButton->setEnabled(true);*/
}

void MainWindow::updateConnectPushButton()
{
  const std::map<string_type, string_type> devs =
    m_usb_hid_device_path_map.get_map();
  ui->connectPushButton->setEnabled(devs.size() && !m_calibrator_link.enabled());
}

generator_mode_t MainWindow::getGeneratorModeFromUI() const
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

void MainWindow::setGeneralModeToUI(generator_mode_t a_mode)
{
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
}

void MainWindow::on_pushButton_4_clicked()
{

}

/*void MainWindow::on_modeListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{

}*/

void MainWindow::on_modeListWidget_itemSelectionChanged()
{
  const QListWidgetItem* item = ui->modeListWidget->currentItem();
  const QVariant data = item->data(Qt::UserRole);
  const int page_index = data.toInt();
  if (page_index < ui->modeStackedWidget->count()) {
    ui->modeStackedWidget->setCurrentIndex(page_index);
  }
}

void MainWindow::on_settingsDialog_apply()
{
  mp_settings->save();
}

void MainWindow::on_connectPushButton_clicked()
{
  tryConnect();
}

void MainWindow::on_disconnectPushButton_clicked()
{
  m_calibrator_link.enabled(false);
  updateConnectPushButton();
  ui->disconnectPushButton->setEnabled(false);
  ui->autoconnectCheckBox->setChecked(false);
}

void MainWindow::on_referencePointLineEdit_returnPressed()
{
  setSetpointToCalibratorFromRefPointUI();
}

void MainWindow::on_setSetpoinPushButton_clicked()
{
  m_calibrator_enabled = true;
  setSetpointToCalibratorFromRefPointUI();
  ui->setSetpoinPushButton->setChecked(true);
  ui->stopPushButton->setChecked(false);
}

void MainWindow::hideAllUnnecessary(bool enable)
{
  const bool visible = !enable;
  ui->groupBox->setVisible(visible);
  ui->modeListWidget->setVisible(visible);

  ui->measuredPointsTreeView->setVisible(visible);

  ui->logPlainTextEdit->setVisible(visible);
}

double MainWindow::getReferencePointFromUI() const
{
  QString setpoint_str = "0";//ui->referencePointLineEdit->text();
  double setpoint = 0;
  irs::str_to_num(setpoint_str, &setpoint);
  return setpoint;
}

double MainWindow::getActualValueFromUI() const
{
  QString setpoint_str = ui->actualValueLineEdit->text();
  double setpoint = 0;
  irs::str_to_num(setpoint_str, &setpoint);
  return setpoint;
}

void MainWindow::setSetpointToCalibratorFromRefPointUI()
{
  double setpoint = getReferencePointFromUI();
  setSetpointToCalibrator(setpoint);
}

void MainWindow::setSetpointToCalibratorFromActualValueUI()
{
  double setpoint = getActualValueFromUI();
  setSetpointToCalibrator(setpoint);
}

void MainWindow::setSetpointToCalibrator(double a_setpoint)
{
  m_setpoint = a_setpoint;
  if (m_calibrator_enabled) {
    const generator_mode_t generator_mode = getGeneratorModeFromUI();
    m_calibrator.set_setpoint(generator_mode, a_setpoint);
  }
}

void MainWindow::on_stopPushButton_clicked()
{
  m_calibrator_enabled = false;
  const generator_mode_t generator_mode = getGeneratorModeFromUI();
  double setpoint = 0;
  m_calibrator.set_setpoint(generator_mode, setpoint);

  ui->setSetpoinPushButton->setChecked(false);
  ui->stopPushButton->setChecked(true);
}

void MainWindow::on_modeDCVPushButton_clicked()
{
  generator_mode_t generator_mode = generator_mode_dc_voltage;
  setGeneralModeToUI(generator_mode);
  updateFixedSteps();
  double setpoint = 0;
  m_calibrator.set_setpoint(generator_mode, setpoint);
}

void MainWindow::on_modeACVPushButton_clicked()
{
  generator_mode_t generator_mode = generator_mode_ac_voltage;
  setGeneralModeToUI(generator_mode);
  updateFixedSteps();
  double setpoint = 0;
  m_calibrator.set_setpoint(generator_mode, setpoint);
}

void MainWindow::on_modeDCIPushButton_clicked()
{
  generator_mode_t generator_mode = generator_mode_dc_current;
  setGeneralModeToUI(generator_mode);
  updateFixedSteps();
  double setpoint = 0;
  m_calibrator.set_setpoint(generator_mode, setpoint);
}

void MainWindow::on_modeACIPushButton_clicked()
{
  generator_mode_t generator_mode = generator_mode_ac_current;
  setGeneralModeToUI(generator_mode);
  updateFixedSteps();
  double setpoint = 0;
  m_calibrator.set_setpoint(generator_mode, setpoint);
}

void MainWindow::on_referencePointLineEdit_editingFinished()
{
  //setSetpointToCalibratorFromUI();
}

void MainWindow::on_copyActualValuePushButton_clicked()
{
  QApplication::clipboard()->setText(ui->actualValueLineEdit->text());
}

void MainWindow::on_reduceSetpointPushButton_clicked()
{
  QString step_str = ui->stepLineEdit->text();
  double step = 0;
  irs::str_to_num(step_str, &step);
  double setpoint = getReferencePointFromUI();
  setpoint -= step;
  QString setpoint_str;
  irs::num_to_str(setpoint, &setpoint_str);
  //ui->referencePointLineEdit->setText(setpoint_str);
}

void MainWindow::on_showNavigationAction_triggered(bool checked)
{
  ui->groupBox->setVisible(checked);
  ui->modeListWidget->setVisible(checked);
}

void MainWindow::on_showLogAction_triggered(bool checked)
{
  ui->logPlainTextEdit->setVisible(checked);
}

void MainWindow::on_showMeasuredPointsTreeViewAction_triggered(bool checked)
{
  ui->measuredPointsTreeView->setVisible(checked);
}

void MainWindow::on_hideNeedlessAction_triggered(bool checked)
{
  hideAllUnnecessary(checked);
}

void MainWindow::on_actualValueLineEdit_returnPressed()
{
  setSetpointToCalibratorFromActualValueUI();
}

double MainWindow::getMaxSetpoint() const
{
  switch (getGeneratorModeFromUI()) {
    case generator_mode_dc_voltage: return dcv_max;
    case generator_mode_ac_voltage: return acv_max;
    case generator_mode_dc_current: return dci_max;
    case generator_mode_ac_current: return aci_max;
  }
  return 0;
}

void MainWindow::changeSetpoint(double a_step)
{
  const double ref_point = getReferencePointFromUI();
  m_setpoint = m_setpoint + ref_point*a_step;
  normalizeSetpoint(&m_setpoint);
  setSetpointToCalibrator(m_setpoint);
}

void MainWindow::increaseSetpoint(double a_step)
{
  changeSetpoint(a_step);
  /*const double ref_point = getReferencePointFromUI();
  m_setpoint += ref_point*a_step;
  normalizeSetpoint(&m_setpoint);
  setSetpointToCalibrator(m_setpoint);*/
}

void MainWindow::reduceSetpoint(double a_step)
{
  changeSetpoint(-a_step);
  /*const double ref_point = getReferencePointFromUI();
  m_setpoint -= ref_point*a_step;
  normalizeSetpoint(&m_setpoint);
  setSetpointToCalibrator(m_setpoint);*/
}

void MainWindow::normalizeSetpoint(double* ap_setpoint) const
{
  *ap_setpoint = irs::range<double>(*ap_setpoint, 0, getMaxSetpoint());
}

void MainWindow::normalizeStep(double* ap_value) const
{
  *ap_value = irs::range<double>(*ap_value, 0, getMaxSetpoint()/2);
}

void MainWindow::updateFixedSteps()
{
  switch (getGeneratorModeFromUI()) {
    case generator_mode_dc_voltage: {
      mp_active_steps = &m_dcv_steps;
    } break;
    case generator_mode_ac_voltage: {
      mp_active_steps = &m_acv_steps;
    } break;
    case generator_mode_dc_current: {
      mp_active_steps = &m_dci_steps;
    } break;
    case generator_mode_ac_current: {
      mp_active_steps = &m_aci_steps;
    } break;
  }

  for (size_type i = 0; i < m_fixed_step_buttons.size(); i++) {
    if (i < mp_active_steps->size()) {
      string_type str = (*mp_active_steps)[i].name;
      m_fixed_step_buttons[i]->setText(
        irs::str_conv<QString>((*mp_active_steps)[i].name));
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


  //ui->fixedStep1PushButton->setChecked(true);
  setFixedStepButtonsChecked(0);
}

void MainWindow::setFixedStepButtonsChecked(size_type a_index)
{
  for (size_type i = 0; i < m_fixed_step_buttons.size(); i++) {
    const bool checked = (i == a_index);
    m_fixed_step_buttons[i]->setChecked(checked);
  }
}

void MainWindow::uncheckFixedStepButtons()
{
  for (size_type i = 0; i < m_fixed_step_buttons.size(); i++) {
    m_fixed_step_buttons[i]->setChecked(false);
  }
}

double MainWindow::getFixedStep() const
{
  for (size_type i = 0; i < m_fixed_step_buttons.size(); i++) {
    if (m_fixed_step_buttons[i]->isChecked()) {
      if (i < mp_active_steps->size()) {
        return (*mp_active_steps)[i].value;
      }
      break;
    }
  }
  double step = 0;
  if (!irs::str_to_num(ui->stepLineEdit->text(), &step)) {
    step = 0;
  }
  normalizeStep(&step);
  return step;
}

void MainWindow::on_coarseIncreasePushButton_clicked()
{
  increaseSetpoint(coarse_step);
}

void MainWindow::on_coarseReducePushButton_clicked()
{
  reduceSetpoint(coarse_step);
}

void MainWindow::on_normalIncreasePushButton_clicked()
{
  increaseSetpoint(normal_step);
}

void MainWindow::on_normalReducePushButton_clicked()
{
  reduceSetpoint(normal_step);
}

void MainWindow::on_fineIncreasePushButton_clicked()
{
  increaseSetpoint(fine_step);
}

void MainWindow::on_fineReducePushButton_clicked()
{
  reduceSetpoint(fine_step);
}

void MainWindow::on_fixedStepPushButton_clicked()
{
  QPushButton* button = dynamic_cast<QPushButton*>(QObject::sender());
  for (size_type i = 0; i < m_fixed_step_buttons.size(); i++) {
    const bool checked = button == m_fixed_step_buttons[i];
    m_fixed_step_buttons[i]->setChecked(checked);
  }
  QColor c = QApplication::palette().color(QPalette::Active, QPalette::Window);
  QString sheet = QString::fromLatin1(
    "QLineEdit{background-color: %1;}"
    "QLineEdit:hover{border: 1px solid gray; background-color 1%;}").arg(c.name());
  QString sheet_def = ui->stepLineEdit->styleSheet();
  ui->stepLineEdit->setStyleSheet(sheet);
}

void MainWindow::on_fixedIncreasePushButton_clicked()
{
  const double step = getFixedStep();
  increaseSetpoint(step);
}

void MainWindow::on_fixedReducePushButton_clicked()
{
  const double step = getFixedStep();
  reduceSetpoint(step);
}

void MainWindow::on_stepLineEdit_selectionChanged()
{
}

void MainWindow::on_pushButton_22_clicked()
{
  EditTemplateDialog dialog(0);
  dialog.exec();
}

void MainWindow::on_pushButton_28_clicked()
{
  //docscreator::msword_t msword;



  QAxObject *Wa = new QAxObject ("Word.Application");
  QAxObject *docs = Wa->querySubObject("Documents()");
  QString fileName = "C:\\Temp\\12.doc";
  QString fileName2 = irs::str_conv<QString>(std::wstring(L"C:\\Temp\\1234"));
  //QAxObject* doc = docs->querySubObject("Open(const QVariant&)", fileName);

  QAxObject* doc = docs->querySubObject("Add()");




  doc->dynamicCall("SaveAs(const QVariant&)", fileName2);

  Wa->setProperty("Visible", true);



  const int wd_collapse_end = 0;
  const int wd_collapse_start = 1;

  const int wd_find_stop = 0;

  const int wd_replace_one = 1;


  std::shared_ptr<QAxObject> range(doc->querySubObject("Range()"));


  /*range->dynamicCall("Collapse(QVariant&)",
    static_cast<int>(wd_collapse_start));

  range->dynamicCall("Collapse(QVariant&)",
    static_cast<int>(wd_collapse_end));
*/
  range->dynamicCall("InsertAfter(Text)","1");
  range->dynamicCall("InsertAfter(QString)","2");
  range->querySubObject("InsertAfter(QString)","3");

  range->dynamicCall("InsertAfter(QVariant&)",
    irs::str_conv<QString>(std::wstring(L"98765")));

  doc->dynamicCall("Save()");
  /*QAxObject* content = doc->querySubObject("Content");
  //content->dynamicCall("InsertAfter (Text:=QVariant&)", "hello");
  //content = doc->querySubObject("Content");
  QAxObject* selection = Wa->querySubObject("Selection");
  QAxObject* find = selection->querySubObject("Find");
*/

  std::shared_ptr<QAxObject> find(range->querySubObject("Find"));
  //find->dynamicCall("ClearFormatting()");

  const bool match_case = true;
  const bool match_whole_word = false;
  const bool match_wildcards = false;
  const bool match_sounds_like = false;
  const bool match_all_word_forms = false;
  const bool forward = true;
  const int wrap = wd_find_stop;
  const bool format = false;
  const QString replace_with ;
  const int replace = wd_replace_one;

  QList<QVariant> list;
  list.append(irs::str_conv<QString>(std::wstring(L"hello")));
  /*list.append(match_case);
  list.append(match_whole_word);
  list.append(match_wildcards);
  list.append(match_sounds_like);
  list.append(match_all_word_forms);
  list.append(forward);
  list.append(wrap);
  list.append(format);
  */
  list.append(QVariant());
  list.append(QVariant());
  list.append(QVariant());
  list.append(QVariant());
  list.append(QVariant());
  list.append(QVariant());
  list.append(QVariant());
  list.append(QVariant());
  list.append(irs::str_conv<QString>(std::wstring(L"olleh")));
  list.append(2);

  list.append(QVariant());
  list.append(QVariant());
  list.append(QVariant());
  list.append(QVariant());

  connect(find.get(), SIGNAL(exception(int, const QString&, const QString&, const QString&)), this, SLOT(catchException(int, const QString&, const QString&, const QString&)));

  QVariant res = find->dynamicCall("Execute(QVariant&, QVariant&, "
    "QVariant&, QVariant&, QVariant&, QVariant&, QVariant&, QVariant&, "
    "QVariant&, QVariant&, QVariant&, QVariant&, QVariant&, QVariant&, QVariant&)", list);
  bool r = res.toBool();

  //Wd-> .OleProcedure("SaveAs", irs::str_conv<WideString>(m_file_name));
  //doc->querySubObject("SaveAs(const QVariant&)", fileName2);
  //var_document.OleProcedure("Close", wd_do_not_save_changes);
  //int wd_save_changes = 1;
  //doc->querySubObject("Close(QVariant)", wd_save_changes);
  doc->dynamicCall("Save()");

  //doc->dynamicCall("Close()");
  int wd_do_not_save_changes = 0;
  doc->dynamicCall("Close(QVariant)", wd_do_not_save_changes);

  //Wa->dynamicCall("Quit()");

  delete doc;

  docs->clear();
  delete docs;

  Wa->dynamicCall("Quit", 0);
  delete Wa;



  /*doc->querySubObject("SaveAs(const Variant&)",
    QDir::currentPath().append("\\Test123.doc"));*/
}

void MainWindow::on_pushButton_53_clicked()
{
  MakeResultDialog dialog(0);
  dialog.exec();
}

void MainWindow::on_pushButton_2_clicked()
{
  docscreator::msword_t msword;
  string_type fileName = irst("C:\\Temp\\123.doc");
  string_type fileName2 = irst("C:\\Temp\\1234");
  docscreator::msdocument_t* doc = msword.document_create(fileName);
  doc->save(fileName2);

  // Регулярное выражение для поиска тегов \%\([A-zА-я0-9_]@\)

  string_type text = doc->text_get();

  std::wofstream ofile("C:\\Temp\\text.txt");
  ofile << text;
  ofile.close();

  //qDebug() << irs::str_conv<QString>(text);

  doc->text_replace(irst("%(наименование_прибора)"), irst("v7-78/1"));
  doc->save();
}

void MainWindow::on_pushButton_3_clicked()
{
  QAxObject *Wa = new QAxObject ("Word.Application");
  QAxObject *docs = Wa->querySubObject("Documents()");

  QString fileName2 = irs::str_conv<QString>(std::wstring(L"C:\\Temp\\1234"));


  QAxObject* doc = docs->querySubObject("Add()");


  //doc->dynamicCall("SaveAs(const QVariant&)", fileName2);

  Wa->setProperty("Visible", true);



  const int wd_collapse_end = 0;
  const int wd_collapse_start = 1;

  const int wd_find_stop = 0;

  const int wd_replace_one = 1;




  std::shared_ptr<QAxObject> range =
    std::shared_ptr<QAxObject>(doc->querySubObject("Range()"));


  range->dynamicCall("InsertAfter(Text)","1");
  range->dynamicCall("InsertAfter(QVariant)","2");
  range->querySubObject("InsertAfter(QString)","3");

  //range->dynamicCall("InsertAfter(QVariant&)",
    //irs::str_conv<QString>(std::wstring(L"98765")));
}

void MainWindow::on_pushButton_5_clicked()
{
  QAxObject *Wa = new QAxObject ("Word.Application");
  QAxObject *docs = Wa->querySubObject("Documents()");
  QString fileName = "C:\\Temp\\123.doc";
  QString fileName2 = irs::str_conv<QString>(std::wstring(L"C:\\Temp\\1234"));
  QAxObject* doc = docs->querySubObject("Open(QVariant)", fileName);

  doc->dynamicCall("SaveAs(const QVariant&)", fileName2);

  Wa->setProperty("Visible", true);

  const int wd_collapse_end = 0;
  const int wd_collapse_start = 1;

  const int wd_find_stop = 0;

  const int wd_replace_one = 1;


  std::shared_ptr<QAxObject> range(doc->querySubObject("Range()"));

  range->dynamicCall("Collapse(QVariant&)",
      static_cast<int>(wd_collapse_start));

  //range->dynamicCall("Collapse(QVariant&)",
    //static_cast<int>(wd_collapse_end));

  //range->dynamicCall("InsertAfter(Text)","abc");



  QAxObject* table3 = doc->querySubObject("Tables(3)");

  range.reset(table3->querySubObject("Range()"));

  std::shared_ptr<QAxObject> find(range->querySubObject("Find()"));
  //find->dynamicCall("ClearFormatting()");

  const bool match_case = true;
  const bool match_whole_word = false;
  const bool match_wildcards = false;
  const bool match_sounds_like = false;
  const bool match_all_word_forms = false;
  const bool forward = true;
  const int wrap = wd_find_stop;
  const bool format = false;
  const QString replace_with = "%(отсчет)";
  const int replace = wd_replace_one;

  QList<QVariant> list;
  list.append("%(отсчет)");
  list.append(match_case);
  list.append(match_whole_word);
  list.append(match_wildcards);
  list.append(match_sounds_like);
  list.append(match_all_word_forms);
  list.append(forward);
  list.append(wrap);
  list.append(format);
  list.append(replace_with);
  list.append(replace);

  connect(find.get(), SIGNAL(exception(int, const QString&, const QString&, const QString&)), this, SLOT(catchException(int, const QString&, const QString&, const QString&)));

  QVariant res = find->dynamicCall("Execute(QVariant, QVariant, "
    "QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, "
    "QVariant, QVariant, QVariant)", list);


  bool r = res.toBool();
  qDebug() << "r: " << r;


  QAxObject* cell = range->querySubObject("Cells(1)");
  int r_index = cell->property("RowIndex").toInt();
  int c_index = cell->property("ColumnIndex").toInt();
  cell->dynamicCall("Select()");
  QAxObject* selection = Wa->querySubObject("Selection()");
  selection->dynamicCall("InsertRowsBelow()");

  doc->dynamicCall("Save()");


  ////////////////////
  /*range.reset(doc->querySubObject("Range(QVariant, QVariant)", range->querySubObject("")));
  find.reset(range->querySubObject("Find()"));

  bool r = res.toBool();*/
  ///////////////////

  QAxObject* tables = range->querySubObject("Tables()");
  QVariant count = tables->property("Count");
  int c = count.toInt();
  QAxObject* Table = range->querySubObject("Tables(1)");

  int c_count = Table->querySubObject("Columns")->property("Count").toInt();
  int r_count = Table->querySubObject("Rows")->property("Count").toInt();

  QAxObject* Rows = Table->querySubObject("Rows()");
  int rc = Rows->property("Count").toInt();


  //QAxObject* row = Rows->querySubObject("Item(QVariant)", 4);
  //Rows->dynamicCall("Add()");
  //QAxObject* row = Rows->querySubObject("Item(QVariant)", 4);
  rc = Rows->property("Count").toInt();



  /*QAxObject* row = Rows->querySubObject("Item(QVariant)", 4);
  int index = row->property("Index").toInt();
  row->dynamicCall("Delete()");
  QAxObject* cells = row->querySubObject("Cells()");*/






  /*QAxObject* Tables = doc->querySubObject("Tables()");
  QVariant Count = Tables->property("Count");
  int C = Count.toInt();
  QAxObject* NewTable = Tables->querySubObject("Add(const QVariant&, const QVariant&, const QVariant&, const QVariant&, const QVariant&)", doc->dynamicCall("Range()"), 1, 3, 0, 2);

  //std::shared_ptr<QAxObject> range(doc->querySubObject("ActiveTable()"));
  QAxObject* range2 = doc->querySubObject("Tables(1)");

  QAxObject* rows = range->querySubObject("Rows()");
*/



}

void MainWindow::on_pushButton_6_clicked()
{
  ui->listWidget->clear();

  //QRegExp rx("%\\((\W+)\\)");
  //QRegExp rx("%\\(([0-9A-zА-я]+)\\)");
  QRegExp rx("%\\(([0-9A-Za-zА-Яа-я_]+)\\)");
  rx.setPatternSyntax(QRegExp::RegExp2);
  QString str = "dsf%(Hello)234fds %(при_вет) %(привет_как_дела) %(ПриВет) %(При вет) %(1ПриВет)";
  ui->textEdit_3->setText(str);
  QStringList list;
  int pos = 0;

  while ((pos = rx.indexIn(str, pos)) != -1) {
     list << rx.cap(1);

     ui->listWidget->addItem(rx.cap(1));

     pos += rx.matchedLength();
  }
  qDebug() << list;
}

void MainWindow::on_pushButton_7_clicked()
{
  vector<int> vec;
  vec.push_back(1);
  vec.push_back(2);
  vec.push_back(3);
  for (auto& v: vec) {
    qDebug() << "v #" << v << '\n';
  }

  map<string_type, size_type> vars;
  vars.insert(make_pair(irst("Привет"), 1));
  vars.insert(make_pair(irst("str2"), 2));
  vars.insert(make_pair(irst("str3"), 3));
  /*<< irs::str_conv<QString>(var.first)*/
  for (const auto& var: vars) {
    qDebug() << irs::str_conv<QString>(var.first) << var.second << '\n';
  }
}

void MainWindow::on_pushButton_8_clicked()
{
  docscreator::msword_t msword;
  string_type fileName = irst("C:\\Temp\\123.doc");
  string_type fileName2 = irst("C:\\Temp\\1234");
  docscreator::msdocument_t* doc = msword.document_create(fileName);
  doc->save(fileName2);

  const string_type var = irst("отсчет");
  map<int, vector<docscreator::msdocument_t::var_t> > row_value_map;

  //doc->get_map_row_vars(NULL, var, &row_value_map);
  doc->get_tables_vars_test();
  doc->save();
  msword.all_document_delete();
}

void MainWindow::on_autoConnectAnyDevicecheckBox_clicked(bool checked)
{
  ui->deviceGroupBox->setEnabled(!checked);
}

void MainWindow::on_syncTimePushButton_clicked()
{
  QDateTime dateTime = QDateTime::currentDateTime();
  const int offset_from_utc = dateTime.offsetFromUtc();

  time_t raw_time = time(NULL);

  // Установка не поддерживает часовые пояса, поэтому сдвигаем время вручную
  raw_time += offset_from_utc;

  m_calibrator.set_time(raw_time);
}
