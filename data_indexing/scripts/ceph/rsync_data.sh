cephfs_mnt="/mnt/cephfs"
srcdir=/root/cx_src/src
datadir=/tmp/data
btreedir=/tmp/btree
resultdir=/tmp/result
sampledir=/tmp/sample

rsync -zvr $btreedir $cephfs_mnt
rsync -zvr $sampledir $cephfs_mnt
