#!/bin/sh

do_nuc980() {
	. /lib/nuc980.sh

	nuc980_board_detect
}

boot_hook_add preinit_main do_nuc980
