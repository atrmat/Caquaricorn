srcdir=/root/cx_src/src
datadir=/tmp/data
btreedir=/tmp/btree
resultdir=/tmp/result_ceph
sampledir=/tmp/sample
cephfs_mnt="/mnt/cephfs"

rm -rf $resultdir/jbc_index_out_*
rm -rf $resultdir/jit_*

for i in `seq 0 1023`
do
	echo "start indexing on $i"
	$srcdir/jbc_index_N $cephfs_mnt/btree/btree$i n $sampledir/index_sample_$i $resultdir/jbc_index_out_$i $resultdir/jit_$i &
done;

echo "finish indexing"
