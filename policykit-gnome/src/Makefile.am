
BUILT_SOURCES =							\
	polkit-gnome-manager-glue.h

polkit-gnome-manager-glue.h: polkit-gnome-manager.xml Makefile.am
	dbus-binding-tool --prefix=polkit_gnome_manager --mode=glib-server --output=polkit-gnome-manager-glue.h polkit-gnome-manager.xml

libexec_PROGRAMS = polkit-gnome-manager

polkit_gnome_manager_SOURCES = 						\
	polkit-gnome-manager.h		polkit-gnome-manager.c		\
	polkit-gnome-auth-dialog.h	polkit-gnome-auth-dialog.c	\
	sexy-url-label.h 		sexy-url-label.c		\
	main.c								\
	$(BUILT_SOURCES)

polkit_gnome_manager_CPPFLAGS = 			\
	-I$(top_srcdir)					\
	-DG_LOG_DOMAIN=\"PolicyKitGnome\"		\
	-DDATADIR=\""$(pkgdatadir)"\"			\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\"	\
	$(DISABLE_DEPRECATED)				\
	$(AM_CPPFLAGS)

polkit_gnome_manager_CFLAGS = 				\
	$(GTK_CFLAGS)					\
	$(GCONF_CFLAGS)					\
	$(DBUS_GLIB_CFLAGS)				\
	$(POLKIT_DBUS_CFLAGS)				\
	$(POLKIT_GRANT_CFLAGS)				\
	$(WARN_CFLAGS)					\
	$(AM_CFLAGS)

polkit_gnome_manager_LDFLAGS = 				\
	$(AM_LDFLAGS)

polkit_gnome_manager_LDADD = 				\
	$(GTK_LIBS)					\
	$(GCONF_LIBS)					\
	$(DBUS_GLIB_LIBS)				\
	$(POLKIT_DBUS_LIBS)				\
	$(POLKIT_GRANT_LIBS)				\
	$(INTLLIBS)

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = polkit-gnome-manager.xml

clean-local :
	rm -f *~
