srcdir=/root/cx_src/src
datadir=/tmp/data
btreedir=/tmp/btree
resultdir=/tmp/result
sampledir=/tmp/sample

for i in `seq 0 1023`
do
	echo "start indexing on $i"
	$srcdir/jbc_index_N $btreedir/btree$i n $sampledir/index_sample_$i $resultdir/jbc_index_out_$i $resultdir/jit_$i
done;

