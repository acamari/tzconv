PROG=tzconv
BINDIR?=/usr/local/bin
MANDIR?=/usr/local/man/man
MANDOCBIN?=mandoc

.include <bsd.prog.mk>

manhtml: ${MAN}
	for m in $?; do \
		${MANDOCBIN} -T html \
			-O style=http://man.openbsd.org/mandoc.css,man=http://man.openbsd.org/%N.%S \
		$$m > $$m.html; \
	done

manmarkdown: ${MAN}
	for m in $?; do \
		${MANDOCBIN} -T markdown $$m > $$m.md; \
	done

README.md: ${MAN}.md
	cp $? ${?:H}/$@

release: manhtml manmarkdown README.md

regress:
	(cd ${.CURDIR}/regress; ${.MAKE})
