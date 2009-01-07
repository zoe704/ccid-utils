/*
 * This file is part of ccid-utils
 * Copyright (c) 2008 Gianni Tedesco <gianni@scaramanga.co.uk>
 * Released under the terms of the GNU GPL version 3
*/

#include <ccid.h>

#include <stdio.h>

static int found_cci(struct usb_device *dev, int c, int i, int a)
{
	chipcard_t cc;
	cci_t cci;
	int ret = 0;

	cci = cci_probe(dev, c, i, a);
	if ( NULL == cci )
		goto out;
	
	cc = cci_get_slot(cci, 0);
	if ( NULL == cc ) {
		fprintf(stderr, "ccid: error: no slots on CCI\n");
		goto out_close;
	}

	printf("\nWAIT FOR CHIP CARD\n");
	chipcard_wait_for_card(cc);
	printf("\nPOWER ON SLOT\n");
	chipcard_slot_on(cc, CHIPCARD_AUTO_VOLTAGE);
	printf("\nSLOT STATUS\n");
	chipcard_slot_status(cc);
	printf("\nSELECT TELECOM DIR\n");
	chipcard_transmit(cc, "\xa0\xcd\x7f\x10", 4);
	printf("\nPOWER OFF SLOT\n");
	chipcard_slot_off(cc);
	printf("\nSLOT STATUS\n");
	chipcard_slot_status(cc);

	ret = 1;

out_close:
	cci_close(cci);
out:
	return ret;
}

static int check_interface(struct usb_device *dev, int c, int i)
{
	struct usb_interface *iface = &dev->config[c].interface[i];
	int a;

	for (a = 0; a < iface->num_altsetting; a++) {
		struct usb_interface_descriptor *id = &iface->altsetting[a];
		if ( id->bInterfaceClass == 0x0b ) {
			if ( found_cci(dev, c, i, a) )
				continue;
				//return 1;
		}
	}

	return 0;
}

static int check_device(struct usb_device *dev)
{
	int c, i;

	if ( dev->descriptor.idVendor == 0x4e6 &&
		dev->descriptor.idProduct == 0xe003 ) {
		return found_cci(dev, 1, 0, 0);
	}

	for(c = 0; c < dev->descriptor.bNumConfigurations; c++) {
		for(i = 0; i < dev->config[c].bNumInterfaces; i++) {
			if ( check_interface(dev, c, i) )
				return 1;
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	struct usb_bus *bus, *busses;
	struct usb_device *dev;
	int ret = EXIT_FAILURE;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	busses = usb_get_busses();

	for(bus = busses; bus; bus = bus->next) {
		for(dev = bus->devices; dev; dev = dev->next)
			if ( check_device(dev) )
				ret = EXIT_SUCCESS;
	}

	return ret;
}
