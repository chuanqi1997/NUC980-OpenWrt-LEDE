#
# Copyright (C) 2010-2015 OpenWrt.org
#
. /lib/functions/system.sh
. /lib/nuc980.sh


#platform_do_upgrade() {
#	local tar_file="$1"
#	local board="$(cat /tmp/sysinfo/board_name)"
#
#	echo "flashing kernel"
#	tar xf $tar_file sysupgrade-$board/kernel -O | mtd write - kernel
#
#	echo "flashing rootfs"
#	tar xf $tar_file sysupgrade-$board/root -O | mtd write - rootfs
#
#	return 0
#}
# more debuf message
export VERBOSE=1   
     
platform_check_image() {

	local board=$(nuc980_board_name)

	case "$board" in
	nuc980-iot-gateway|\
	nuc980-iot-cm100|\
	nuc980-iot-psm100)
		nand_do_platform_check $board $1
		return $?;
		;;
	esac

	echo "Sysupgrade is not yet supported on $board."
	return 1
}

disable_watchdog() {
	killall watchdog
	( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
		echo 'Could not disable watchdog'
		return 1
	}
}
platform_pre_upgrade() {
	local board=$(nuc980_board_name)

	case "$board" in
	nuc980-iot-gateway|\
	nuc980-iot-cm100|\
	nuc980-iot-psm100)
		nand_do_upgrade "$1"
		#nand_do_upgrade "$ARGV"
		#default_do_upgrade "$ARGV"
		;;
	esac
}


blink_led() {
	. /etc/diag.sh; set_state upgrade
}

append blink_led
#append sysupgrade_pre_upgrade disable_watchdog
#append sysupgrade_pre_upgrade blink_led
