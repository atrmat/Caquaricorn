srcdir=/root/cx_src/src
csvdir=/tmp/test
datadir=/tmp/datatmp
resultdir=/tmp/resulttmp
if [ -d "$datadir" ]
then
	rm -rf $datadir
#	mkdir $datadir
fi
if [ -d "$resultdir" ]
then
	rm -rf $resultdir
fi
mkdir $resultdir
cd $csvdir
echo "run prepare"
$srcdir/prepare $csvdir/googlebooks-eng-all-5gram-20090715- 101 $datadir $resultdir/ino.txt $resultdir/lno.txt $resultdir/cdo.txt
echo "get sample"
$srcdir/get_sample $datadir
echo "jbc_build"
$srcdir/jbc_build $datadir /tmp/btree.bin $resultdir/jbt.txt
echo "jbc_build blooms"
rm -rf $datadir/jbc_dabloom.bin
$srcdir/jbc_build_blooms $datadir /tmp/btree_blooms.bin $resultdir/jbt_blooms.txt
echo "jbc index"
$srcdir/jbc_index /tmp/btree.bin n $datadir/index_sample.txt $resultdir/jbc_index_out.txt $resultdir/jit.txt
echo "jbc update"
$srcdir/jbc_update $datadir/update_sample.txt /tmp/btree.bin /tmp/modified_btree.bin $resultdir/jut.txt
