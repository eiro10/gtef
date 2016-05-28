@CODE_COVERAGE_RULES@

AM_CPPFLAGS =				\
	-DDATADIR=\""$(datadir)"\"	\
	-DG_LOG_DOMAIN=\"Gtef\"		\
	-DGTEF_COMPILATION		\
	-I$(top_builddir)		\
	-I$(top_srcdir)			\
	$(WARN_CFLAGS)			\
	$(CODE_COVERAGE_CPPFLAGS)	\
	$(DEP_CFLAGS)

gtef_public_headers =			\
	gtef.h				\
	gtef-buffer.h			\
	gtef-file.h			\
	gtef-info-bar.h			\
	gtef-metadata-manager.h		\
	gtef-types.h			\
	gtef-tab.h			\
	gtef-view.h

gtef_public_c_files =			\
	gtef-buffer.c			\
	gtef-file.c			\
	gtef-info-bar.c			\
	gtef-metadata-manager.c		\
	gtef-tab.c			\
	gtef-view.c

gtef_private_headers =			\
	gtef-io-error-info-bar.h	\
	gtef-progress-info-bar.h	\
	gtef-settings.h			\
	gtef-utils.h

gtef_private_c_files =			\
	gtef-init.c			\
	gtef-io-error-info-bar.c	\
	gtef-progress-info-bar.c	\
	gtef-settings.c			\
	gtef-utils.c

# Helper Libtool library, so that the private functions can be used in unit
# tests.
noinst_LTLIBRARIES = libgtef-core.la

libgtef_core_la_SOURCES =		\
	$(gtef_private_c_files)		\
	$(gtef_private_headers)		\
	$(gtef_public_c_files)		\
	$(gtef_public_headers)

libgtef_core_la_CFLAGS =		\
	$(CODE_COVERAGE_CFLAGS)

libgtef_core_la_LDFLAGS =		\
	-no-undefined			\
	$(WARN_LDFLAGS)			\
	$(CODE_COVERAGE_LDFLAGS)

# The real library.
lib_LTLIBRARIES = libgtef-@GTEF_API_VERSION@.la

libgtef_@GTEF_API_VERSION@_la_SOURCES =

libgtef_@GTEF_API_VERSION@_la_LIBADD =	\
	libgtef-core.la			\
	$(DEP_LIBS)

libgtef_@GTEF_API_VERSION@_la_CFLAGS =	\
	$(CODE_COVERAGE_CFLAGS)

libgtef_@GTEF_API_VERSION@_la_LDFLAGS =		\
	-version-info $(GTEF_LT_VERSION)	\
	-no-undefined				\
	-export-symbols-regex "^gtef_.*"	\
	$(WARN_LDFLAGS)				\
	$(CODE_COVERAGE_LDFLAGS)

libgtef_includedir = $(includedir)/gtef-@GTEF_API_VERSION@/gtef
libgtef_include_HEADERS = $(gtef_public_headers)

CLEANFILES =

if HAVE_INTROSPECTION
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS = Gtef-@GTEF_API_VERSION@.gir

INTROSPECTION_SCANNER_ENV = CC="$(CC)"

Gtef-@GTEF_API_VERSION@.gir: libgtef-@GTEF_API_VERSION@.la
INTROSPECTION_SCANNER_ARGS =		\
	-I$(top_srcdir)			\
	-I$(top_builddir)		\
	--c-include=gtef/gtef.h		\
	--warn-all
Gtef_@GTEF_API_VERSION@_gir_NAMESPACE = Gtef
Gtef_@GTEF_API_VERSION@_gir_VERSION = @GTEF_API_VERSION@
Gtef_@GTEF_API_VERSION@_gir_CFLAGS = $(DEP_CFLAGS)
Gtef_@GTEF_API_VERSION@_gir_SCANNERFLAGS = $(WARN_SCANNERFLAGS)
Gtef_@GTEF_API_VERSION@_gir_LIBS = libgtef-@GTEF_API_VERSION@.la
Gtef_@GTEF_API_VERSION@_gir_FILES =	\
	$(gtef_public_headers)		\
	$(gtef_public_c_files)

Gtef_@GTEF_API_VERSION@_gir_INCLUDES = Gtk-3.0 GtkSource-3.0
Gtef_@GTEF_API_VERSION@_gir_PACKAGES = gtk+-3.0 gtksourceview-3.0
Gtef_@GTEF_API_VERSION@_gir_EXPORT_PACKAGES = gtef-@GTEF_API_VERSION@

girdir = $(datadir)/gir-1.0
gir_DATA = Gtef-@GTEF_API_VERSION@.gir

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = Gtef-@GTEF_API_VERSION@.typelib

CLEANFILES +=		\
	$(gir_DATA)	\
	$(typelib_DATA)

endif # HAVE_INTROSPECTION

if ENABLE_VAPIGEN
-include $(VAPIGEN_MAKEFILE)

gtef-@GTEF_API_VERSION@.vapi: Gtef-@GTEF_API_VERSION@.gir

VAPIGEN_VAPIS = gtef-@GTEF_API_VERSION@.vapi

gtef_@GTEF_API_VERSION@_vapi_DEPS = gtk+-3.0 gtksourceview-3.0

gtef-@GTEF_API_VERSION@.deps:
	$(AM_V_GEN) for pkg in $(gtef_@GTEF_API_VERSION@_vapi_DEPS); do \
		echo $$pkg >> $@; \
	done

gtef_@GTEF_API_VERSION@_vapi_METADATADIRS = $(srcdir)
gtef_@GTEF_API_VERSION@_vapi_FILES = Gtef-@GTEF_API_VERSION@.gir

vapidir = $(datadir)/vala/vapi
vapi_DATA =				\
	gtef-@GTEF_API_VERSION@.vapi	\
	gtef-@GTEF_API_VERSION@.deps

CLEANFILES += $(vapi_DATA)

endif # ENABLE_VAPIGEN

-include $(top_srcdir)/git.mk
