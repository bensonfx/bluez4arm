all: prepare GLIB DBUS BLUEZ lite
	@echo -e "\e[0;36;1m->make $(BLUEZ) done!\e[0m"

prepare:
	$(Q)install -d $(DEPS_DESTDIR) $(SRC_DIR)
	$(Q)[ -d $(BLUEZ_DESTDIR) ] || mkdir -p $(BLUEZ_DESTDIR)
	$(Q)dir=$(SRC_DIR)/$(GLIB);install -d $$dir;cp $(PKG_DIR)/$(GLIB_CACHE_FILE) $${dir}/

ZLIB:
	$(Q)$(call build_mod,$@)

NCURSES:
	$(Q)$(call build_mod,$@)

READLINE: prepare NCURSES
	$(Q)$(call build_mod,$@)

PCRE: READLINE ZLIB
	$(Q)$(call build_mod,$@)

LIBFFI:
	$(Q)$(call build_mod,$@)
	$(Q)cd $(DEPS_DESTDIR);install -m 644 -Dt include/ lib/$(LIBFFI)/include/*.h;cd -

GLIB: LIBFFI PCRE
	$(Q)$(call build_mod,$@)

EXPAT:
	$(Q)$(call build_mod,$@)

DBUS: EXPAT
	$(Q)$(call build_mod,$@)

BLUEZ:
	$(Q)$(call build_mod,$@)
	$(Q)-rm -rf  $(DEPS_DESTDIR)$(PREFIX)/lib/*.old
	$(Q)cp -rfLv $(addprefix $(DEPS_DESTDIR)/lib/,$(BLUEZ_DEPS_LIBS)) $(BLUEZ_DESTDIR)$(PREFIX)/lib/
	$(Q)$(call install_profile,$(BLUEZ_PROFILE_DIST))
	$(Q)install -Dt $(BLUEZ_PROFILE_DIST) $(addprefix $(SRC_DIR)/$(BLUEZ)/,$(BLUEZ_PROFILE))
	$(Q)find $(SRC_DIR)/$($@) -name btmgmt -exec cp {} $(BLUEZ_DESTDIR)$(PREFIX)/bin/ \;

.PHONY: clean distclean lite
lite:
	$(Q)cd $(BLUEZ_DESTDIR)$(PREFIX); \
		install -Dt $($@)/lib $(LITE_LIB); \
		install -Dt $($@)/bin $(LITE_BIN); \
		cp -rf --parents $(LITE_CONF) $($@)
	$(Q)install -Dt $(LITE_PROFILE_DIST) $(addprefix $(SRC_DIR)/$(BLUEZ)/,$(BLUEZ_PROFILE))
	$(Q)#cp -rf $(BLUEZ_DESTDIR)/var $(lite)/

clean:
	$(Q)[ -d $(SRC_DIR) ] || exit 0
	$(Q)-for x in `find $(BUILD_DIR) -maxdepth 2 -mindepth 2 -name Makefile`;do{ \
		dir=$$(dirname $$x); \
		make -C $$dir clean; \
	}& done;wait

distclean:
	$(Q)-rm -rf $(SRC_DIR) $(DEPS_DESTDIR)
	$(Q)if [ "$(BLUEZ_DESTDIR)" != "$(TOP)" ];then \
		rm -rf $(BLUEZ_DESTDIR); \
	else rm -rf $(BLUEZ_DESTDIR)$(PREFIX); fi
	$(Q)rm -rf $(lite)
