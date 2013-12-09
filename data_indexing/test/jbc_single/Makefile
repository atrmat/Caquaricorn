SRCDIR = /root/cx_src/src
DISTDIR = /tmp/datatmp
RESULTDIR = /tmp/resulttmp
CSVDIR = /tmp/test
jbc_prepare:
	${SRCDIR}/prepare ${CSVDIR}/googlebooks-eng-all-5gram-20090715- 101 ${DISTDIR} ${RESULTDIR}/ino.txt ${RESULTDIR}/lno.txt ${RESULTDIR}/cdo.txt
jbc_sample:
	cd ${CSVDIR}
	${SRCDIR}/get_sample ${DISTDIR}
jbc_build:
	${SRCDIR}/jbc_build ${DISTDIR} /tmp/btree.bin ${RESULTDIR}/jbt.txt
jbc_dablooms:
	rm -rf ${DISTDIR}/jbc_dabloom.bin
	${SRCDIR}/jbc_build_blooms ${DISTDIR} /tmp/btree_blooms.bin ${RESULTDIR}/jbt_blooms.txt
jbc_index_cache:
	${SRCDIR}/jbc_index /tmp/btree.bin y ${DISTDIR}/index_sample.txt ${RESULTDIR}/jbc_index_out.txt ${RESULTDIR}/jit.txt
jbc_update:
	${SRCDIR}/jbc_update ${DISTDIR}/update_sample.txt /tmp/btree.bin /tmp/modified_btree.bin ${RESULTDIR}/jut.txt
