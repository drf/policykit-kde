schema_in_files =  polkit-gnome.schemas.in
schemadir   = $(GCONF_SCHEMA_FILE_DIR)
schema_DATA = $(schema_in_files:.schemas.in=.schemas)

@INTLTOOL_SCHEMAS_RULE@

# don't do this if we are building in eg. rpm
if GCONF_SCHEMAS_INSTALL
install-data-local:
	if test -z "$(DESTDIR)" ; then \
		for p in $(schema_DATA) ; do \
			GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $$p; \
		done \
	fi
endif

@INTLTOOL_DESKTOP_RULE@
desktopdir = $(datadir)/applications
desktop_in_files = polkit-gnome-authorization.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

servicedir       = $(datadir)/dbus-1/services
service_in_files = org.gnome.PolicyKit.service.in                                  \
		   org.gnome.PolicyKit.AuthorizationManager.service.in             \
		   gnome-org.freedesktop.PolicyKit.AuthenticationAgent.service.in
service_DATA     = $(service_in_files:.service.in=.service)

org.gnome.PolicyKit.service : org.gnome.PolicyKit.service.in Makefile
	@sed -e "s|\@libexecdir\@|$(libexecdir)|" $< > $@

gnome-org.freedesktop.PolicyKit.AuthenticationAgent.service : gnome-org.freedesktop.PolicyKit.AuthenticationAgent.service.in Makefile
	@sed -e "s|\@libexecdir\@|$(libexecdir)|" $< > $@

org.gnome.PolicyKit.AuthorizationManager.service : org.gnome.PolicyKit.AuthorizationManager.service.in Makefile
	@sed -e "s|\@bindir\@|$(bindir)|" $< > $@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = polkit-gnome.pc

DISTCLEANFILES = $(service_DATA) $(desktop_DATA) $(pkgconfig_DATA)

EXTRA_DIST =			\
	$(service_in_files)	\
	$(desktop_in_files)	\
	$(schema_in_files)	\
	polkit-gnome.pc.in

clean-local :
	rm -f *~ $(service_DATA) $(pkgconfig_DATA) $(schema_DATA)
