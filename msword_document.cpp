//! \file
//! \brief Компонент документа MS Word.
//!
//! Дата создания: 19.11.2010

#include "msword.h"
//#include "msword_table.h"
#include "msword_document.h"
#include <QDebug>

#include <irsfinal.h>

// class table_variables_t
docscreator::table_variables_t::table_variables_t():
  m_row_vars_map()
{
}

void docscreator::table_variables_t::add(
  size_type a_column, size_type a_row, const string_type& a_var)
{
  map<size_type, string_type>& column_var_map = m_row_vars_map[a_row];
  column_var_map[a_column] = a_var;
}

bool docscreator::table_variables_t::empty() const
{
  return m_row_vars_map.empty();
}

docscreator::table_variables_t::size_type
docscreator::table_variables_t::get_first_row_index() const
{
  if (m_row_vars_map.empty()) {
    throw std::out_of_range("table_variables_t is empty");
  }
  return m_row_vars_map.begin()->first;
}

map<docscreator::table_variables_t::size_type, docscreator::table_variables_t::string_type>
docscreator::table_variables_t::get_first_row_vars() const
{
  return m_row_vars_map.begin()->second;
}

void docscreator::table_variables_t::erase_first_row()
{
  m_row_vars_map.erase(m_row_vars_map.begin());
}

void docscreator::table_variables_t::increase_all_rows(size_type a_value)
{
  vector<pair<size_type, map<size_type, string_type> > > vec;

  auto it = m_row_vars_map.rbegin();
  while (it != m_row_vars_map.rend()) {

    size_type key = it->first + a_value;
    auto value = it->second;

    vec.push_back(make_pair(key, value));

    ++it;
  }

  m_row_vars_map.clear();

  for (auto e: vec) {
    m_row_vars_map.insert(e);
  }
}


// class msdocument_t
docscreator::msdocument_t::msdocument_t(msword_t* ap_msword,
  QAxObject* ap_document,
  const string_type& a_file_name):
  mp_document(ap_document),
  m_document_id(),
  m_file_name(a_file_name),
  mp_msword(ap_msword),
  m_var_prefix(irst("%(")),
  m_var_suffix(irst(")"))
{

}

docscreator::msdocument_t::~msdocument_t()
{
}

void docscreator::msdocument_t::save()
{
  const QString fileName = irs::str_conv<QString>(m_file_name);
  mp_document->dynamicCall("SaveAs(const QVariant&)", fileName);
}

void docscreator::msdocument_t::save(const string_type& a_file_name)
{
  m_file_name = a_file_name;
  save();
}

docscreator::msdocument_t::string_type docscreator::msdocument_t::text_get()
{
  std::shared_ptr<QAxObject> range(mp_document->querySubObject("Range()"));
  QString text = range->property("Text").toString();
  return irs::str_conv<string_type>(text);
}

void docscreator::msdocument_t::text_replace(const string_type& a_find_text,
  const string_type& a_replace_text)
{
  find_text_t find_text(mp_document);
  while (find_text.find(a_find_text)) {
    std::shared_ptr<QAxObject> range = find_text.get_range();
    text_insert(range.get(), a_replace_text);
    find_text.set_range(range);
  }
}

void docscreator::msdocument_t::text_insert(
  QAxObject* ap_range, const string_type& a_text)
{
  ap_range->dynamicCall("Collapse(QVariant&)",
    static_cast<int>(wd_collapse_end));
  ap_range->dynamicCall("InsertAfter(QVariant&)",
    irs::str_conv<QString>(a_text));
}

void docscreator::msdocument_t::get_tables_vars_test()
{
  vector<string_type> vars;
  vars.push_back(irst("диапазон"));
  vars.push_back(irst("отсчет"));
  vars.push_back(irst("действительное_значение"));
  vector<table_variables_t> tables_vars = get_tables_vars(vars);

  wofstream ofile("C:\\Temp\\table.log");

  int i = 0;
  for (auto& table_vars: tables_vars) {
    ofile << "Table #" << i << '\n';

    while (!table_vars.empty()) {
      ofile << "Row #" << table_vars.get_first_row_index() << '\n';
      map<size_type, string_type> vars = table_vars.get_first_row_vars();
      for (const auto& p: vars) {
        ofile << "col #" << p.first << " " << p.second << " ";
      }
      ofile << '\n';
      table_vars.erase_first_row();
    }
    i++;
  }
  ofile.close();
}

vector<docscreator::table_variables_t>
docscreator::msdocument_t::get_tables_vars(
  const vector<string_type>& a_vars)
{
  std::shared_ptr<QAxObject> tables(mp_document->querySubObject("Tables()"));
  const int count = tables->property("Count").toInt();
  vector<docscreator::table_variables_t> tables_vars;
  tables_vars.resize(count);
  for (int i = 0; i < count; i++) {
    std::shared_ptr<QAxObject> table(
      mp_document->querySubObject("Tables(QVariant)", i + 1));
    get_table_vars(table.get(), a_vars, &tables_vars[i]);
  }
  return tables_vars;
}

void docscreator::msdocument_t::get_table_vars(
  QAxObject* ap_table,
  const vector<string_type> &a_vars,
  table_variables_t* ap_table_variables)
{
  for (auto& var_name: a_vars) {
    get_map_row_vars(ap_table, var_name, ap_table_variables);
  }
}

void docscreator::msdocument_t::get_map_row_vars(QAxObject* ap_table,
  const string_type& a_var_value,
  table_variables_t *ap_table_variables)
{
  //QAxObject* table = mp_document->querySubObject("Tables(2)");

  //std::shared_ptr<QAxObject> range(ap_table->querySubObject("Range()"));

  //std::shared_ptr<QAxObject> find(range->querySubObject("Find()"));

  const bool match_case = true;
  const bool match_whole_word = false;
  const bool match_wildcards = false;
  const bool match_sounds_like = false;
  const bool match_all_word_forms = false;
  const bool forward = true;
  const int wrap = wd_find_stop;
  const bool format = false;
  const QString replace_with = "";
  const int replace = wd_replace_one;

  QString find_text =
    irs::str_conv<QString>(m_var_prefix + a_var_value + m_var_suffix);

  QList<QVariant> list;
  list.append(find_text);
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



  bool find_success = true;
  while (find_success) {

    std::shared_ptr<QAxObject> range(ap_table->querySubObject("Range()"));

    std::shared_ptr<QAxObject> find(range->querySubObject("Find()"));


    QVariant res = find->dynamicCall("Execute(QVariant, QVariant, "
      "QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, "
      "QVariant, QVariant, QVariant)", list);

    find_success = res.toBool();
    if (find_success) {
      QAxObject* cell = range->querySubObject("Cells(1)");
      int row_index = cell->property("RowIndex").toInt();
      int column_index = cell->property("ColumnIndex").toInt();


      ap_table_variables->add(column_index, row_index, a_var_value);

      //qDebug() << "row: " << row_index << " col: " << column_index << "\n";
    }
  }
}

// class find_text_t
docscreator::find_text_t::find_text_t(QAxObject* ap_document):
  mp_document(ap_document),
  mp_range(),
  mp_find()
{
  std::shared_ptr<QAxObject> range(mp_document->querySubObject("Range()"));
  range->dynamicCall("Collapse(QVariant&)",
    static_cast<int>(msdocument_t::wd_collapse_start));
  /*Variant range = mp_document->OleFunction("Range");
  range.OleProcedure("Collapse",
    static_cast<int>(msdocument_t::wd_collapse_start));*/
  set_range(range);
}

bool docscreator::find_text_t::find(const string_type& a_find_text)
{
  const bool match_case = true;
  const bool match_whole_word = false;
  const bool match_wildcards = false;
  const bool match_sounds_like = false;
  const bool match_all_word_forms = false;
  const bool forward = true;
  const int wrap = msdocument_t::wd_find_stop;
  const bool format = false;
  const QString replace_with;
  const int replace = msdocument_t::wd_replace_one;

  QList<QVariant> list;
  list.append(irs::str_conv<QString>(a_find_text));
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


  QVariant res = mp_find->dynamicCall("Execute(QVariant, QVariant, "
    "QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, "
    "QVariant, QVariant, QVariant)", list);


  /*Variant res =
    m_find.OleFunction("Execute", irs::str_conv<QString>(a_find_text),
    match_case, match_whole_word, match_wildcards, match_sounds_like,
    match_all_word_forms, forward, wrap, format, replace_with, replace);*/
  return res.toBool();
}

std::shared_ptr<QAxObject> docscreator::find_text_t::get_range()
{
  return mp_range;
}

void docscreator::find_text_t::set_range(std::shared_ptr<QAxObject> ap_range)
{
  mp_range = ap_range;
  mp_range->dynamicCall("Collapse(QVariant&)",
    static_cast<int>(msdocument_t::wd_collapse_end));

  /*mp_range.OleProcedure("Collapse",
    static_cast<int>(msdocument_t::wd_collapse_end));*/

  mp_find.reset(mp_range->querySubObject("Find()"));

  //connect(mp_find.get(), SIGNAL(exception(int, const QString&, const QString&, const QString&)), this, SLOT(catchException(int, const QString&, const QString&, const QString&)));



  mp_find->dynamicCall("ClearFormatting()");

  /*m_find = m_range.OlePropertyGet("Find");
  m_find.OleProcedure("ClearFormatting");*/
}
/*
// class msdoc_actions_t
docscreator::msdoc_actions_t::msdoc_actions_t():
  m_actions()
{
}

docscreator::msdoc_actions_t::msdoc_actions_t(
  const msdoc_actions_t& a_msdoc_actions):
  m_actions()
{
  for (size_type action_i = 0; action_i < m_actions.size(); action_i++) {
    m_actions.push_back(a_msdoc_actions.m_actions[action_i]->clone());
  }
}

void docscreator::msdoc_actions_t::swap(msdoc_actions_t* ap_msdoc_actions)
{
  std::swap(m_actions, ap_msdoc_actions->m_actions);
}

const docscreator::msdoc_actions_t&
docscreator::msdoc_actions_t::operator=(const msdoc_actions_t& a_msdoc_actions)
{
  msdoc_actions_t msdoc_actions(a_msdoc_actions);
  swap(&msdoc_actions);
  return *this;
}

void docscreator::msdoc_actions_t::exec_actions(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  DC_ASSERT(ap_msword);
  for (size_type action_i = 0; action_i < m_actions.size(); action_i++) {
    m_actions[action_i]->exec(ap_msword, ap_msdoc);
  }
}

docscreator::msdoc_actions_t::size_type
docscreator::msdoc_actions_t::action_count() const
{
  return m_actions.size();
}

const docscreator::msdoc_action_t*
docscreator::msdoc_actions_t::operator[](size_type a_index) const
{
  return m_actions[a_index].get();
}

void docscreator::msdoc_actions_t::clear_actions()
{
  m_actions.clear();
}

void docscreator::msdoc_actions_t::erase_last_actions(size_type a_count)
{
  actions_iterator it = m_actions.end();
  advance(it, -static_cast<difference_type>(a_count));
  m_actions.erase(it, m_actions.end());
}

void docscreator::msdoc_actions_t::create()
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_create_t()));
}

void docscreator::msdoc_actions_t::open(const string_type& a_file_name)
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_open_t(a_file_name)));
}

void docscreator::msdoc_actions_t::close()
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_close_t()));
}

void docscreator::msdoc_actions_t::save()
{
  //msdoc_action_t* g = new msdoc_action_save_t();
  //m_actions.push_back(g);
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_save_t()));
}

void docscreator::msdoc_actions_t::save(const string_type& a_file_name)
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_save_as_t(a_file_name)));
}

void docscreator::msdoc_actions_t::text_add(const string_type& a_text,
  const font_t& a_font)
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_text_add_t(a_text, a_font)));
}

void docscreator::msdoc_actions_t::rtf_text_add(const rtf_text_type& a_text)
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_rtf_text_add_t(a_text)));
}

void docscreator::msdoc_actions_t::image_add(const string_type& a_file_name,
  const shape_size_and_position_t& a_shape_size_and_position)
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_image_add_t(a_file_name, a_shape_size_and_position)));
}

void docscreator::msdoc_actions_t::table_add(
  const irs::table_string_t& a_table_string)
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_table_string_add_t(a_table_string)));
}

void docscreator::msdoc_actions_t::table_add(const doc_table_type& a_doc_table,
  table_format_t& a_table_format)
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_doc_table_add_t(a_doc_table, a_table_format)));
}

void docscreator::msdoc_actions_t::file_add(const string_type& a_file_name)
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_file_add_t(a_file_name)));
}

void docscreator::msdoc_actions_t::break_add()
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_break_add_t()));
}

void docscreator::msdoc_actions_t::paragraph_format_set(
  const paragraph_format_t& a_format)
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_paragraph_format_set_t(a_format)));
}

void docscreator::msdoc_actions_t::header_footer_set(
  const header_footer_t& a_header_footer)
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_header_footer_set_t(a_header_footer)));
}

void docscreator::msdoc_actions_t::page_numbers_set(
  const page_numbers_options_t& a_page_numbers_options)
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_page_numbers_set_t(a_page_numbers_options)));
}

void docscreator::msdoc_actions_t::page_setup_set(
  const page_setup_t& a_page_setup)
{
  m_actions.push_back(irs::handle_t<msdoc_action_t>(
    new msdoc_action_page_setup_set_t(a_page_setup)));
}

// class msword_doc_actions_executor_t
docscreator::msword_doc_actions_executor_t::msword_doc_actions_executor_t(
  const msdoc_actions_t* ap_actions,
  msword_t* ap_msword,
  msdocument_t* ap_msdoc
):
  mp_actions(ap_actions),
  mp_msword(ap_msword),
  mp_msdoc(ap_msdoc),
  m_process(p_wait),
  m_action_index(0)
{
}

void docscreator::msword_doc_actions_executor_t::set_actions(
  const msdoc_actions_t* ap_actions)
{
  mp_actions = ap_actions;
}

void docscreator::msword_doc_actions_executor_t::set_msword(
  msword_t* ap_msword)
{
  mp_msword = ap_msword;
}

void docscreator::msword_doc_actions_executor_t::set_msdocument(
  msdocument_t* ap_msdoc)
{
  mp_msdoc = ap_msdoc;
}

docscreator::msdocument_t*
docscreator::msword_doc_actions_executor_t::get_msdocument()
{
  return mp_msdoc;
}

void docscreator::msword_doc_actions_executor_t::exec_actions()
{
  DC_ASSERT(mp_actions);
  DC_ASSERT(mp_msword);
  m_process = p_exec_action;
  m_action_index = 0;
}

bool docscreator::msword_doc_actions_executor_t::is_complete() const
{
  return m_process == p_wait;
}

double docscreator::msword_doc_actions_executor_t::get_progress() const
{
  if (mp_actions->action_count() == 0) {
    return 1.0;
  } else {
    return static_cast<double>(m_action_index)/mp_actions->action_count();
  }

}

void docscreator::msword_doc_actions_executor_t::tick()
{
  switch (m_process) {
    case p_exec_action: {
      if (m_action_index < mp_actions->action_count()) {
        (*mp_actions)[m_action_index]->exec(mp_msword, &mp_msdoc);
        m_action_index++;
      } else {
        m_process = p_wait;
      }
    } break;
    case p_wait: {
      // Ожидаем запуска
    } break;
    default: {
      DC_ASSERT("Неучтенный процесс");
    }
  }
}

// class msdoc_action_create_t
docscreator::msdoc_action_create_t::msdoc_action_create_t()
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_create_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_create_t(*this));
}

void docscreator::msdoc_action_create_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  if ((*ap_msdoc) == IRS_NULL) {
    *ap_msdoc = ap_msword->document_create();
  }
}

// class msdoc_action_open_t
docscreator::msdoc_action_open_t::msdoc_action_open_t(
  const string_type& a_file_name
):
  m_file_name(a_file_name)
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_open_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_open_t(*this));
}

void docscreator::msdoc_action_open_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  DC_ASSERT(ap_msword);
  if ((*ap_msdoc) == IRS_NULL) {
    *ap_msdoc = ap_msword->document_create(m_file_name);
  }
}

// class msdoc_action_close_t
docscreator::msdoc_action_close_t::msdoc_action_close_t()
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_close_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_close_t(*this));
}

void docscreator::msdoc_action_close_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  DC_ASSERT(ap_msword);
  if ((*ap_msdoc) != IRS_NULL) {
    ap_msword->document_delete(*ap_msdoc);
    (*ap_msdoc) = IRS_NULL;
  }
}


// class msdoc_action_save_t
docscreator::msdoc_action_save_t::msdoc_action_save_t()
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_save_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_save_t(*this));
}

void docscreator::msdoc_action_save_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->save();
}

// class msdoc_action_save_as_t
docscreator::msdoc_action_save_as_t::msdoc_action_save_as_t(
  const string_type& a_file_name
):
  m_file_name(a_file_name)
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_save_as_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_save_as_t(*this));
}

void docscreator::msdoc_action_save_as_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->save(m_file_name);
}

// class msdoc_action_text_add_t
docscreator::msdoc_action_text_add_t::msdoc_action_text_add_t(
  const string_type& a_text, const font_t& a_font
):
  m_text(a_text),
  m_font(a_font)
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_text_add_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_text_add_t(*this));
}

void docscreator::msdoc_action_text_add_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->text_add(m_text, m_font);
}

// class msdoc_action_rtf_text_add_t
docscreator::msdoc_action_rtf_text_add_t::msdoc_action_rtf_text_add_t(
  const rtf_text_type& a_rtf_text
):
  m_rtf_text(a_rtf_text)
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_rtf_text_add_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_rtf_text_add_t(*this));
}

void docscreator::msdoc_action_rtf_text_add_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->rtf_text_add(m_rtf_text);
}

// class msdoc_action_table_string_add_t
docscreator::msdoc_action_table_string_add_t::msdoc_action_table_string_add_t(
  const irs::table_string_t& a_table_string
):
  m_table(a_table_string)
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_table_string_add_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_table_string_add_t(*this));
}

void docscreator::msdoc_action_table_string_add_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->table_add(m_table);
}

// class msdoc_action_doc_table_add_t
docscreator::msdoc_action_doc_table_add_t::msdoc_action_doc_table_add_t(
  const doc_table_type& a_table, const table_format_t& a_table_format
):
  m_table(a_table),
  m_table_format(a_table_format)
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_doc_table_add_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_doc_table_add_t(*this));
}

void docscreator::msdoc_action_doc_table_add_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->table_add(m_table, m_table_format);
}

// class msdoc_action_break_add_t
docscreator::msdoc_action_break_add_t::msdoc_action_break_add_t()
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_break_add_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_break_add_t(*this));
}

void docscreator::msdoc_action_break_add_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->break_add();
}

// class msdoc_action_file_add_t
docscreator::msdoc_action_file_add_t::msdoc_action_file_add_t(
  const string_type& a_file_name
):
  m_file_name(a_file_name)
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_file_add_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_file_add_t(*this));
}

void docscreator::msdoc_action_file_add_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->file_add(m_file_name);
}

// class msdoc_action_image_add_t
docscreator::msdoc_action_image_add_t::msdoc_action_image_add_t(
  const string_type& a_file_name,
  const shape_size_and_position_t& a_shape_size_and_position
):
  m_file_name(a_file_name),
  m_shape_size_and_position(a_shape_size_and_position)
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_image_add_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_image_add_t(*this));
}

void docscreator::msdoc_action_image_add_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->image_add(m_file_name, m_shape_size_and_position);
}

// class msdoc_action_paragraph_format_set_t
docscreator::msdoc_action_paragraph_format_set_t::
msdoc_action_paragraph_format_set_t(const paragraph_format_t& a_format
):
  m_format(a_format)
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_paragraph_format_set_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_paragraph_format_set_t(*this));
}

void docscreator::msdoc_action_paragraph_format_set_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->paragraph_format_set(m_format);
}

// class msdoc_action_header_footer_set_t
docscreator::msdoc_action_header_footer_set_t::
msdoc_action_header_footer_set_t(const header_footer_t& a_header_footer):
  m_header_footer(a_header_footer)
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_header_footer_set_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_header_footer_set_t(*this));
}

void docscreator::msdoc_action_header_footer_set_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->header_footer_set(m_header_footer);
}

// class msdoc_action_page_numbers_set_t
docscreator::msdoc_action_page_numbers_set_t::msdoc_action_page_numbers_set_t(
  const page_numbers_options_t& a_page_numbers_options
):
  m_page_numbers_options(a_page_numbers_options)
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_page_numbers_set_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_page_numbers_set_t(*this));
}

void docscreator::msdoc_action_page_numbers_set_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->page_numbers_set(m_page_numbers_options);
}

// class msdoc_action_page_setup_set_t
docscreator::msdoc_action_page_setup_set_t::msdoc_action_page_setup_set_t(
  const page_setup_t& a_page_setup
):
  m_page_setup(a_page_setup)
{
}

irs::handle_t<docscreator::msdoc_action_t>
docscreator::msdoc_action_page_setup_set_t::clone() const
{
  return irs::handle_t<docscreator::msdoc_action_t>(
    new msdoc_action_page_setup_set_t(*this));
}

void docscreator::msdoc_action_page_setup_set_t::exec(
  msword_t* ap_msword, msdocument_t** ap_msdoc) const
{
  (*ap_msdoc)->page_setup_set(m_page_setup);
}
*/
