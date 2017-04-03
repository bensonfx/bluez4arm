include build/base.mk
NCURSES				:= ncurses-6.0
NCURSES_PARAMS		:=$(EXTRA_FLAGS) CC=$(CROSS_COMPILE)-gcc CXX=$(CROSS_COMPILE)-g++ --prefix=$(DEPS_DESTDIR)

ZLIB				:= zlib-1.2.11
ZLIB_PARAMS			:= --prefix=$(DEPS_DESTDIR)
ZLIB_PREFLAG		:= CHOST=$(CROSS_COMPILE)


LIBFFI 				:= libffi-3.2.1
LIBFFI_PARAMS		:= $(EXTRA_FLAGS) --prefix=$(DEPS_DESTDIR)

GLIB				:= glib-2.52.3
GLIB_CACHE_FILE		:= arm-linux.cache
GLIB_PARAMS			:= $(EXTRA_FLAGS) PKG_CONFIG_PATH=$(DEPS_DESTDIR)/lib/pkgconfig --prefix=$(DEPS_DESTDIR) \
						--cache-file=$(GLIB_CACHE_FILE) --disable-libmount

PCRE				:= pcre-8.41
PCRE_PARAMS			:= $(EXTRA_FLAGS) --prefix=$(DEPS_DESTDIR) --enable-unicode-properties --enable-pcre16 \
						--enable-pcre32 --enable-pcregrep-libz \
						--enable-utf8 --disable-static

READLINE			:= readline-7.0
READLINE_PARAMS		:= $(EXTRA_FLAGS) --prefix=$(DEPS_DESTDIR) bash_cv_wcwidth_broken=yes
READLINE_MKFLAG		:= SHLIB_LIBS=-lncurses

EXPAT				:= expat-2.2.4
EXPAT_PARAMS		:= $(EXTRA_FLAGS) --prefix=$(DEPS_DESTDIR)

DBUS				:= dbus-1.10.24
DBUS_USER			:= root
DBUS_PARAMS			:= $(EXTRA_FLAGS) --prefix=$(PREFIX) --localstatedir=/var --with-dbus-user=$(DBUS_USER) \
						--with-x=no --enable-epoll
DBUS_INSTFLAG		:= DESTDIR=$(DBUS_DESTDIR)

BLUEZ				:= bluez-5.47
BLUEZ_PARAMS		:= $(EXTRA_FLAGS) --prefix=$(PREFIX) PKG_CONFIG_PATH=$(DEPS_DESTDIR)/lib/pkgconfig \
						--with-dbusconfdir=$(DBUS_CONF) \
						--disable-systemd --disable-udev \
						--disable-cups --disable-obex --disable-manpages \
						--disable-a2dp --disable-avrcp --disable-network \
						--enable-library --enable-experimental
BLUEZ_INSTFLAG		:= DESTDIR=$(BLUEZ_DESTDIR)

ifeq ($(product),hi3559)
    BLUEZ_PARAMS	+= --host=$(CROSS_COMPILE)
endif
