#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <irsdefs.h>

#include <cstdio>
#include <map>

#include <irsstrm.h>
#include <irssysutils.h>
#include <irsdevices.h>
#include <irsmbus.h>

#include <irsfinal.h>

const double dcv_max = 630;
const double acv_max = 630;
const double dci_max = 11;
const double aci_max = 11;
const double dcv_step_min = 2e-7;
const double dcv_step_max = dcv_max/2;
const double acv_step_min = 2e-6;
const double acv_step_max = acv_max/2;
const double dci_step_min = 2e-9;
const double dci_step_max = dci_max/2;
const double aci_step_min = 2e-6;
const double aci_step_max = aci_max/2;
const int out_precision = 7;

class usb_hid_device_path_map_t
{
public:
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;
  typedef std::map<string_type, string_type> map_type;
  usb_hid_device_path_map_t();
  const map_type& get_map() const;
  bool changed();
  void tick();
private:
  void update();
  const string_type m_manufacturer;
  const string_type m_product;
  map_type m_map;
  bool m_changed;
  irs::loop_timer_t m_update_timer;
};

struct usb_hid_settings_t
{
  typedef irs::string_t string_type;
  string_type manufacturer;
  string_type product;
  irs_u16 vendor_id;
  irs_u16 product_id;
  usb_hid_settings_t():
    manufacturer(irst("RES")),
    product(irst("Calibrator in FS Mode")),
    vendor_id(0x0483),
    product_id(0x5711)
  {
  }
};

struct modbus_settings_t
{
  typedef irs_size_t size_type;
  size_type discret_inputs;
  size_type coils;
  size_type holding_registers;
  size_type input_registers;
  double update_period;
  modbus_settings_t():
    discret_inputs(0),
    coils(0),
    holding_registers(565),
    input_registers(0),
    update_period(0.1)
  {
  }
};

struct fixed_step_t
{
  size_t selected_fixed_step = 0;
  double user_step = 0;
};

struct gen_mode_settings_t
{
  /*enum mode_t {
    mode_dcv,
    mode_acv,
    mode_dci,
    mode_aci
  };
  mode_t mode = mode_dcv;*/
  double setpoint = 0;
  double step = 0;
  double fine_step = 0.00002;
  double normal_step = 0.0005;
  double coarse_step = 0.005;
  fixed_step_t dcv_fixed_step;
  fixed_step_t acv_fixed_step;
  fixed_step_t dci_fixed_step;
  fixed_step_t aci_fixed_step;
};

struct calibrator_settings_t
{
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;
  string_type device_path;
  size_type modbus_channel = 1;
  modbus_settings_t modbus_settings;
};

enum generator_mode_t {
  generator_mode_dc_voltage,
  generator_mode_ac_voltage,
  generator_mode_dc_current,
  generator_mode_ac_current
};

enum generator_status_t
{
  //generator_status_disabled,
  generator_status_busy,
  generator_status_ready
};

enum device_type_t
{
  device_type_analog,
  device_type_digital
};

template <class T, int size = sizeof(T), bool check = false>
class conn_data_sync_t
{
public:
  conn_data_sync_t(double a_sync_timeout = 1.);
  irs_uarc connect(irs::mxdata_t *a_data, irs_uarc a_index);
  const T &operator=(const T &a_elem);
  const conn_data_sync_t& operator=(const conn_data_sync_t& a_conn_data_sync);
  operator T();
  bool changed();
private:
  irs::conn_data_t<T, size, check> m_conn_data;
  T m_value;
  T m_prev_value;
  irs::timer_t m_sync_timeout;
};

template <class T, int size, bool check>
conn_data_sync_t<T, size, check>::conn_data_sync_t(double a_sync_timeout):
  m_conn_data(),
  m_value(0),
  m_sync_timeout(irs::make_cnt_s(a_sync_timeout))
{
}

template <class T, int size, bool check>
irs_uarc conn_data_sync_t<T, size, check>::connect(irs::mxdata_t *a_data,
  irs_uarc a_index)
{
  return m_conn_data.connect(a_data, a_index);
}
template <class T, int size, bool check>
const T& conn_data_sync_t<T, size, check>::operator=(const T &a_elem)
{
  if (m_value == a_elem) {
    return a_elem;
  }
  m_value = a_elem;
  m_conn_data.operator=(a_elem);
  m_sync_timeout.start();
  return a_elem;
}
template <class T, int size, bool check>
const conn_data_sync_t<T, size, check>&
conn_data_sync_t<T, size, check>::operator=(
  const conn_data_sync_t<T, size, check>& a_conn_data_sync)
{
  T elem = a_conn_data_sync;
  operator=(elem);
  return *this;
}
template <class T, int size, bool check>
conn_data_sync_t<T, size, check>::operator T()
{
  m_sync_timeout.check();
  if (m_sync_timeout.stopped()) {
    m_value = m_conn_data;
  }
  return m_value;
}

template <class T, int size, bool check>
bool conn_data_sync_t<T, size, check>::changed()
{
  operator T();
  bool changed = (m_prev_value != m_value);
  m_prev_value = m_value;

  return changed;
}

class bit_data_sync_t
{
public:
  bit_data_sync_t(double a_sync_timeout = 1.);
  irs_uarc connect(irs::mxdata_t *a_data, irs_uarc a_index,
    irs_uarc a_bit_index);
  bool operator=(const bool a_elem);
  const bit_data_sync_t& operator=(const bit_data_sync_t& a_bit_data_sync);
  operator bool();
  bool changed();
private:
  irs::bit_data_t m_conn_data;
  bool m_value;
  bool m_prev_value;
  irs::timer_t m_sync_timeout;
};

class calibrator_t
{
public:
  calibrator_t();
  void connect(irs::mxdata_t* ap_data);
  bool connected() const;
  double get_battery_voltage();
  bool get_signal_enabled();
  void set_signal_enabled(bool a_enable);
  bool is_setpoint_changed();
  double get_setpoint();
  void get_setpoint(generator_mode_t* ap_mode, double* ap_value);
  void set_setpoint(generator_mode_t a_mode, double a_value);
  bool get_reverse_enabled();
  void set_reverse_enabled(bool a_enable);
  bool is_frequency_changed();
  double get_frequency();
  void set_frequency(const double a_value);
  double get_actual_value() const;
  void set_step(double a_value);
  double get_step();
  generator_status_t get_status();
  time_t get_time();
  void set_time(const time_t a_time);
private:
  struct network_variables_t
  {
    void connect(irs::mxdata_t* ap_data);
    irs::conn_data_t<double> battery_v;
    irs::bit_data_t signal_on;
    conn_data_sync_t<double> setpoint;
    bit_data_sync_t current_enabled;
    bit_data_sync_t dc_enabled;
    bit_data_sync_t reverse;
    irs::bit_data_t source_ready;
    irs::conn_data_t<double> actual_value;
    conn_data_sync_t<double> frequency;
    irs::conn_data_t<irs_u64> time;
    irs::conn_data_t<irs_u64> set_time;
  };
  irs::mxdata_t* mp_data;
  network_variables_t m_network_variables;
  double m_setpoint;
};

class calibrator_link_t: public irs::mxdata_assembly_t
{
public:
public:
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;

  calibrator_link_t(const calibrator_settings_t& a_calibrator_settings);
  virtual ~calibrator_link_t();
  virtual bool enabled() const;
  virtual void enabled(bool a_enabled);
  virtual irs::mxdata_t* mxdata();
  virtual void tick();
  void set_settings(const calibrator_settings_t& a_settings);
  const calibrator_settings_t& get_settings() const;
  virtual void show_options();
  virtual void tstlan4(irs::tstlan4_base_t* ap_tstlan4);
  void calibrator(calibrator_t* ap_calibrator);
  virtual status_t get_status();
  virtual error_string_list_type get_last_error_string_list();
private:
  void add_error(const string_type& a_error);
  calibrator_settings_t m_settings;
  //param_box_tune_t m_param_box_tune;
  irs::tstlan4_base_t* mp_tstlan4;
  calibrator_t* mp_calibrator;
  bool m_enabled;
  irs::handle_t<irs::hardflow_t> mp_modbus_client_hardflow;
  irs::handle_t<irs::mxdata_t> mp_modbus_client;
  bool m_activated;
  irs::loop_timer_t m_activation_timer;
  enum { error_list_max_size = 100 };
  error_string_list_type m_error_list;

  static irs::handle_t<irs::mxdata_t> make_client(
    irs::handle_t<irs::hardflow_t> ap_hardflow,
    const modbus_settings_t& a_modbus_settings);
  irs::handle_t<irs::hardflow_t> make_hardflow();
  void try_create_modbus();
  void create_modbus();
  void destroy_modbus();
};

#endif // CALIBRATOR_H
