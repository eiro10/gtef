## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = 1.6

# The name of the module.
# For library-web, it's simpler if the DOC_MODULE follows the scheme:
# $name-$major.$minor
# (to show the API online at developer.gnome.org)
# So, add ".0".
DOC_MODULE = gtef-@GTEF_API_VERSION@.0

# The top-level XML file (SGML in the past).
DOC_MAIN_SGML_FILE = gtef-docs.xml

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS = --rebuild-types

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR = $(top_srcdir)/gtef

# Used for dependencies
HFILE_GLOB = $(top_srcdir)/gtef/*.h
CFILE_GLOB = $(top_srcdir)/gtef/*.c

# Header files or dirs to ignore when scanning. Use base file/dir names
# e.g. IGNORE_HFILES=gtkdebug.h gtkintl.h private_code
IGNORE_HFILES =				\
	gtef.h				\
	gtef-io-error-info-bar.h	\
	gtef-progress-info-bar.h	\
	gtef-settings.h			\
	gtef-utils.h

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS = --xml-mode --output-format=xml

# Images to copy into HTML directory
HTML_IMAGES =

# Extra XML files that are included by $(DOC_MAIN_SGML_FILE)
content_files =

# Extra options to supply to gtkdoc-fixref
FIXXREF_OPTIONS =

# CFLAGS and LDFLAGS for compiling gtkdoc-scangobj with your library.
# Only needed if you are using gtkdoc-scangobj to dynamically query widget
# signals and properties.
GTKDOC_CFLAGS = -I$(top_srcdir) $(DEP_CFLAGS)
GTKDOC_LIBS =							\
	$(top_builddir)/gtef/libgtef-@GTEF_API_VERSION@.la	\
	$(DEP_LIBS)

MAINTAINERCLEANFILES =			\
	$(DOC_MODULE)-overrides.txt	\
	$(DOC_MODULE).types

# include common portion ...
include $(top_srcdir)/gtk-doc.make

EXTRA_DIST += gtef-docs.xml.in
DISTCLEANFILES = gtef-docs.xml

-include $(top_srcdir)/git.mk
