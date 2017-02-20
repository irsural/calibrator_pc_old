#include "calibrator.h"

// class usb_hid_device_path_map_t
usb_hid_device_path_map_t::usb_hid_device_path_map_t():
  m_manufacturer(irst("RES")),
  m_product(irst("N4-25")),
  m_map(),
  m_changed(false),
  m_update_timer(irs::make_cnt_s(1))
{
}

const usb_hid_device_path_map_t::map_type&
usb_hid_device_path_map_t::get_map() const
{
  return m_map;
}

bool usb_hid_device_path_map_t::changed()
{
  bool changed = m_changed;
  m_changed = false;
  return changed;
}

void usb_hid_device_path_map_t::tick()
{
  if (m_update_timer.check()) {
    update();
  }
}

void usb_hid_device_path_map_t::update()
{
  #if IRS_USE_HID_WIN_API
  map_type map = m_map;
  m_map.clear();
  std::vector<irs::usb_hid_device_info_t> devs =
    irs::usb_hid_info_t::get_instance()->get_devices_info();
  for (size_type i = 0; i < devs.size(); i++) {
    string_type device;

    if (m_manufacturer != devs[i].attributes.manufacturer) {
      continue;
    }
    if (m_product != devs[i].attributes.product) {
      continue;
    }

    /*device += devs[i].attributes.manufacturer;
    if (!device.empty() && !devs[i].attributes.product.empty()) {
      device += irst(" ");
    }*/
    /*device += devs[i].attributes.product;
    if (!device.empty() && !devs[i].attributes.serial_number.empty()) {
      device += irst(" ");
    }*/
    device += devs[i].attributes.serial_number;
    /*if (!device.empty() && !devs[i].path.empty()) {
      device += irst(" ");
    }
    device += devs[i].path;
*/
    m_map.insert(make_pair(device, devs[i].path));
  }
  if (map != m_map) {
    m_changed = true;
  }
  #else //IRS_USE_HID_WIN_API
  m_map.insert(
    make_pair(irst("define IRS_USE_HID_WIN_API выключен!"), irst("")));
  #endif //IRS_USE_HID_WIN_API
}

// class bit_data_sync_t
bit_data_sync_t::bit_data_sync_t(double a_sync_timeout):
  m_conn_data(),
  m_value(false),
  m_sync_timeout(irs::make_cnt_s(a_sync_timeout))
{
}

irs_uarc bit_data_sync_t::connect(irs::mxdata_t *a_data,
  irs_uarc a_index, irs_uarc a_bit_index)
{
  return m_conn_data.connect(a_data, a_index, a_bit_index);
}

bool bit_data_sync_t::operator=(const bool a_elem)
{
  if (m_value == a_elem) {
    return a_elem;
  }
  m_value = a_elem;
  m_conn_data.operator=(a_elem);
  m_sync_timeout.start();
  return a_elem;
}

const bit_data_sync_t&
bit_data_sync_t::operator=(const bit_data_sync_t& a_bit_data_sync)
{
  bit_data_sync_t bit_data_sync = a_bit_data_sync;
  const bool elem = bit_data_sync;
  operator=(elem);
  return *this;
}

bit_data_sync_t::operator bool()
{
  m_sync_timeout.check();
  if (m_sync_timeout.stopped()) {
    m_value = m_conn_data;
  }
  return m_value;
}

bool bit_data_sync_t::changed()
{
  operator bool();
  bool changed = (m_prev_value != m_value);
  m_prev_value = m_value;

  return changed;
}

// class calibrator_t
calibrator_t::calibrator_t():
  mp_data(NULL),
  m_network_variables(),
  m_setpoint(0)
{
}

void calibrator_t::connect(irs::mxdata_t* ap_data)
{
  mp_data = ap_data;
  m_network_variables.connect(mp_data);
}

bool calibrator_t::connected() const
{
  if (mp_data) {
    if (mp_data->connected()) {
      return true;
    }
  }
  return false;
}

double calibrator_t::get_battery_voltage()
{
  if (!connected()) {
    return 0;
  }
  return m_network_variables.battery_v;
}

bool calibrator_t::get_signal_enabled()
{
  if (!connected()) {
    return false;
  }
  return (m_network_variables.signal_on == 1);
}

void calibrator_t::set_signal_enabled(bool a_enable)
{
  if (!connected()) {
    return;
  }
  m_network_variables.signal_on = (a_enable == 0) ? 0 : 1;
}

bool calibrator_t::is_setpoint_changed()
{
  if (!connected()) {
    return false;
  }
  bool changed = (m_network_variables.setpoint != m_setpoint);
  m_setpoint = m_network_variables.setpoint;
  return changed;
}

double calibrator_t::get_setpoint()
{
  if (!connected()) {
    return 0;
  }
  return m_network_variables.setpoint;
}

void calibrator_t::get_setpoint(generator_mode_t* ap_mode, double* ap_value)
{
  if (connected()) {
    network_variables_t& nvars = m_network_variables;

    *ap_value = nvars.setpoint;

    if (nvars.current_enabled) {
      if (nvars.dc_enabled) {
        *ap_mode = generator_mode_dc_current;
      } else {
        *ap_mode = generator_mode_ac_current;
      }
    } else {
      if (nvars.dc_enabled) {
        *ap_mode = generator_mode_dc_voltage;
      } else {
        *ap_mode = generator_mode_ac_voltage;
      }
    }
  } else {
    *ap_mode = generator_mode_dc_voltage;
    *ap_value = 0;
  }
}

void calibrator_t::set_setpoint(generator_mode_t a_mode, double a_value)
{
  if (connected()) {
    network_variables_t& nvars = m_network_variables;

    //nvars.signal_on = (a_value == 0) ? 0 : 1;

    nvars.setpoint = a_value;

    nvars.current_enabled = (a_mode == generator_mode_dc_current) |
      (a_mode == generator_mode_ac_current);

    nvars.dc_enabled = (a_mode == generator_mode_dc_voltage) |
      (a_mode == generator_mode_dc_current);
  }
}

bool calibrator_t::get_reverse_enabled()
{
  if (!connected()) {
    return false;
  }
  return m_network_variables.reverse;
}

void calibrator_t::set_reverse_enabled(bool a_enable)
{
  if (!connected()) {
    return;
  }
  m_network_variables.reverse = a_enable;
}

bool calibrator_t::is_frequency_changed()
{
  return m_network_variables.frequency.changed();
}

double calibrator_t::get_frequency()
{
  if (!connected()) {
    return 0;
  }
  return m_network_variables.frequency;
}

void calibrator_t::set_frequency(const double a_value)
{
  if (!connected()) {
    return;
  }
  m_network_variables.frequency = a_value;
}

double calibrator_t::get_actual_value() const
{
  if (connected()) {
    return m_network_variables.actual_value;
  }
  return 0;
}

void calibrator_t::set_step(double a_value)
{

}

double calibrator_t::get_step()
{
  return 0;
}

generator_status_t calibrator_t::get_status()
{
  if (connected()) {
    network_variables_t& nvars = m_network_variables;
    return nvars.source_ready ? generator_status_ready : generator_status_busy;
  }
  return generator_status_ready;
}

time_t calibrator_t::get_time()
{
  if (connected()) {
    return m_network_variables.time;
  }
  return 0;
}

void calibrator_t::set_time(const time_t a_time)
{
  if (!connected()) {
    return;
  }
  m_network_variables.set_time = a_time;
}

void calibrator_t::network_variables_t::connect(irs::mxdata_t* ap_data)
{
  battery_v.connect(ap_data, 44);
  signal_on.connect(ap_data, 60, 0);
  setpoint.connect(ap_data, 61);
  current_enabled.connect(ap_data, 69, 0);
  dc_enabled.connect(ap_data, 69, 1);
  reverse.connect(ap_data, 69, 2);
  source_ready.connect(ap_data, 220, 0);
  actual_value.connect(ap_data, 229);
  frequency.connect(ap_data, 293);
  time.connect(ap_data, 1072);
  set_time.connect(ap_data, 1080);
}

// class calibrator_link_t
irs::handle_t<irs::mxdata_t> calibrator_link_t::make_client(
  irs::handle_t<irs::hardflow_t> ap_hardflow,
  const modbus_settings_t& a_modbus_settings)
{
  return new irs::modbus_client_t(
    ap_hardflow.get(),
    irs::modbus_client_t::mode_refresh_auto,
    a_modbus_settings.discret_inputs,
    a_modbus_settings.coils,
    a_modbus_settings.holding_registers,
    a_modbus_settings.input_registers,
    irs::make_cnt_s(a_modbus_settings.update_period)
  );
}
irs::handle_t<irs::hardflow_t> calibrator_link_t::make_hardflow()
{
  irs::handle_t<irs::hardflow_t> hardflow_ret = IRS_NULL;
  /*const string_type device =
    mp_param_box->get_param(irst("Имя устройства"));
  map<string_type, string_type>::const_iterator it =
    m_usb_hid_device_path_map.find(device);*/
  const string_type device_path = m_settings.device_path;
  /*if (it != m_usb_hid_device_path_map.end()) {
    device_path = it->second;
  }*/
  const irs::hardflow_t::size_type channel_id = m_settings.modbus_channel;
  hardflow_ret.reset(new irs::hardflow::usb_hid_t(device_path, channel_id));
  return hardflow_ret;
}

calibrator_link_t::calibrator_link_t(const calibrator_settings_t& a_calibrator_settings):
  m_settings(a_calibrator_settings),
  mp_tstlan4(NULL),
  mp_calibrator(NULL),
  m_enabled(false),
  mp_modbus_client_hardflow(NULL),
  mp_modbus_client(NULL),
  m_activated(false),
  m_activation_timer(irs::make_cnt_s(1))
{
  //mp_tstlan4->ini_name(m_conf_file_name);
}
calibrator_link_t::~calibrator_link_t()
{
}

void calibrator_link_t::add_error(const string_type& a_error)
{
  stringstream s;
  s << irs::sdatetime;
  string_type msg = irs::str_conv<string_type>(s.str()) + a_error;
  m_error_list.push_back(msg);
  if (m_error_list.size() > error_list_max_size) {
    m_error_list.pop_front();
  }
}

bool calibrator_link_t::enabled() const
{
  return m_enabled;
}
void calibrator_link_t::enabled(bool a_enabled)
{
  if (a_enabled == m_enabled) {
    return;
  }
  if (a_enabled) {
    try_create_modbus();
  } else {
    destroy_modbus();
  }
  m_enabled = a_enabled;
}

void calibrator_link_t::try_create_modbus()
{
  try {
    create_modbus();
  } catch (std::runtime_error& e) {
    const string_type s = irs::str_conv<string_type>(string(e.what()));
    add_error(s);
    destroy_modbus();
  } catch (...) {
    add_error(irst("Неизвестная ошибка"));
    destroy_modbus();
  }
}

void calibrator_link_t::create_modbus()
{
  mp_modbus_client_hardflow = make_hardflow();
  mp_modbus_client = make_client(mp_modbus_client_hardflow,
    m_settings.modbus_settings);
  //mp_tstlan4->connect(mp_modbus_client.get());
  mp_calibrator->connect(mp_modbus_client.get());
  m_activated = true;
}

void calibrator_link_t::destroy_modbus()
{
  //mp_tstlan4->connect(NULL);
  mp_calibrator->connect(NULL);
  mp_modbus_client.reset();
  mp_modbus_client_hardflow.reset();
  m_activated = false;
}

irs::mxdata_t* calibrator_link_t::mxdata()
{
  return mp_modbus_client.get();
}
void calibrator_link_t::tick()
{
  if (!mp_modbus_client.is_empty()) {
    mp_modbus_client->tick();
    const string_type error_string =
      mp_modbus_client_hardflow->param(irst("error_string"));
    if (!error_string.empty()) {
      add_error(error_string);
      destroy_modbus();
    }
  }
  if (m_enabled && !m_activated && m_activation_timer.check()) {
    try_create_modbus();
  }
}

void calibrator_link_t::set_settings(const calibrator_settings_t& a_settings)
{
  m_settings = a_settings;
}

const calibrator_settings_t& calibrator_link_t::get_settings() const
{
  return m_settings;
}

void calibrator_link_t::show_options()
{
}

void calibrator_link_t::tstlan4(irs::tstlan4_base_t* ap_tstlan4)
{
  mp_tstlan4 = ap_tstlan4;
}

void calibrator_link_t::calibrator(calibrator_t* ap_calibrator)
{
  mp_calibrator = ap_calibrator;
}

calibrator_link_t::status_t calibrator_link_t::get_status()
{
  if (m_activated) {
    if (mp_modbus_client->connected()) {
      return status_connected;
    }
  }
  if (m_enabled) {
    return status_busy;
  }
  return status_disabled;
}

calibrator_link_t::error_string_list_type
calibrator_link_t::get_last_error_string_list()
{
  error_string_list_type error_list = m_error_list;
  m_error_list.clear();
  return error_list;
}
