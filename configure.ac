dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.64)

# Package version of the form 'major.minor.micro'.
m4_define(gtef_package_version, 0.1.1)

AC_INIT([gtef],
	[gtef_package_version],
	[https://bugzilla.gnome.org/enter_bug.cgi?product=gtef],
	[gtef],
	[https://wiki.gnome.org/Projects/gtef])

AX_REQUIRE_DEFINED([AX_IS_RELEASE])
AX_IS_RELEASE([git-directory])

# Libtool versioning
#
# For development releases (if the minor package version is odd), keep the same
# Libtool version.
#
# For a new minor stable release (when incrementing the minor package version
# to an even number), apply the following algorithm step by step:
# 1. If the library source code has changed at all since the last
#    update, then increment REVISION.
# 2. If any exported functions or data have been added, removed, or
#    changed since the last update, increment CURRENT and set REVISION
#    to 0.
# 3. If any exported functions or data have been added since the last
#    public release, increment AGE.
# 4. If any exported functions or data have been removed since the last
#    public release, set AGE to 0.
#
# When incrementing the API version (usually for a new major package version),
# set CURRENT, REVISION and AGE to 0 since it's like a new library.

GTEF_LT_CURRENT=0
GTEF_LT_REVISION=0
GTEF_LT_AGE=0
GTEF_LT_VERSION="$GTEF_LT_CURRENT:$GTEF_LT_REVISION:$GTEF_LT_AGE"
AC_SUBST(GTEF_LT_VERSION)

# API version, used for parallel installability.
# Not used in:
# - po/Makevars
# - the docs/reference/gtef-1.0-sections.txt filename
# Because as far as I've tested, it is not easily feasible to not hardcode the
# API version in those places (swilmet).
GTEF_API_VERSION=1
AC_SUBST(GTEF_API_VERSION)

# Dependencies
glib_req=2.44
gtk_req=3.20
gtksourceview_req=3.20
libxml_req=2.5

AC_CONFIG_SRCDIR([gtef/gtef.h])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

# gnu strictness to generate the INSTALL file
AM_INIT_AUTOMAKE([1.11 tar-ustar dist-xz no-dist-gzip gnu -Wno-portability])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Check for programs
AC_PROG_CC
AC_PROG_INSTALL

AX_REQUIRE_DEFINED([PKG_PROG_PKG_CONFIG])
AX_REQUIRE_DEFINED([PKG_INSTALLDIR])
PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

# Initialize libtool
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

# Compile warnings
AX_REQUIRE_DEFINED([AX_COMPILER_FLAGS])
AX_COMPILER_FLAGS([WARN_CFLAGS], [WARN_LDFLAGS])

# Dynamic linking dependencies
AX_REQUIRE_DEFINED([AX_PKG_CHECK_MODULES])
AX_PKG_CHECK_MODULES([DEP],
		     [glib-2.0 >= $glib_req  gtk+-3.0 >= $gtk_req  gtksourceview-3.0 >= $gtksourceview_req],
		     [libxml-2.0 >= $libxml_req])

# Use GVfs metadata or the old XML file store.
AC_ARG_ENABLE([gvfs-metadata],
	      AS_HELP_STRING([--disable-gvfs-metadata], [Disable using GVfs to store metadata]),
	      [enable_gvfs_metadata=$enableval],
	      [enable_gvfs_metadata=yes])

if test "x$enable_gvfs_metadata" = "xyes"; then
	AC_DEFINE([ENABLE_GVFS_METADATA], [1], [Define to enable using GVfs metadata])
fi

# i18n
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.4])
AC_SUBST([GETTEXT_PACKAGE], [gtef-$GTEF_API_VERSION])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Define to the gettext package name.])

# Documentation
AX_REQUIRE_DEFINED([GTK_DOC_CHECK])
GTK_DOC_CHECK([1.25], [--flavour=no-tmpl])

# Installed Tests for gnome continuous integration
AC_ARG_ENABLE(installed_tests,
	      AS_HELP_STRING([--enable-installed-tests],
			     [Install test programs (default: no)]),,
	      [enable_installed_tests=no])

AM_CONDITIONAL(INSTALLED_TESTS, test "x$enable_installed_tests" = "xyes")

# Introspection
AX_REQUIRE_DEFINED([GOBJECT_INTROSPECTION_CHECK])
GOBJECT_INTROSPECTION_CHECK([1.42.0])

# Vala
AX_REQUIRE_DEFINED([VAPIGEN_CHECK])
VAPIGEN_CHECK

# Code coverage for unit tests
AX_REQUIRE_DEFINED([AX_CODE_COVERAGE])
AX_CODE_COVERAGE()

# GSettings
AX_REQUIRE_DEFINED([GLIB_GSETTINGS])
GLIB_GSETTINGS

# Output files
AC_CONFIG_FILES([
	gtef-${GTEF_API_VERSION}.pc:gtef.pc.in
	Makefile
	data/Makefile
	data/org.gnome.gtef.gschema.xml
	docs/Makefile
	docs/reference/gtef-docs.xml
	docs/reference/Makefile
	gtef/Makefile
	po/Makefile.in
	tests/Makefile
	testsuite/Makefile
])

AC_OUTPUT

echo "
Configuration:

	Source code location:	${srcdir}
	Compiler:		${CC}
	GVfs metadata:		${enable_gvfs_metadata}
	Documentation:		${enable_gtk_doc}
	GObject introspection:	${found_introspection}
	Vala:			${enable_vala}
	Code coverage:		${enable_code_coverage}
	Installed tests:	${enable_installed_tests}
"
