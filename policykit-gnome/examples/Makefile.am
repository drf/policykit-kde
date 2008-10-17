
bin_PROGRAMS = polkit-gnome-example

polkit_gnome_example_SOURCES = 				\
	polkit-gnome-example.c

polkit_gnome_example_CPPFLAGS = 			\
	-I$(top_srcdir)					\
	-DLIBEXECDIR=\""$(libexecdir)"\"		\
	-DDATADIR=\""$(pkgdatadir)"\"			\
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\"	\
	$(DISABLE_DEPRECATED)				\
	$(AM_CPPFLAGS)

polkit_gnome_example_CFLAGS = 			\
	$(GTK_CFLAGS)				\
	$(DBUS_GLIB_CFLAGS)			\
	$(POLKIT_DBUS_CFLAGS)			\
	$(WARN_CFLAGS)				\
	$(AM_CFLAGS)

polkit_gnome_example_LDFLAGS = 			\
	$(AM_LDFLAGS)

polkit_gnome_example_LDADD = 				\
	$(GTK_LIBS)					\
	$(DBUS_GLIB_LIBS)				\
	$(POLKIT_DBUS_LIBS)				\
	$(POLKIT_GRANT_LIBS)				\
	$(INTLLIBS)					\
	$(top_builddir)/polkit-gnome/libpolkit-gnome.la

# PolicyKit action definitions for this example
#
polkit_policydir = $(datadir)/PolicyKit/policy
dist_polkit_policy_DATA = org.gnome.policykit.examples.policy

# You will need a recent intltool or the patch from this bug
# http://bugzilla.gnome.org/show_bug.cgi?id=462312
@INTLTOOL_POLICY_RULE@

check:
	$(POLKIT_POLICY_FILE_VALIDATE) $(top_srcdir)/examples/$(dist_polkit_policy_DATA)

EXTRA_DIST = org.gnome.policykit.examples.policy.in

CLEANFILES = $(BUILT_SOURCES)
DISTCLEANFILES = org.gnome.policykit.examples.policy

clean-local :
	rm -f *~
