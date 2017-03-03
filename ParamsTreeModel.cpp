#include "ParamsTreeModel.h"

// class Parameter
Parameter::Parameter(const QString& aName, int aPrecision):
  name(aName),
  precision(aPrecision)
{
}

void Parameter::setMinMax(double aMinValue, double aMaxValue)
{
  minValue = aMinValue;
  maxValue = aMaxValue;
  if (minValue != maxValue) {
    parameter = irs::range(parameter, minValue, maxValue);
  }
}

bool Parameter::isSet() const
{
  return set;
}

Parameter& Parameter::operator=(double value)
{
  setValue(value);
  return *this;
}

Parameter::operator double() const
{
  return value();
}

double Parameter::value() const
{
  return parameter;
}

void Parameter::setValue(double value)
{
  parameter = value;
  set = true;
  if (minValue != maxValue) {
    parameter = irs::range(parameter, minValue, maxValue);
  }
}

bool Parameter::setValueFromString(const QString& aStr)
{
  if (aStr.isEmpty()) {
    set = false;
    return true;
    parameter = 0;
    if (minValue != maxValue) {
      parameter = irs::range(parameter, minValue, maxValue);
    }
    return true;
  }

  bool ok = false;
  double num = QLocale().toDouble(aStr, &ok);
  if (!ok) {
    return false;
  }

  parameter = num;
  set = true;

  if (minValue != maxValue) {
    parameter = irs::range(num, minValue, maxValue);
  }
  return true;
}

QString Parameter::string(const int column) const
{
  if (column == 0) {
    return name;
  } if (column == 1) {
    if (isSet()) {
      return QString("%L1").arg(parameter, 0, 'g', precision);
    } else {
      return QString();
    }
  }
  return QString();
}

QString Parameter::valueToString() const
{
  return string(valueColumn);
}

Group* Parameter::parent()
{
  return parentPtr;
}

void Parameter::setHidden(bool hide)
{
  if (parentPtr) {
    parentPtr->setHidden(this, hide);
  }
}

ParamsTreeModel::ParamsTreeModel(QObject *parent)
  : QAbstractItemModel(parent)
{
  /*points.push_back(point_t(10, 10.1));
  points.push_back(point_t(20, 20.2));
  points.push_back(point_t(30, 30.1));
  points.push_back(point_t(40, 40.4));
  points.push_back(point_t(50, 50.3));

  auto it = points.begin();
  while (it != points.end()) {
    displayedPoints.push_back(it);
    ++it;
  }*/
  commonParameters.reset(new Group(toQString(L"Общие параметры")));
  groups.add(commonParameters.get());

  diapasonParameters.reset(new Group(toQString(L"Параметры диапазона")));
  groups.add(diapasonParameters.get());

  pointParameters.reset(new Group(toQString(L"Параметры точки")));
  groups.add(pointParameters.get());

  responseTime.reset(new ParameterIcon(toQString(L"Время успокоения, с")));
  responseTime->setMinMax(0, 100);
  //responseTime->setIconForParameterIsNotSet(
    //":/Images/Images/warning.png");

  commonParameters->add(responseTime.get());

  range.reset(new ParameterIcon(toQString(L"Диапазон, В")));
  range->setMinMax(0, 1000);
  diapasonParameters->add(range.get());

  maxMark.reset(new ParameterIcon(toQString(L"Максимальная метка")));
  maxMark->setMinMax(-10000, 10000);
  diapasonParameters->add(maxMark.get());

  maxMarkValue.reset(new ParameterIcon(toQString(L"Значение максимальной метки")));
  maxMarkValue->setMinMax(-1000, 1000);
  diapasonParameters->add(maxMarkValue.get());

  normalizingValue.reset(new ParameterIcon(toQString(L"Нормирующее значение")));
  normalizingValue->setMinMax(1e-9, 5000);
  diapasonParameters->add(normalizingValue.get());

  maxErrorOfRange.reset(new ParameterIcon(toQString(L"Допустимая погрешность, %")));
  maxErrorOfRange->setMinMax(1e-7, 1000);
  diapasonParameters->add(maxErrorOfRange.get());

  maxErrorOfReading.reset(
    new ParameterIcon(toQString(L"Допуст. погр. от показания, %")));
  maxErrorOfReading->setMinMax(0, 1000);
  diapasonParameters->add(maxErrorOfReading.get());

  mark.reset(
    new ParameterIcon(toQString(L"Текущая метка")));
  mark->setMinMax(-10000, 10000);
  pointParameters->add(mark.get());

  markValue.reset(
    new ParameterIcon(toQString(L"Значение метки, В")));
  markValue->setMinMax(-1000, 1000);
  pointParameters->add(markValue.get());

  absoluteError.reset(
    new Parameter(toQString(L"Абсолютная погрешность, В")));
  pointParameters->add(absoluteError.get());

  relativeError.reset(
    new Parameter(toQString(L"Относительная погрешность, %")));
  pointParameters->add(relativeError.get());

  reducedError.reset(
    new Parameter(toQString(L"Приведенная погрешность, %")));
  pointParameters->add(reducedError.get());

  setDeviceType(deviceType, true);
}

ParamsTreeModel::~ParamsTreeModel()
{
}

QModelIndex ParamsTreeModel::index(int row, int column, const QModelIndex &parent)
  const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid()) {
    Group* group = groups[row];
    return createIndex(row, column, group);
  }

  Group* group = static_cast<Group*>(parent.internalPointer());
  Parameter* parameter = (*group)[row];
  return createIndex(row, column, parameter);
}

QModelIndex ParamsTreeModel::parent(const QModelIndex& index) const
{
  if (!index.isValid()) {
    return QModelIndex();
  }
  Node *node = static_cast<Node*>(index.internalPointer());
  Group *parent = node->parent();
  if (parent == nullptr) {
    return QModelIndex();
  }
  /*auto it = find(groups.begin(), groups.end(), parent);
  if (it == groups.end()) {
    return QModelIndex();
  }
  int row = distance(groups.begin(), it);*/
  int row = groups.index(parent);
  return createIndex(row, 0, parent);
}

int ParamsTreeModel::rowCount(const QModelIndex &parent) const
{
  if (!parent.isValid()) {
    return groups.size();
  }
  Node* node = static_cast<Node*>(parent.internalPointer());
  return node->size();
}

int ParamsTreeModel::columnCount(const QModelIndex &parent) const
{
  /*if (!parent.isValid()) {
    return 1;
  }*/
  return 2;
}

void ParamsTreeModel::setDeviceType(device_type_t aDeviceType, bool force)
{
  if ((deviceType == aDeviceType) && !force) {
    return;
  }

  beginResetModel();
  deviceType = aDeviceType;
  if (deviceType == device_type_analog) {
    groups.setHidden(commonParameters.get(), false);
    maxMark->setHidden(false);
    maxMarkValue->setHidden(false);
    normalizingValue->setHidden(false);
    maxErrorOfReading->setHidden(true);
    markValue->setHidden(false);
    reducedError->setHidden(false);
  } else {
    groups.setHidden(commonParameters.get(), true);
    maxMark->setHidden(true);
    maxMarkValue->setHidden(true);
    normalizingValue->setHidden(true);
    maxErrorOfReading->setHidden(false);
    markValue->setHidden(true);
    reducedError->setHidden(true);
  }
  endResetModel();
}

bool ParamsTreeModel::setData(const QModelIndex & index, const QVariant & value,
  int role)
{
  if (role != Qt::EditRole) {
    return false;
  }

  if (!index.isValid()) {
    return false;
  }

  Node* node = static_cast<Node*>(index.internalPointer());
  Parameter * parameter = dynamic_cast<Parameter *>(node);
  if (!parameter) {
    return false;
  }

  //const bool prevValueSet = parameter->isSet();
  const QString prevString = parameter->valueToString();

  if (!parameter->setValueFromString(value.toString())) {
    return false;
  }

  if (parameter == range.get()) {
    if (maxMark->valueToString() == prevString) {
      maxMark->setValueFromString(parameter->valueToString());
    }
    if (maxMarkValue->valueToString() == prevString) {
      maxMarkValue->setValueFromString(parameter->valueToString());
    }
    if (normalizingValue->valueToString() == prevString) {
      normalizingValue->setValueFromString(parameter->valueToString());
    }
  } else if (parameter == maxMark.get()) {
    if (maxMarkValue->valueToString() == prevString) {
      maxMarkValue->setValueFromString(parameter->valueToString());
    }
    if (normalizingValue->valueToString() == prevString) {
      normalizingValue->setValueFromString(parameter->valueToString());
    }
  } else if (parameter == maxMarkValue.get()) {
    if (normalizingValue->valueToString() == prevString) {
      normalizingValue->setValueFromString(parameter->valueToString());
    }
  }

  return true;
}

QVariant ParamsTreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) {
     return QVariant();
  }

  Node *node = static_cast<Node*>(index.internalPointer());

  if ( role == Qt::DecorationRole ) {
    if (index.column() == nameColumn) {
      //return getIcon();
      //QPixmap pixmap(":/Images/Images/greenStone.png");
      //return QIcon(pixmap.scaled(QSize(16, 16)));
      return node->icon();
    }
  }

  if ((role != Qt::DisplayRole) && (role != Qt::EditRole)) {
    return QVariant();
  }

  return node->string(index.column());
}

Qt::ItemFlags ParamsTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) {
     return 0;
  }

  if (index.column() == valueColumn) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ParamsTreeModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
  if (role != Qt::DisplayRole) {
    return QVariant();
  }
  if (orientation != Qt::Horizontal) {
    return QVariant();
  }
  if (section == 0)
    return QVariant(toQString(L"Параметр"));
  else
    return QVariant(toQString(L"Значение"));
}

/*void TreeModel::setFilter(const QString& afilter)
{
  filter = afilter.toStdWString();
  updateDisplayedPeople();
}*/

QIcon ParamsTreeModel::getIcon() const
{
  return QIcon();
}

void ParamsTreeModel::updateDisplayedPeople()
{
  /*beginResetModel();
  displayedPeople.clear();
  if (filter.empty()) {
    auto it = people.begin();
    while (it != people.end()) {
      displayedPeople.push_back(it);
      ++it;
    }
  } else {
    auto it = people.begin();
    while (it != people.end()) {
      const std::wstring& name = it->name;
      if (filter.size() <= name.size()) {
        if (filter == name.substr(0, filter.size())) {
          displayedPeople.push_back(it);
        }
      }
      ++it;
    }
  }
  endResetModel();*/
}


ParamsTreeModel2::ParamsTreeModel2(QObject *parent)
  : QAbstractItemModel(parent),
  points()
{
  points.push_back(point_t(10, 10.1));
  points.push_back(point_t(20, 20.2));
  points.push_back(point_t(30, 30.1));
  points.push_back(point_t(40, 40.4));
  points.push_back(point_t(50, 50.3));

  auto it = points.begin();
  while (it != points.end()) {
    displayedPoints.push_back(it);
    ++it;
  }
}

ParamsTreeModel2::~ParamsTreeModel2()
{
}

QModelIndex ParamsTreeModel2::index(int row, int column, const QModelIndex &parent)
  const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid()) {
    const pointsItType* it = &displayedPoints[row];

    return createIndex(row, column, const_cast<pointsItType*>(it));
  }

  return QModelIndex();
}

QModelIndex ParamsTreeModel2::parent(const QModelIndex & /*index*/) const
{
  return QModelIndex();
}

int ParamsTreeModel2::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }

  return points.size();
}

int ParamsTreeModel2::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return 2;
}

bool ParamsTreeModel2::setData(const QModelIndex & index, const QVariant & value,
  int role)
{
  if (role != Qt::EditRole) {
    return false;
  }
  const pointsItType* it = static_cast<pointsItType*>(index.internalPointer());
  QString s = value.toString();
  bool ok = false;
  double num = s.toDouble(&ok);
  if (!ok) {
    return false;
  }
  (*it)->value = num;
  return true;
}

QVariant ParamsTreeModel2::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
     return QVariant();

  if (role != Qt::DisplayRole)
      return QVariant();

  const pointsItType* it = static_cast<pointsItType*>(index.internalPointer());

  switch (index.column()) {
    case 0: {
      return QString("%L1").arg((*it)->mark, 0, 'g', 7);
    };
    case 1: {
      return QString("%L1").arg((*it)->value, 0, 'g', 7);
    }
    default: {
      break;
    }
  }
  return QVariant();
}

Qt::ItemFlags ParamsTreeModel2::flags(const QModelIndex &index) const
{
  if (!index.isValid()) {
     return 0;
  }

  if (index.column() == 1) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
  }

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ParamsTreeModel2::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
  if (role != Qt::DisplayRole) {
    return QVariant();
  }
  if (orientation != Qt::Horizontal) {
    return QVariant();
  }
  if (section == 0)
    return QVariant(toQString(L"Параметр"));
  else
    return QVariant(toQString(L"Значение"));
}

/*void TreeModel::setFilter(const QString& afilter)
{
  filter = afilter.toStdWString();
  updateDisplayedPeople();
}*/

void ParamsTreeModel2::updateDisplayedPeople()
{
  /*beginResetModel();
  displayedPeople.clear();
  if (filter.empty()) {
    auto it = people.begin();
    while (it != people.end()) {
      displayedPeople.push_back(it);
      ++it;
    }
  } else {
    auto it = people.begin();
    while (it != people.end()) {
      const std::wstring& name = it->name;
      if (filter.size() <= name.size()) {
        if (filter == name.substr(0, filter.size())) {
          displayedPeople.push_back(it);
        }
      }
      ++it;
    }
  }
  endResetModel();*/
}

