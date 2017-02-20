#include <irspch.h>

#include "settings.h"

#include <irsfinal.h>

void save(const fixed_step_t& a_fixed_step, Json::Value* ap_value)
{
  const fixed_step_t* fs = &a_fixed_step;

  (*ap_value)["selected_fixed_step"] =
    num_to_utf_8_classic(fs->selected_fixed_step);

  (*ap_value)["user_step"] = num_to_utf_8_classic(fs->user_step);
}

void load(const Json::Value& a_value, fixed_step_t* ap_fixed_step)
{
  fixed_step_t* fs = ap_fixed_step;

  utf_8_to_num_classic(a_value["selected_fixed_step"],
    &fs->selected_fixed_step);

  utf_8_to_num_classic(a_value["user_step"], &fs->user_step);
}


// class settings_t
settings_t* settings_t::mp_instance = 0;

settings_t::settings_t():
  usb_hid_settings(),
  calibrator_settings(),
  generator_mode(),
  m_file_name(irst("calibratorpc.cfg"))
{
  mp_instance = this;
}

settings_t* settings_t::instance()
{
  return mp_instance ? mp_instance : new settings_t();
}
void settings_t::save()
{
  Json::Value cfg;

  gen_mode_settings_t* gm = &instance()->generator_mode;
  Json::Value generator_mode;
  generator_mode["fine_step"] = num_to_utf_8_classic(gm->fine_step);
  generator_mode["normal_step"] = num_to_utf_8_classic(gm->normal_step);
  generator_mode["coarse_step"] = num_to_utf_8_classic(gm->coarse_step);

  ::save(gm->dcv_fixed_step, &generator_mode["dcv_fixed_step"]);
  ::save(gm->acv_fixed_step, &generator_mode["acv_fixed_step"]);
  ::save(gm->dci_fixed_step, &generator_mode["dci_fixed_step"]);
  ::save(gm->aci_fixed_step, &generator_mode["aci_fixed_step"]);

  cfg["generator_mode"] = generator_mode;

  ofstream ofile;
  ofile.open(IRS_SIMPLE_FROM_TYPE_STR(m_file_name.c_str()),
    ios::in|ios::out|ios::trunc);
  if (!ofile.good()) {
    throw runtime_error("Не удалось сохранить файл");
  }
  ofile << cfg << std::endl;
}

void settings_t::load()
{
  std::ifstream ifile;
  ifile.open(IRS_SIMPLE_FROM_TYPE_STR(m_file_name.c_str()), ios::in);
  if (!ifile.good()) {
    return;
    //throw runtime_error("Не удалось загрузить файл");
  }

  Json::Reader reader;
  Json::Value cfg;
  const bool collect_comments = false;
  const bool parsed_success = reader.parse(ifile, cfg, collect_comments);
  if(!parsed_success) {
    std::cout << "Failed to parse JSON" <<
      std::endl << reader.getFormatedErrorMessages() <<
      std::endl;
    return;
    //throw std::runtime_error("Неудалось разобрать файл");
  }

  const Json::Value generator_mode = cfg["generator_mode"];
  gen_mode_settings_t* gm = &instance()->generator_mode;
  utf_8_to_num_classic(generator_mode["fine_step"], &gm->fine_step);
  utf_8_to_num_classic(generator_mode["normal_step"], &gm->normal_step);
  utf_8_to_num_classic(generator_mode["coarse_step"], &gm->coarse_step);

  ::load(generator_mode["dcv_fixed_step"], &gm->dcv_fixed_step);
  ::load(generator_mode["acv_fixed_step"], &gm->acv_fixed_step);
  ::load(generator_mode["dci_fixed_step"], &gm->dci_fixed_step);
  ::load(generator_mode["aci_fixed_step"], &gm->aci_fixed_step);
}
