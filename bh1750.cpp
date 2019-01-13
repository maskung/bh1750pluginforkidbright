/***
 * BH1750 Lux meter sensor plugin for kidbright
 *
 * Suphanut Thanyaboon <suphanut@gmail.com>
 * Metropolitan Maker Club <KMMC>
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "esp_system.h"
#include "kidbright32.h"
#include "driver/i2c.h"
#include "bh1750.h"

BH1750::BH1750(int bus_ch, int dev_addr) {
	channel = bus_ch;
	address = dev_addr;
	polling_ms = BH1750_POLLING_MS;
}

void BH1750::init(void) {
	first_read = true;
	illuminance = 0;
	state = s_detect;
}

int BH1750::prop_count(void) {
	return 1;
}

bool BH1750::prop_name(int index, char *name) {
	if (index == 0) {
		snprintf(name, DEVICE_PROP_NAME_LEN_MAX, "%s", "illuminance");
		return true;
	}

	// not supported
	return false;
}

bool  BH1750::prop_unit(int index, char *unit) {
	if (index == 0) {
		snprintf(unit, DEVICE_PROP_UNIT_LEN_MAX, "%s", "lux");
		return true;
	}

	// not supported
	return false;
}

bool BH1750::prop_attr(int index, char *attr) {
	if (index == 0) {
		get_attr_str(attr, PROP_ATTR_READ_FLAG | PROP_ATTR_TYPE_NUMBER_FLAG); // read only, number
		return true;
	}

	// not supported
	return false;
}

bool BH1750::prop_read(int index, char *value) {
	if (index == 0) {
		snprintf(value, DEVICE_PROP_VALUE_LEN_MAX, "%f", illuminance);
		return true;
	}

	// not supported
	return false;
}

bool BH1750::prop_write(int index, char *value) {
	// not supported
	return false;
}

void BH1750::process(Driver *drv) {
	I2CDev *i2c = (I2CDev *)drv;
	uint8_t byte, data[2];

	//printf("%d\n\r",state);
	switch (state) {
		case s_detect:
			// stamp polling tickcnt
			polling_tickcnt = get_tickcnt();
			// detect i2c device
			if (i2c->detect(channel, address) == ESP_OK) {
					// power up
					data[0] = 0x01;
					i2c->write(channel, address, data, 1);

					//config mode
					data[0] = 0x10;
					if (i2c->write(channel, address, data, 1) == ESP_OK) {
						// clear error flag
						error = false;
						// get current tickcnt
						tickcnt = get_tickcnt();
						state = s_get_lux;
					}
					else {
						state = s_error;
					}
				}
			else {
				state = s_error;
			}
			break;

		case s_get_lux:
			// delay > 400ms
			if (is_tickcnt_elapsed(tickcnt, 500) || (!first_read)) {
				// clear first read flag
				first_read = false;

					//byte = 0xac;
					//if (i2c->read(channel, address, &byte, 1, data, 2) == ESP_OK) {
					if (i2c->read(channel, address, NULL, 0, data, 2) == ESP_OK) {
						// calc illuminance in lux
						illuminance = (data[0] << 8) | data[1] ;
						illuminance = (int)(illuminance/1.2); // convert to lux
						// set initialized flag
						initialized = true;

						//printf("====> BEGIN <====\r\n");
						//printf("%x\r\n",data[0]);
						//printf("%x\r\n",data[1]);
						//printf("====> END <====\r\n");

						// load polling tickcnt
						tickcnt = polling_tickcnt;
						// goto wait and retry with detect state
						state = s_wait;
					}
					else {
						state = s_error;
					}
			}
			break;

		case s_error:
			// set error flag
			error = true;
			// clear initialized flag
			initialized = false;
			// set first read flag
			first_read = true;
			// get current tickcnt
			tickcnt = get_tickcnt();
			// goto wait and retry with detect state
			state = s_wait;
			break;

		case s_wait:

			// wait polling_ms timeout
			if (is_tickcnt_elapsed(tickcnt, polling_ms)) {
				state = s_detect;
			}
			break;
	}
}

double BH1750::get_illuminance(void) {
	return illuminance;
}
