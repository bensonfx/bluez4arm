define extract_pkg
    $(Q)mod=$(addprefix $(PKG_DIR)/,$(1)); \
	if [ ! -d $${mod} ];then \
		tar xf $${mod}.* -C $(SRC_DIR); \
	fi
endef

define build_mod
	$(call extract_pkg,$($(1)))
	$(Q)pushd $(addprefix $(SRC_DIR)/,$($(1))); \
	$($(addsuffix _PREFLAG,$(@))) ./configure $($(addsuffix _PARAMS,$(1))); \
	make $($(addsuffix _MKFLAG,$(1))) -j || exit $$?; \
	$($(addsuffix _INSTFLAG,$(1))) make install || exit $$?; \
	popd
endef

Q                   := @
ifeq ($(V),1)
Q                   :=
endif

product				:= hi3559
-include product.mk

TOP					?= $(shell pwd)
PKG_DIR				:= $(TOP)/pkg
SRC_DIR				:= $(TOP)/src
PREFIX				:= /data
DEPS_DESTDIR		:= $(TOP)/deps
BLUEZ_DESTDIR		:= $(TOP)/bluez
lite				:= $(TOP)/out

BLUEZ_DEPS_LIBS		:= libexpat.so.1 libreadline.so.7 libglib-2.0.so.0 libpcre.so.1
BLUEZ_PROFILE		:= src/main.conf profiles/network/network.conf profiles/input/input.conf
BLUEZ_PROFILE_DIR	:= /etc/bluetooth
BLUEZ_PROFILE_DIST  := $(BLUEZ_DESTDIR)$(PREFIX)$(BLUEZ_PROFILE_DIR)

DBUS_LIB 			:= libdbus-1.so.3
DBUS_CONF 			:= /data/etc
DBUS_DESTDIR 		:= $(BLUEZ_DESTDIR)

###lite target###
LITE_BIN			:= l2ping bluetoothctl btmon dbus-daemon dbus-cleanup-sockets dbus-monitor dbus-launch
LITE_BIN			:= $(addprefix bin/,$(LITE_BIN)) libexec/bluetooth/bluetoothd

LITE_LIB			:= $(addprefix lib/,$(BLUEZ_DEPS_LIBS)) $(DBUS_DESTDIR)$(PREFIX)/lib/$(DBUS_LIB)
LITE_CONF			:= etc/dbus-1 share/dbus-1
LITE_PROFILE_DIST  := $(lite)$(BLUEZ_PROFILE_DIR)
#################

ifeq ($(product),hi3559)
    CROSS_COMPILE	?= arm-hisiv600-linux
    CC				:= $(CROSS_COMPILE)-gcc
    CXX				:= $(CROSS_COMPILE)-g++
    HOST			:= --host=$(CROSS_COMPILE)
endif
EXTRA_FLAGS			:= $(HOST) CFLAGS="-I$(DEPS_DESTDIR)/include" LDFLAGS="-L$(DEPS_DESTDIR)/lib -L$(DEPS_DESTDIR)$(PREFIX)/lib -L$(BLUEZ_DESTDIR)$(PREFIX)/lib"
