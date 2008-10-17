## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_builddir) -I$(top_srcdir) \
	-DPACKAGE_LIBEXECDIR=\""$(libexecdir)"\" \
	-DPACKAGE_SYSCONFDIR=\""$(sysconfdir)"\" \
	-DPACKAGE_DATADIR=\""$(datadir)"\" \
	-DPACKAGE_BINDIR=\""$(bindir)"\" \
	-DPACKAGE_LOCALSTATEDIR=\""$(localstatedir)"\" \
	-DPACKAGE_LOCALEDIR=\""$(localedir)"\" \
	-DPACKAGE_LIBDIR=\""$(libdir)"\" \
	-D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT	\
	-DPOLKIT_GNOME_COMPILATION

lib_LTLIBRARIES=libpolkit-gnome.la

libpolkit_gnomeincludedir=$(includedir)/PolicyKit/polkit-gnome

libpolkit_gnomeinclude_HEADERS =              				\
	polkit-gnome.h							\
	polkit-gnome-context.h						\
	polkit-gnome-action.h						\
	polkit-gnome-toggle-action.h					\
	polkit-gnome-auth.h

libpolkit_gnome_la_SOURCES =                                		\
	polkit-gnome-context.h		polkit-gnome-context.c		\
	polkit-gnome-action.h		polkit-gnome-action.c		\
	polkit-gnome-toggle-action.h	polkit-gnome-toggle-action.c	\
	polkit-gnome-auth.h		polkit-gnome-auth.c

libpolkit_gnome_la_CFLAGS = 	\
	$(GTK_CFLAGS)		\
	$(DBUS_GLIB_CFLAGS)	\
	$(POLKIT_DBUS_CFLAGS)	\
	$(POLKIT_GRANT_CFLAGS)	\
	$(WARN_CFLAGS)		\
	$(AM_CFLAGS)

libpolkit_gnome_la_LIBADD = 	\
	$(GTK_LIBS)		\
	$(DBUS_GLIB_LIBS)	\
	$(POLKIT_DBUS_LIBS)	\
	$(POLKIT_GRANT_LIBS)	\
	$(INTLLIBS)

libpolkit_gnome_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

clean-local :
	rm -f *~ $(BUILT_SOURCES)

