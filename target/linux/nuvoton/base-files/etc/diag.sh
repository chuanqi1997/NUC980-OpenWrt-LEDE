#!/bin/sh
# Copyright (C) 2010-2013 OpenWrt.org

. /lib/functions/leds.sh
. /lib/nuc980.sh

get_status_led() {
	board=$(nuc980_board_name)

	case $board in

	nuc980-iot-gateway|\
	nuc980-iot-cm100|\
	nuc980-iot-psm100)
		status_led="nuc980:green:power"
		;;

	esac
}

set_state() {
	get_status_led $1

	case "$1" in
	preinit)
		status_led_blink_preinit
		;;
	failsafe)
		status_led_blink_failsafe
		;;
	upgrade | \
	preinit_regular)
		status_led_blink_preinit_regular
		;;
	done)
		status_led_on
		;;
	esac
}
