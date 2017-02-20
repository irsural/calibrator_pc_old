//! \file
//! \brief Компонент документа MS Word.
//!
//! Дата создания: 19.11.2010

#ifndef msword_documentH
#define msword_documentH

#include <irsdefs.h>

#include <irstable.h>

#include <QAxObject>

//#include "composite_table.h"


#include <irsfinal.h>

namespace docscreator {

typedef irs::string_t text_type;

//! \brief Базовый класс для конверторов типов в строку и обратно
template <class T>
class type_converter_base_t
{
public:
  typedef irs::string_t string_type;
  typedef T value_type;
  typedef map<string_type, value_type> str_map_type;
  typedef typename map<string_type, value_type>::const_iterator
    str_map_const_iterator;
  typedef map<value_type, str_map_const_iterator> type_map_type;
  typedef typename map<value_type, str_map_const_iterator>::const_iterator
    type_map_const_iterator;
  void type_to_str(const value_type& a_value, string_type *ap_type_str) const
  {
    type_map_const_iterator type_map_const_it =
      m_type_map.find(a_value);
    if (type_map_const_it != m_type_map.end()) {
      *ap_type_str = type_map_const_it->second->first;
    } else {
      ap_type_str->clear();
      IRS_LIB_ASSERT("Неучтенное значение");
    }
  }
  bool str_to_type(const string_type& a_type_str, value_type* ap_value) const
  {
    str_map_const_iterator str_map_const_it = m_str_map.find(a_type_str);
    bool convert_success = false;
    if (str_map_const_it != m_str_map.end()) {
      convert_success = true;
      *ap_value = str_map_const_it->second;
    } else {
      // Возвращаем статус неудачи
    }
    return convert_success;
  }
protected:
  type_converter_base_t():
    m_str_map(),
    m_type_map()
  {
  }
  void insert(const value_type& a_value, const string_type& a_str)
  {
    pair<str_map_const_iterator, bool> result = m_str_map.insert(
      make_pair(a_str, a_value));
    m_type_map.insert(make_pair(a_value, result.first));
  }
private:
  str_map_type m_str_map;
  type_map_type m_type_map;
};

//! \brief Класс для хранения строк переменных.
//! \details Позволяет хранить позиции переменных в таблице MS Word.
class table_variables_t
{
public:
  typedef size_t size_type;
  typedef irs::string_t string_type;
  table_variables_t();
  //! \brief Добавляет переменную в список
  void add(size_type a_column, size_type a_row, const string_type& a_var);
  //! \brief Возвращает \c true, если переменных нет
  bool empty() const;
  size_type get_first_row_index() const;
  //! \brief Возвращает список пар имя переменной - номер столбца, принадлежащих
  //!   первой строке таблицы
  map<size_type, string_type> get_first_row_vars() const;
  //! \brief Удаляет переменные, принадлежащие первой строке таблицы
  void erase_first_row();
  //! \brief Повышает значения индекса всех строк на указанную величину
  void increase_all_rows(size_type a_value);
private:
  // key - строка; mapped_type - словарь: индекс столбца, имя переменной
  map<size_type, map<size_type, string_type> > m_row_vars_map;
};

class msword_t;
class msdocument_t
{
public:
  typedef size_t size_type;
  typedef irs::string_t string_type;
  typedef string_type rtf_text_type;
  enum { wd_collapse_end = 0 };
  enum { wd_collapse_start = 1 };
  enum { wd_auto_fit_fixed = 0 };
  enum { wd_auto_fit_content = 1 };
  enum { wd_auto_fir_window = 2 };
  enum { wd_align_row_left = 0 };
  enum { wd_align_row_center = 1 };
  enum { wd_align_row_right = 2 };
  enum { wd_line = 5 };
  enum { wd_character = 1 };
  enum { wd_extended = 1 };
  enum { wd_adjust_none = 0 };

  enum wd_header_footer_index_t{
    wd_header_footer_primary = 1,
    wd_header_footer_first_page = 2,
    wd_header_footer_even_pages = 3
  };

  enum wd_page_number_aligment_t {
    wd_align_page_number_left = 0,
    wd_align_page_number_center = 1,
    wd_align_page_number_right = 2,
    wd_align_page_number_inside = 3,
    wd_align_page_number_outside = 4
  };

  enum wd_orientation_t {
    wd_orient_landscape = 1,
    wd_orient_portrait = 0
  };

  enum wd_cell_align_vertical_t {
    wd_cell_align_vertical_top = 0,
    wd_cell_align_vertical_center = 1,
    wd_cell_align_vertical_bottom = 3
  };

  enum wd_align_paragraph_t {
    wd_align_paragraph_left = 0,
    wd_align_paragraph_center = 1,
    wd_align_paragraph_right = 2
  };

  enum wd_break_type_t {
    wd_page_break = 7
  };

  //! \brief Тип обтекания фигуры текстом для MS Word
  enum wd_wrap_type_t {
    wd_wrap_inline = 7,     //!< \brief В тексте
    wd_wrap_none = 3,       //!< \brief Нет
    wd_wrap_square = 0,     //!< \brief Вокруг рамки
    wd_wrap_through = 2,    //!< \brief Сквозное
    wd_wrap_tight = 1,      //!< \brief По контуру
    wd_wrap_top_bottom = 4, //!< \brief Свехру и снизу
    wd_wrap_behind = 5,     //!< \brief За текстом
    wd_wrap_front = 6       //!< \brief Перед текстом
  };
  //! \brief Вариант относительной горизонтальной позиции
  enum wd_relative_horizontal_position_t {
    //! \brief Относительно знака
    wd_relative_horizontal_position_character = 3,
    //! \brief Относительно колонки
    wd_relative_horizontal_position_column = 2,
    //! \brief Относительно внутреннего поля
    wd_relative_horizontal_position_inner_margin_area = 6,
    //! \brief Относительно левого поля
    wd_relative_horizontal_position_left_margin_area = 4,
    //! \brief Относительно поля
    wd_relative_horizontal_position_margin = 0,
    //! \brief Относительно внешнего поля
    wd_relative_horizontal_position_outer_margin_area = 7,
    //! \brief Относительно страницы
    wd_relative_horizontal_position_page = 1,
    //! \brief Относительно левого поля
    wd_relative_horizontal_position_right_margin_area = 5
  };
  //! \brief Вариант относительной вертикальной позиции
  enum wd_relative_vertical_position_t {
    //! \brief Относительно нижнего поля
    wd_relative_vertical_position_bottom_margin_area = 5,
    //! \brief Относительно внутреннего поля
    wd_relative_vertical_position_inner_margin_area = 6,
    //! \brief Относительно строки
    wd_relative_vertical_position_line = 3,
    //! \brief Относительно поля
    wd_relative_vertical_position_margin = 0,
    //! \brief Относительно внешнего поля
    wd_relative_vertical_position_outer_mrgin_area = 7,
    //! \brief Относительно страницы
    wd_relative_vertical_position_page = 1,
    //! \brief Относительно абзаца
    wd_relative_vertical_position_paragraph = 2,
    //! \brief Относительно верхнего поля
    wd_relative_vertical_position_top_margin_area = 4
  };
  //! \brief Тип обтекания фигуры текстом
  enum wd_wrap_side_type_t {
    wd_wrap_both = 0,       //!< \brief Вокруг
    wd_wrap_largest = 3,    //!< \brief По большей стороне
    wd_wrap_left = 1,       //!< \brief Слева
    wd_wrap_right = 2       //!< \brief Справа
  };
  //! \brief Поведение поиска
  enum wd_find_wrap {
    //! \bruef Окончание поиска при достижении начала/конца
    wd_find_stop = 0,
    //! \brief Продолжение поиска при достижении начала/конца
    wd_find_continue = 1,
    //! \brief Запрос поиска в оставшейся части у пользователя
    wd_find_ask = 2
  };

  //! \brief Способ замены
  enum wd_replace {
    wd_replace_none = 0,
    wd_replace_one = 1,
    wd_replace_all = 2
  };

  enum wd_movement_type_t {
    wd_move = 0,
    wd_extend = 1
  };

  enum {
    table_col_max_count = 32767,
    table_row_max_count = 63
  };

  msdocument_t(msword_t* ap_msword, QAxObject* ap_document,
    const string_type& a_file_name = irst(""));
  ~msdocument_t();
  void save();
  void save(const string_type& a_file_name);
  string_type text_get();

  void text_replace(const string_type& a_find_text,
    const string_type& a_replace_text);
  struct var_t {
    int column_index;
    string_type value;
  };

  void get_tables_vars_test();
  vector<table_variables_t> get_tables_vars(const vector<string_type>& a_vars);
  void get_table_vars(
    QAxObject* ap_table, const vector<string_type> &a_vars,
    table_variables_t* ap_table_variables);

  void get_map_row_vars(
    QAxObject* ap_table, const string_type &a_var_value,
    table_variables_t* ap_table_variables);
private:

  msdocument_t();
  void text_insert(QAxObject *ap_range, const string_type& a_text);

  QAxObject* mp_document;
  size_type m_document_id;
  string_type m_file_name;
  msword_t* mp_msword;

  string_type m_var_prefix;
  string_type m_var_suffix;
};

class find_text_t
{
public:
  typedef irs::string_t string_type;
  find_text_t(QAxObject* ap_document);
  bool find(const string_type& a_find_text);
  std::shared_ptr<QAxObject> get_range();
  void set_range(std::shared_ptr<QAxObject> ap_range);
private:
  QAxObject* mp_document;
  std::shared_ptr<QAxObject> mp_range;
  std::shared_ptr<QAxObject> mp_find;
};

/*class msdoc_action_t
{
public:
  typedef stringns_t string_type;
  virtual ~msdoc_action_t()
  {
  }
  virtual irs::handle_t<msdoc_action_t> clone() const = 0;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const = 0;
};

class msdoc_actions_t
{
public:
  typedef size_t size_type;
  typedef ptrdiffns_t difference_type;
  typedef irs::string_t string_type;
  typedef msdocument_t::rtf_text_type rtf_text_type;
  msdoc_actions_t();
  msdoc_actions_t(const msdoc_actions_t& a_msdoc_actions);
  void swap(msdoc_actions_t* ap_msdoc_actions);
  const msdoc_actions_t& operator=(const msdoc_actions_t& a_msdoc_actions);
  void exec_actions(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
  size_type action_count() const;
  const msdoc_action_t* operator[](size_type a_index) const;
  void clear_actions();
  void erase_last_actions(size_type a_count);
  void create();
  void open(const string_type& a_file_name);
  void close();
  void save();
  void save(const string_type& a_file_name);
  void text_add(const string_type& a_text, const font_t& a_font);
  void rtf_text_add(const rtf_text_type& a_text);
  void image_add(const string_type& a_file_name,
    const shape_size_and_position_t& a_shape_size_and_position =
    shape_size_and_position_t());
  void table_add(const irs::table_string_t& a_table_string);
  void table_add(const doc_table_type& a_doc_table,
    table_format_t& a_table_format);
  void file_add(const string_type& a_file_name);
  void break_add();
  void paragraph_format_set(const paragraph_format_t& a_format);
  void header_footer_set(const header_footer_t& a_header_footer);
  void page_numbers_set(const page_numbers_options_t& a_page_numbers_options);
  void page_setup_set(const page_setup_t& a_page_setup);
private:
  typedef deque<irs::handle_t<msdoc_action_t> > actions_type;
  typedef deque<irs::handle_t<msdoc_action_t> >::iterator actions_iterator;
  actions_type m_actions;
};

class msword_doc_actions_executor_t
{
public:
  typedef size_t size_type;
  msword_doc_actions_executor_t(const msdoc_actions_t* ap_actions = IRS_NULL,
    msword_t* ap_msword = IRS_NULL, msdocument_t* ap_msdoc  = IRS_NULL);
  void set_actions(const msdoc_actions_t* ap_actions);
  void set_msword(msword_t* ap_msword);
  void set_msdocument(msdocument_t* ap_msdoc);
  msdocument_t* get_msdocument();
  void exec_actions();
  bool is_complete() const;
  //! \brief Возвращает прогресс в виде значения от 0 до 1
  double get_progress() const;
  void tick();
private:
  enum process_t {
    p_wait,
    p_exec_action
  };
  const msdoc_actions_t* mp_actions;
  msword_t* mp_msword;
  msdocument_t* mp_msdoc;
  process_t m_process;
  size_type m_action_index;
};

class msdoc_action_create_t: public msdoc_action_t
{
public:
  msdoc_action_create_t();
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
};

class msdoc_action_open_t: public msdoc_action_t
{
public:
  msdoc_action_open_t(const string_type& a_file_name);
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
  const string_type& m_file_name;
};

class msdoc_action_close_t: public msdoc_action_t
{
public:
  msdoc_action_close_t();
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
};

class msdoc_action_save_t: public msdoc_action_t
{
public:
  msdoc_action_save_t();
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
};

class msdoc_action_save_as_t: public msdoc_action_t
{
public:
  msdoc_action_save_as_t(const string_type& a_file_name);
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
  const string_type m_file_name;
};

class msdoc_action_text_add_t: public msdoc_action_t
{
public:
  msdoc_action_text_add_t(const string_type& a_text, const font_t& a_font);
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
  const string_type m_text;
  const font_t m_font;
};

class msdoc_action_rtf_text_add_t: public msdoc_action_t
{
public:
  typedef stringns_t rtf_text_type;
  msdoc_action_rtf_text_add_t(const rtf_text_type& a_rtf_text);
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
  rtf_text_type m_rtf_text;
};

class msdoc_action_table_string_add_t: public msdoc_action_t
{
public:
  msdoc_action_table_string_add_t(const irs::table_string_t& a_table_string);
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
  irs::table_string_t m_table;
};

class msdoc_action_doc_table_add_t: public msdoc_action_t
{
public:
  msdoc_action_doc_table_add_t(const doc_table_type& a_table,
    const table_format_t& a_table_format);
    virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
  doc_table_type m_table;
  table_format_t m_table_format;
};

class msdoc_action_break_add_t: public msdoc_action_t
{
public:
  msdoc_action_break_add_t();
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
};

class msdoc_action_file_add_t: public msdoc_action_t
{
public:
  typedef stringns_t string_type;
  msdoc_action_file_add_t(const string_type& a_file_name);
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
  const string_type m_file_name;
};

class msdoc_action_image_add_t: public msdoc_action_t
{
public:
  typedef stringns_t string_type;
  msdoc_action_image_add_t(const string_type& a_file_name,
    const shape_size_and_position_t& a_shape_size_and_position =
    shape_size_and_position_t());
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
  const string_type m_file_name;
  const shape_size_and_position_t m_shape_size_and_position;
};

class msdoc_action_paragraph_format_set_t: public msdoc_action_t
{
public:
  typedef stringns_t string_type;
  msdoc_action_paragraph_format_set_t(const paragraph_format_t& a_format);
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
  const paragraph_format_t m_format;
};

class msdoc_action_header_footer_set_t: public msdoc_action_t
{
public:
  typedef stringns_t string_type;
  msdoc_action_header_footer_set_t(const header_footer_t& a_header_footer);
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
  const header_footer_t m_header_footer;
};

class msdoc_action_page_numbers_set_t: public msdoc_action_t
{
public:
  typedef stringns_t string_type;
  msdoc_action_page_numbers_set_t(
    const page_numbers_options_t& a_page_numbers_options);
    virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
  const page_numbers_options_t m_page_numbers_options;
};

class msdoc_action_page_setup_set_t: public msdoc_action_t
{
public:
  typedef stringns_t string_type;
  msdoc_action_page_setup_set_t(const page_setup_t& a_page_setup);
  virtual irs::handle_t<msdoc_action_t> clone() const;
  virtual void exec(msword_t* ap_msword, msdocument_t** ap_msdoc) const;
private:
  const page_setup_t m_page_setup;
};*/

/*class queue_msdoc_actions_t
{
public:
  typedef stringns_t string_type;
  typedef string_type rtf_text_type;
  typedef sizens_t size_type;
  void add(irs::handle_t<msdoc_action_t> ap_msdoc_action);
  size_type size();
  void pop_back(size_type a_count);
  void text_add(const string_type& a_text, const font_t& a_font);
  void rtf_text_add(const rtf_text_type& a_text);
  void image_add(const string_type& a_file_name,
    const shape_size_and_position_t& a_shape_size_and_position =
    shape_size_and_position_t());
  void table_add(const doc_table_type& a_doc_table,
    table_format_t& a_table_format);
  void file_add(const string_type& a_file_name);
  void break_add();
  bool paragraph_format_set(const paragraph_format_t& a_format);
  void header_footer_set(const header_footer_t& a_header_footer);
  void page_numbers_set(const page_numbers_options_t& a_page_numbers_options);
  void page_setup_set(const page_setup_t& a_page_setup);
private:
  deque<irs::handle_t<msdoc_action_t> > m_actions;
};
*/
} // namespace docscreator

#endif // msword_documentH
