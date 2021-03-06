/**
 * usb_backend_uart.h
 *      send usb report throught the uart 
 */

#include "usb_interface.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "app_usbd.h"
#include "app_usbd_core.h"
#include "app_usbd_hid_generic.h"
#include "app_usbd_serial_num.h"
#include "usb_descriptors.h"

// event listener
static void hid_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                app_usbd_hid_user_event_t event);

// HCK hid custom keyboard
#define HCK_INTERFACE               0
#define HCK_EP_IN                   NRF_DRV_USBD_EPIN1
#define HCK_REPORT_IN_QUEUE_SIZE    1
#define HCK_REPORT_OUT_MAXSIZE      1
#define HCK_REPORT_FEATURE_MAXSIZE  31

#define HCK_EPLIST()  (HCK_EP_IN)

APP_USBD_HID_GENERIC_SUBCLASS_REPORT_DESC(hck_desc,{ TUD_HID_REPORT_DESC_KEYBOARD() });

static const app_usbd_hid_subclass_desc_t * hck_reps[] = {&hck_desc};

APP_USBD_HID_GENERIC_GLOBAL_DEF(m_hck,
                                HCK_INTERFACE,
                                hid_user_ev_handler,
                                HCK_EPLIST(),
                                hck_reps,
                                HCK_REPORT_IN_QUEUE_SIZE,
                                HCK_REPORT_OUT_MAXSIZE,
                                HCK_REPORT_FEATURE_MAXSIZE,
                                APP_USBD_HID_SUBCLASS_BOOT,
                                APP_USBD_HID_PROTO_KEYBOARD);

// HCO hid custom other
#define HCO_INTERFACE               1
#define HCO_EP_IN                   NRF_DRV_USBD_EPIN2
#define HCO_REPORT_IN_QUEUE_SIZE    1
#define HCO_REPORT_OUT_MAXSIZE      1
#define HCO_REPORT_FEATURE_MAXSIZE  31

#define HCO_EPLIST() (HCO_EP_IN)

APP_USBD_HID_GENERIC_SUBCLASS_REPORT_DESC(hco_desc, { TUD_HID_REPORT_DESC_MOUSE( HID_REPORT_ID(HID_REPORT_ID_MOUSE) ),
                                                    TUD_HID_REPORT_DESC_EXTRA( HID_REPORT_ID_SYSTEM, HID_REPORT_ID_CONSUMER) });

static const app_usbd_hid_subclass_desc_t * hco_reps[] = {&hco_desc};
APP_USBD_HID_GENERIC_GLOBAL_DEF(m_hco,
                                HCO_INTERFACE,
                                hid_user_ev_handler,
                                HCO_EPLIST(),
                                hco_reps,
                                HCO_REPORT_IN_QUEUE_SIZE,
                                HCO_REPORT_OUT_MAXSIZE,
                                HCO_REPORT_FEATURE_MAXSIZE,
                                APP_USBD_HID_SUBCLASS_NONE,
                                APP_USBD_HID_PROTO_GENERIC);
                                

typedef struct {
    nrf_usb_event_handler_t event;
    app_usbd_config_t usbd_config;
} nrf_usb_config_t;

static void usbd_user_ev_handler(app_usbd_event_type_t event);

static nrf_usb_config_t nrf_usb_config = {
    .usbd_config.ev_state_proc= usbd_user_ev_handler,
};

void nrf_usb_preinit(void)
{
    ret_code_t rc;
    app_usbd_serial_num_generate();

    if (!nrf_drv_clock_init_check()) {
        rc = nrf_drv_clock_init();
        APP_ERROR_CHECK(rc);
    }

    rc = app_usbd_init(&nrf_usb_config.usbd_config);
    APP_ERROR_CHECK(rc);

    app_usbd_class_inst_t const * class_inst_generic;
    class_inst_generic = app_usbd_hid_generic_class_inst_get(&m_hck);
    rc = app_usbd_class_append(class_inst_generic);
    APP_ERROR_CHECK(rc);

    class_inst_generic = app_usbd_hid_generic_class_inst_get(&m_hco);
    rc = app_usbd_class_append(class_inst_generic);
    APP_ERROR_CHECK(rc);
    NRF_LOG_INFO("NRF USB device initialized");
}

void nrf_usb_init(nrf_usb_event_handler_t* eh)
{
    nrf_usb_config.event.disable_cb = eh->disable_cb;
    nrf_usb_config.event.enable_cb  = eh->enable_cb;
    nrf_usb_config.event.suspend_cb = eh->suspend_cb;
    nrf_usb_config.event.resume_cb  = eh->resume_cb;
    nrf_usb_config.event.leds_cb    = eh->leds_cb;
}

void nrf_usb_postinit(void)
{
    app_usbd_power_events_enable();
}

void nrf_usb_task(void)
{
    while (app_usbd_event_queue_process()) { /* Nothing to do */ }
}

void nrf_usb_send_report(nrf_report_id report, const void *data, size_t size)
{
    ret_code_t rc = NRF_SUCCESS;
    uint8_t buf[16];
    switch(report) {
        case NRF_REPORT_ID_KEYBOARD: 
            rc = app_usbd_hid_generic_in_report_set(&m_hck, data, size);
            APP_ERROR_CHECK(rc);
            NRF_LOG_INFO("Key report:[%x%x]", ((uint32_t*)data)[0], ((uint32_t*)data)[1]);
            break;
        case NRF_REPORT_ID_MOUSE:
            buf[0] = HID_REPORT_ID_MOUSE;
            memcpy(&buf[1], data, size);
            rc = app_usbd_hid_generic_in_report_set(&m_hco, buf, size+1);
            APP_ERROR_CHECK(rc);

            NRF_LOG_INFO("Mouse report:");
            for (int i = 0; i < size; i++) {
                NRF_LOG_INFO("0x%x", ((uint8_t*)data)[i]);
            }
            break;
        case NRF_REPORT_ID_SYSTEM:
            buf[0] = HID_REPORT_ID_SYSTEM;
            memcpy(&buf[1], data, size);
            rc = app_usbd_hid_generic_in_report_set(&m_hco, buf, size+1);
            APP_ERROR_CHECK(rc);
            NRF_LOG_INFO("system report: 0x%x", *((uint32_t*)data));
            break;
        case NRF_REPORT_ID_CONSUMER:
            buf[0] = HID_REPORT_ID_CONSUMER;
            memcpy(&buf[1], data, size);
            rc = app_usbd_hid_generic_in_report_set(&m_hco, buf, size+1);
            APP_ERROR_CHECK(rc);
            NRF_LOG_INFO("consumer report: 0x%x", *((uint32_t*)data));
            break;
        default:
            NRF_LOG_INFO("Unknow report id: %d", report);
            break;
    }
}

void nrf_usb_wakeup(void) 
{
    app_usbd_wakeup_req();
}

void nrf_usb_prepare_sleep(void) { }

void nrf_usb_reboot(void) { }

static void usbd_user_ev_handler(app_usbd_event_type_t event)
{
    switch (event) {
    case APP_USBD_EVT_DRV_SUSPEND:
        nrf_usb_config.event.suspend_cb(true);
        break;
    case APP_USBD_EVT_DRV_RESUME:
        nrf_usb_config.event.resume_cb();
        break;
    case APP_USBD_EVT_STARTED:
        break;
    case APP_USBD_EVT_STOPPED:
        break;
    case APP_USBD_EVT_POWER_DETECTED:
        if (!nrf_drv_usbd_is_enabled()) {
            app_usbd_enable();
        }
        break;
    case APP_USBD_EVT_POWER_REMOVED:
        app_usbd_stop();
        nrf_usb_config.event.disable_cb();
        break;
    case APP_USBD_EVT_POWER_READY:
        app_usbd_start();
        nrf_usb_config.event.enable_cb();
        break;
    default:
        break;
    }
}

static void hid_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                app_usbd_hid_user_event_t event)
{
    switch (event) {
    case APP_USBD_HID_USER_EVT_OUT_REPORT_READY: {
        NRF_LOG_INFO("USBD HID OUT report ready");
        app_usbd_class_inst_t const * kbd = app_usbd_hid_generic_class_inst_get(&m_hck);
        if (p_inst == kbd) {
            size_t size = 0;
            const void* data = app_usbd_hid_generic_out_report_get(&m_hck, &size);
            NRF_LOG_INFO("USBD out report size=%d", size);
            if (size == 2) {
                uint8_t* pd = (uint8_t*)data;
                NRF_LOG_INFO("USBD out report byte[0]=%d, byte[1]=%d", pd[0], pd[1]);
                nrf_usb_config.event.leds_cb(pd[1]);
            }
        }
    } break;
    case APP_USBD_HID_USER_EVT_IN_REPORT_DONE: {
    } break;
    case APP_USBD_HID_USER_EVT_SET_BOOT_PROTO: {
        NRF_LOG_INFO("SET_BOOT_PROTO");
    } break;
    case APP_USBD_HID_USER_EVT_SET_REPORT_PROTO: {
        NRF_LOG_INFO("SET_REPORT_PROTO");
    } break;
    default:
        break;
    }
}