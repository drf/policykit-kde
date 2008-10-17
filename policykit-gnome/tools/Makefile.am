
bin_PROGRAMS = polkit-gnome-authorization

polkit_gnome_authorization_SOURCES = 						\
						polkit-gnome-authorization.c	\
	sexy-url-label.h 			sexy-url-label.c

polkit_gnome_authorization_CPPFLAGS = 			\
	-I$(top_srcdir)					\
	-DLIBEXECDIR=\""$(libexecdir)"\"		\
	-DDATADIR=\""$(pkgdatadir)"\"			\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\"	\
	$(DISABLE_DEPRECATED)				\
	$(AM_CPPFLAGS)

polkit_gnome_authorization_CFLAGS = 			\
	$(GTK_CFLAGS)					\
	$(GCONF_CFLAGS)					\
	$(DBUS_GLIB_CFLAGS)				\
	$(POLKIT_DBUS_CFLAGS)				\
	$(POLKIT_GRANT_CFLAGS)				\
	$(WARN_CFLAGS)					\
	$(AM_CFLAGS)

polkit_gnome_authorization_LDFLAGS = 			\
	$(AM_LDFLAGS)

polkit_gnome_authorization_LDADD = 			\
	$(GTK_LIBS)					\
	$(GCONF_LIBS)					\
	$(DBUS_GLIB_LIBS)				\
	$(POLKIT_DBUS_LIBS)				\
	$(POLKIT_GRANT_LIBS)				\
	$(INTLLIBS)					\
	$(top_builddir)/polkit-gnome/libpolkit-gnome.la

BUILT_SOURCES =						\
	single-instance-glue.h

single-instance-glue.h: single-instance.xml Makefile.am
	dbus-binding-tool --prefix=single_instance --mode=glib-server --output=single-instance-glue.h $(top_srcdir)/tools/single-instance.xml

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = single-instance.xml

clean-local :
	rm -f *~
