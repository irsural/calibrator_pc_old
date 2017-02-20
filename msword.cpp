//! \file
//! \brief Компонент документа MS Word.
//!
//! Дата создания: 19.11.2010

#include "msword.h"

#include <QDebug>

#include <irsfinal.h>

//class msword_t
docscreator::msword_t::msword_t(bool a_word_show):
  m_centimeters_in_inch(2.54),
  m_points_in_inch(72),
  m_word_app(),
  m_documents(),
  m_version(0)
{
  m_word_app.reset(new QAxObject ("Word.Application"));

  /*String version_builder_str = m_word_app.OlePropertyGet("Version");
  string_type version_str = irs::str_conv<string_type>(version_builder_str);
  size_type pos = version_str.find_first_not_of(irst("0123456789"));
  version_str = version_str.substr(0, pos);
  irs::str_to_num(version_str, &m_version);
*/
  if(a_word_show) {
    show();
  } else {
    hide();
  }
}

docscreator::msword_t::~msword_t()
{
  qDebug() << "~msword_t()" << "\n";
  m_word_app->dynamicCall("Quit()");
}

docscreator::msdocument_t* docscreator::msword_t::document_create()
{
  QAxObject* docs = m_word_app->querySubObject("Documents()");
  std::shared_ptr<QAxObject> document(docs->querySubObject("Add()"));


  //Variant var_documents = m_word_app.OlePropertyGet("Documents");
  //Variant var_document = var_documents.OleFunction("Add");


  std::shared_ptr<var_msdocument_t> p_doc(
    new var_msdocument_t(msdocument_t(this, document.get()), document));
  m_documents.push_back(p_doc);
  return &(m_documents.back()->msdocument);
}

docscreator::msdocument_t*
docscreator::msword_t::document_create(const string_type& a_file_path)
{
  const QString file_name = irs::str_conv<QString>(a_file_path);

  QAxObject* docs = m_word_app->querySubObject("Documents()");
  std::shared_ptr<QAxObject> document(
    docs->querySubObject("Open(const QVariant&)", file_name));


  //Variant var_documents = m_word_app.OlePropertyGet("Documents");
  //Variant var_document = var_documents.OleFunction("Open", a_file_path.c_str());
  std::shared_ptr<var_msdocument_t> p_doc(
    new var_msdocument_t(msdocument_t(this, document.get()), document));

  m_documents.push_back(p_doc);

  return &(m_documents.back()->msdocument);
}

void docscreator::msword_t::document_delete(msdocument_t* ap_document)
{
  bool find_success = false;
  size_type doc_i_for_delete = 0;
  for(size_type doc_i = 0; doc_i < m_documents.size(); doc_i++) {
    if(&(m_documents[doc_i]->msdocument) == ap_document) {
      doc_i_for_delete = doc_i;
      find_success = true;
      break;
    }
  }
  if (find_success) {
    int wd_do_not_save_changes = 0;
    auto var_document = m_documents[doc_i_for_delete]->var_document;
    var_document->dynamicCall("Close(QVariant)", wd_do_not_save_changes);
    /*try {
      var_document.OleProcedure("Close", wd_do_not_save_changes);
    } catch (EOleSysError& ole_sys_error) {
      //Документ закрыт из вне
    }*/
    //m_documents[doc_i_for_delete]->var_document.ChangeType(varNull);
    m_documents.erase(m_documents.begin() + doc_i_for_delete);
  } else {
    IRS_LIB_ASSERT_MSG("Недопустимый указатель");
  }
}

void docscreator::msword_t::all_document_delete()
{
  while (!m_documents.empty()) {
    int wd_do_not_save_changes = 0;
    auto var_document = m_documents.front()->var_document;
    var_document->dynamicCall("Close(QVariant)", wd_do_not_save_changes);
    /*try {
      var_document.OleProcedure("Close", wd_do_not_save_changes);
    } catch (EOleSysError& ole_sys_error) {
      //Документ закрыт из вне
    }
    m_documents.front()->var_document.ChangeType(varNull);*/
    m_documents.erase(m_documents.begin());
  }

}

void docscreator::msword_t::show()
{
  m_word_app->setProperty("Visible", true);
  //m_word_app.OlePropertySet("Visible", true);
}

void docscreator::msword_t::hide()
{
  m_word_app->setProperty("Visible", false);
  //m_word_app.OlePropertySet("Visible", false);
}

QVariant docscreator::msword_t::selection_get()
{
  return m_word_app->property("Selection");
  //return m_word_app.OlePropertyGet("Selection");
}

double docscreator::msword_t::centimeters_to_points(
  const double a_centimeters)
{
  return inches_to_points(a_centimeters/m_centimeters_in_inch);
}

double docscreator::msword_t::points_to_centimeters(
  const double a_points)
{
  return points_to_inches(a_points)*m_centimeters_in_inch;
}

double docscreator::msword_t::inches_to_points(const double a_inches)
{
  return a_inches*m_points_in_inch;
}

double docscreator::msword_t::points_to_inches(const double a_points)
{
  return a_points/m_points_in_inch;
}

int docscreator::msword_t::version_get() const
{
  return m_version;
}

