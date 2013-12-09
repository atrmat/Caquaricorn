srcdir=/root/cx_src/src
datadir=/tmp/data
btreedir=/tmp/btree
resultdir=/tmp/result


for i in `seq 0 1023`
do
	echo "start update on btree $i"
	$srcdir/jbc_update_N $datadir/update_sample_$i $btreedir/btree$i $btreedir/modified_btree $resultdir/jut $i
done;

