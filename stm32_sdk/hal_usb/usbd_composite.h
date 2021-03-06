/**
 * @file usbd_composite.h
 *  composite device handler
 */

#pragma once

#include "usbd_def.h"

extern USBD_ClassTypeDef USBD_COMP;

uint8_t usbd_comp_send(USBD_HandleTypeDef *pdev, uint8_t type, uint8_t *report, uint16_t len);