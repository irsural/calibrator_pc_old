#ifndef PARAMSTREEMODEL_H
#define PARAMSTREEMODEL_H

#include <QAbstractItemModel>
#include <QLocale>
#include <QFileIconProvider>

#include <irsstrconv.h>

#include "calibrator.h"

#include <irsfinal.h>

inline QString toQString(const std::wstring& a_str)
{
  return irs::str_conv<QString>(a_str);
}

struct point_t
{
  double mark;
  double value;
  point_t(double a_mark = 0, double a_value = 0):
    mark(a_mark),
    value(a_value)
  {
  }
};
/*
struct point_t
{
  typedef irs::string_t string_type;
  string_type name;
  double response_time;
  double diapason;
  //! \brief Нормирующее значение
  double normalizing_value;
  //! \brief Погрешность от диапазона в относительных единицах
  double range_error_max;
  //! \brief Погрешность от показания в относительных единицах
  double reading_error_max;
  //! \brief Метка
  double mark;
  //! \brief Значение метки
  double mark_value;
};
*/

class Group;

class Node
{
public:
  virtual QString string(const int column) const = 0;
  virtual Group* parent()
  {
    return nullptr;
  }
  virtual std::size_t size() const
  {
    return 0;
  }
  virtual QIcon icon() const
  {
    return QIcon();
  }
};

class Parameter: public Node
{
public:
  Parameter(const QString& aName, int aPrecision = out_precision);
  void setMinMax(double aMinValue, double aMaxValue);
  bool isSet() const;
  Parameter& operator=(double value);
  operator double() const;
  double value() const;
  void setValue(double value);
  bool setValueFromString(const QString& aStr);
  QString string(const int column) const override;
  QString valueToString() const;
  virtual Group* parent() override;
  void setHidden(bool hide);
private:
  QString name;
  double parameter = 0;
  bool set = false;
  int valueColumn = 1;
  int precision = out_precision;
  double minValue = 0;
  double maxValue = 0;
  Group *parentPtr;
  friend class Group;
};

class ParameterIcon: public Parameter
{
public:
  ParameterIcon(const QString& aName, int aPrecision = out_precision):
    Parameter(aName, aPrecision)
  {
    setIconForParameterIsNotSet(":/Images/Images/warning.png");
  }

  QIcon icon() const override
  {
    if (isSet()) {
      return QIcon();
    } else {
      return iconForParameterIsNotSet;
    }
  }
  void setIconForParameterIsNotSet(const QString& fileNameIcon)
  {
    QPixmap pixmap(fileNameIcon);
    iconForParameterIsNotSet = QIcon(pixmap.scaled(QSize(16, 16)));
  }
private:
  QIcon iconForParameterIsNotSet;
};

class Group: public Node
{
public:
  Group(const QString& aName):
    name(aName)
  {
  }

  QString string(const int column) const override
  {
    if (column == 0) {
      return name;
    }
    return QString();
  }

  void add(Parameter *parameter)
  {
    parameter->parentPtr = this;
    ParameterWrapper p;
    p.parameter = parameter;
    p.hidden = false;
    children.push_back(p);
    visibleChildren.push_back(parameter);
  }

  Parameter* operator[](std::size_t i)
  {
    return visibleChildren[i];
  }

  std::size_t size() const override
  {
    return visibleChildren.size();
  }

  int index(Parameter *parameter) const
  {
    auto it = find(visibleChildren.begin(), visibleChildren.end(), parameter);
    if (it == visibleChildren.end()) {
      throw std::logic_error("Parameter not found");
    }
    return distance(visibleChildren.begin(), it);
  }

  void setHidden(Parameter *parameter, bool hide)
  {
    visibleChildren.clear();
    for (auto child : children) {
      if (child.parameter == parameter) {
        child.hidden = hide;
      }
      if (!child.hidden) {
        visibleChildren.push_back(child.parameter);
      }
    }
  }
private:
  QString name;
  struct ParameterWrapper
  {
    Parameter *parameter = nullptr;
    bool hidden = false;
  };

  vector<ParameterWrapper> children;
  std::vector<Parameter*> visibleChildren;
};

class Groups
{
public:
  void add(Group* group)
  {
    GroupWrapper g;
    g.group = group;
    g.hidden = false;
    groups.push_back(g);
    visibleGroups.push_back(group);
  }

  int size() const
  {
    return visibleGroups.size();
  }

  int index(Group *group) const
  {
    auto it = find(visibleGroups.begin(), visibleGroups.end(), group);
    if (it == visibleGroups.end()) {
      throw std::logic_error("Group not found");
    }
    return distance(visibleGroups.begin(), it);
  }

  Group* operator[](int index) const
  {
    return visibleGroups[index];
  }

  void setHidden(Group *group, bool hide)
  {
    visibleGroups.clear();
    for (auto g : groups) {
      if (g.group == group) {
        g.hidden = hide;
      }
      if (!g.hidden) {
        visibleGroups.push_back(g.group);
      }
    }
  }

private:
  struct GroupWrapper
  {
    Group* group = nullptr;
    bool hidden = false;
  };

  vector<GroupWrapper> groups;
  vector<Group*> visibleGroups;
};

class ParamsTreeModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  ParamsTreeModel(QObject *parent = 0);
  ~ParamsTreeModel();
  bool setData(const QModelIndex & index, const QVariant & value,
    int role = Qt::EditRole);
  QVariant data(const QModelIndex &index, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation,
    int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column,
    const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  void setDeviceType(device_type_t aDeviceType, bool force = false);

private:
  QIcon getIcon() const;
  void updateDisplayedPeople();

  /*std::vector<point_t> points;
  typedef std::vector<point_t>::iterator pointsItType;
  std::vector<pointsItType> displayedPoints;
*/
  const int nameColumn = 0;
  const int valueColumn = 1;
  std::wstring filter;
  device_type_t deviceType = device_type_analog;
  Groups groups;
  std::shared_ptr<Group> commonParameters;
  std::shared_ptr<Group> diapasonParameters;
  std::shared_ptr<Group> pointParameters;

  std::shared_ptr<ParameterIcon> responseTime;
  std::shared_ptr<ParameterIcon> range;
  std::shared_ptr<ParameterIcon> maxMark;
  std::shared_ptr<ParameterIcon> maxMarkValue;
  std::shared_ptr<ParameterIcon> normalizingValue;
  std::shared_ptr<ParameterIcon> maxErrorOfRange;
  std::shared_ptr<ParameterIcon> maxErrorOfReading;
  std::shared_ptr<ParameterIcon> mark;
  std::shared_ptr<ParameterIcon> markValue;
  std::shared_ptr<Parameter> absoluteError;
  std::shared_ptr<Parameter> relativeError;
  std::shared_ptr<Parameter> reducedError;

  //QSpinBox spinbox;
};

class ParamsTreeModel2 : public QAbstractItemModel
{
  Q_OBJECT
public:
  ParamsTreeModel2(QObject *parent = 0);
  ~ParamsTreeModel2();
  bool setData(const QModelIndex & index, const QVariant & value,
    int role = Qt::EditRole);
  QVariant data(const QModelIndex &index, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation,
    int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column,
    const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
private:
  void updateDisplayedPeople();

  std::vector<point_t> points;
  typedef std::vector<point_t>::iterator pointsItType;
  std::vector<pointsItType> displayedPoints;

  std::wstring filter;
  //QSpinBox spinbox;
};

#endif // PARAMSTREEMODEL_H
