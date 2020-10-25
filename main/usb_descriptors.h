/**
 *  usb_descriptors.h
 */ 

#pragma once

#include "tusb.h"

//=======================================================================
// usb configuration
//=======================================================================
enum {
  ITF_NUM_HID_KBD,
  ITF_NUM_HID_OTHER,
  ITF_NUM_VENDOR,
  ITF_NUM_TOTAL
};

#define EPNUM_HID_KBD       0x01
#define EPNUM_HID_OTHER     0x02
#define EPNUM_VENDOR_OUT    0x03
#if defined(STM32F103xB)
#define EPNUM_VENDOR_IN     0x04
#else
#define EPNUM_VENDOR_IN     0x03
#endif



enum {
    HID_REPORT_ID_KEYBOARD,
    HID_REPORT_ID_MOUSE,
    HID_REPORT_ID_SYSTEM,
    HID_REPORT_ID_CONSUMER,
    HID_REPORT_ID_WEBUSB,
};

enum {
    VENDOR_REQUEST_WEBUSB = 1,
    VENDOR_REQUEST_MICROSOFT = 2
};

uint8_t const* tud_descriptor_device_cb(void);
uint32_t tud_descriptor_device_size(void);

uint8_t const* tud_descriptor_configuration_cb(uint8_t index);
uint32_t tud_descriptor_configuration_size(uint8_t index);

uint8_t const* tud_hid_descriptor_report_cb(uint8_t itf);

uint8_t const* tud_descriptor_hid_report_kbd_cb(void);
uint32_t tud_descriptor_hid_report_kbd_size(void);

uint8_t const* tud_descriptor_hid_report_other_cb(void);
uint32_t tud_descriptor_hid_report_other_size(void);

uint8_t const* tud_descriptor_hid_kbd_cb(void);
uint32_t tud_descriptor_hid_kbd_size(void);

uint8_t const* tud_descriptor_hid_other_cb(void);
uint32_t tud_descriptor_hid_other_size(void);

uint8_t const* tud_descriptor_bos_cb(void);
uint32_t tud_descriptor_bos_size(void);

uint8_t const* tud_descriptor_url_cb(void);
uint32_t tud_descriptor_url_size(void);

uint8_t const* tud_descriptor_msos20_cb(void);
uint32_t tud_descriptor_msos20_size(void);

enum {
    DESC_STR_LANGID,
    DESC_STR_MANUFACTURE,
    DESC_STR_PRODUCT,
    DESC_STR_SERIAL,
    DESC_STR_CONFIG_HID,
    DESC_STR_INTERFACE_HID,
    DESC_STR_INTERFACE_WEBUSB,
};

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid);