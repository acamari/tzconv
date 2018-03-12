REGRESS_TARGETS=main

main:
	../obj/tzconv -f Z -t America/Monterrey main.in | cmp -s main.exp /dev/stdin

.include <bsd.regress.mk>
