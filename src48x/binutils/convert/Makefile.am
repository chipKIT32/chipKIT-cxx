## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = cygnus
ACLOCAL_AMFLAGS = -I . -I .. -I ../config

noinst_PROGRAMS = conv

conv_SOURCES =  convert.c elfio.c elfout.c mipsflash.c \
		srec.c lsifast.c idtbin.c stagbin.c bin.c \
		elf.h elfmips.h sysdep.h conv-version.h \
		conv.1

#EXTRA_conv_SOURCES = getopt.h # alloca.c getopt.c strtoul.c

conv-version.h:	$(srcdir)/version.h $(srcdir)/Makefile.in
	@echo "creating $@"
	conv_version_package="\"$(PKGVERSION)\"" ;\
	report_bugs_to="\"$(REPORT_BUGS_TO)\"" ;\
	sed -e "s,@conv_version_package@,$$conv_version_package," \
	    -e "s,@report_bugs_to@,$$report_bugs_to," \
	    < $(srcdir)/version.h > $@

convert.o: convert.c conv-version.h

LIBIBERTY = ../libiberty/libiberty.a

conv_LDADD = $(LIBIBERTY)

man_MANS = conv.1

info_TEXINFOS = conv.texi

conv_TEXI = $(srcdir)/conv.texi

# FIXME!
conv.html: $(conv_TEXI)
#	@rm -f conv.html
#	texi2html -monolithic -glossary -menu -I . -I $(srcdir) $(conv_TEXI)
	touch $@

conv.dvi:  $(conv_TEXI)

conv.info: $(conv_TEXI)

.PHONY: install-exec-local install-html

install-exec-local: $(noinst_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(noinst_PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    t="$(DESTDIR)$(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`"; \
	    echo "  $(INSTALL_PROGRAM) $$p $$t"; \
	     $(INSTALL_PROGRAM) $$p $$t; \
	  else :; fi; \
	done

install-html:
	echo FIXME > /dev/null

install-pdf:
	echo FIXME > /dev/null
