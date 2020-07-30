/**************************************************************************
 *
 * Copyright (C) 2009 Steve Karg <skarg@users.sourceforge.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *********************************************************************/
#include <stddef.h>
#include <stdint.h>
#include "bacnet/config.h"
#include "bacnet/bacdef.h"
#include "bacnet/bacdcode.h"
#include "bacnet/basic/binding/address.h"
#include "bacnet/npdu.h"
#include "bacnet/apdu.h"
#include "bacnet/bactext.h"
#include "bacnet/readrange.h"
/* some demo stuff needed */
#include "bacnet/basic/object/device.h"
#include "bacnet/basic/services.h"
#include "bacnet/basic/tsm/tsm.h"

/** @file h_rr_a.c  Handles Read Range Acknowledgments. */

/* for debugging... */
static void PrintReadRangeData(BACNET_READ_RANGE_DATA *data)
{
#ifdef BACAPP_PRINT_ENABLED
    BACNET_OBJECT_PROPERTY_VALUE object_value; /* for bacapp printing */
#endif
    BACNET_APPLICATION_DATA_VALUE value; /* for decode value data */
    int len = 0;
    uint8_t *application_data;
    int application_data_len;
    bool first_value = true;
#if PRINT_ENABLED
    bool print_brace = false;
#endif

    if (data) {
        application_data = data->application_data;
        application_data_len = data->application_data_len;
        /* FIXME: what if application_data_len is bigger than 255? */
        /* value? need to loop until all of the len is gone... */
        for (;;) {
            len = bacapp_decode_application_data(
                application_data, (uint8_t)application_data_len, &value);
            if (first_value && (len < application_data_len)) {
                first_value = false;
#if PRINT_ENABLED
                fprintf(stdout, "{");
                print_brace = true;
#endif
            }
#ifdef BACAPP_PRINT_ENABLED
            object_value.object_type = data->object_type;
            object_value.object_instance = data->object_instance;
            object_value.object_property = data->object_property;
            object_value.array_index = data->array_index;
            object_value.value = &value;
            bacapp_print_value(stdout, &object_value);
#endif
            if (len > 0) {
                if (len < application_data_len) {
                    application_data += len;
                    application_data_len -= len;
                    /* there's more! */
#if PRINT_ENABLED
                    fprintf(stdout, ",");
#endif
                } else {
                    break;
                }
            } else {
                break;
            }
        }
#if PRINT_ENABLED
        if (print_brace)
            fprintf(stdout, "}");
        fprintf(stdout, "\r\n");
#endif
    }
}

void handler_read_range_ack(uint8_t *service_request,
    uint16_t service_len,
    BACNET_ADDRESS *src,
    BACNET_CONFIRMED_SERVICE_ACK_DATA *service_data,
    void *token)
{
    int len = 0;
    BACNET_READ_RANGE_DATA data;

    (void)src;
    (void)service_data; /* we could use these... */
    len = rr_ack_decode_service_request(service_request, service_len, &data);

#if PRINT_ENABLED
    fprintf(stderr, "Received ReadRange Ack!\n");
#endif

    if (len > 0) {
        PrintReadRangeData(&data);
    }
}
