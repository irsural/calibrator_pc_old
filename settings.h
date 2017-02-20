#ifndef SETTINGS_H
#define SETTINGS_H

#include <irsdefs.h>

#include <cstdio>

#include <irsstrm.h>

#if IRS_USE_JSON_CPP
# include <json/json.h>
#endif // IRS_USE_JSON_CPP

#include "calibrator.h"

#include <irsfinal.h>

template <class T>
std::string num_to_utf_8_classic(T a_num)
{
  irs::string_t str;
  irs::num_to_str_classic(a_num, &str);
  return irs::encode_utf_8(str);
}

template <class T>
bool utf_8_to_num_classic(const Json::Value& a_value, T* ap_num)
{
  std::string str_utf_8 = a_value.asString();
  irs::string_t str = irs::decode_utf_8<irs::string_t>(str_utf_8);
  T num = 0;
  if (!irs::str_to_num_classic(str, &num)) {
    return false;
  }

  *ap_num = num;
  return true;
}

void save(const fixed_step_t& a_fixed_step, Json::Value* ap_value);
void load(const Json::Value& a_value, fixed_step_t* ap_fixed_step);

class settings_t
{
public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;

  static settings_t* instance();
  void save();
  void load();
  usb_hid_settings_t usb_hid_settings;
  calibrator_settings_t calibrator_settings;
  gen_mode_settings_t generator_mode;
private:
  settings_t();
  static settings_t* mp_instance;
  const string_type m_file_name;
};

#endif // SETTINGS_H
