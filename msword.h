//! \file
//! \brief Компонент документа MS Word.
//!
//! Дата создания: 19.11.2010

#ifndef mswordH
#define mswordH

#include <irsdefs.h>

#include "msword_document.h"
#include <memory>

#include <irsfinal.h>

namespace docscreator {

enum ms_word_version_t {
  ms_word_2003 = 11,
  ms_word_2010 = 14
};

class msword_t
{
public:
  typedef size_t size_type;
  typedef irs::string_t string_type;
  struct var_msdocument_t {
    msdocument_t msdocument;
    std::shared_ptr<QAxObject> var_document;
    var_msdocument_t(const msdocument_t& a_msdocument,
      std::shared_ptr<QAxObject> ap_var_document
    ):
      msdocument(a_msdocument),
      var_document(ap_var_document)
    {}
  };
  msword_t(bool a_word_show = false);
  ~msword_t();
  msdocument_t* document_create();
  msdocument_t* document_create(const string_type& a_file_path);
  void document_delete(msdocument_t* ap_document);
  void all_document_delete();
  void show();
  void hide();
  QVariant selection_get();
  double centimeters_to_points(const double a_centimeters);
  double points_to_centimeters(const double a_points);
  double inches_to_points(const double a_inches);
  double points_to_inches(const double a_points);
  int version_get() const;
private:
  const double m_centimeters_in_inch;
  const int m_points_in_inch;
  irs::handle_t<QAxObject> m_word_app;
  vector<std::shared_ptr<var_msdocument_t> > m_documents;
  int m_version;
};

} // namespace docscreator

#endif // mswordH
