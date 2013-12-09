srcdir=/root/cx_src/src
datadir=/tmp/data
btreedir=/tmp/btree
resultdir=/tmp/result
sampledir=/tmp/sample

if [ ! -d "$sampledir" ]
then
	mkdir $sampledir
else
	rm -rf $sampledir/*
fi
rm -rf "$datadir"/index_sample_*
rm -rf "$datadir"/update_sample_*

for i in `seq 0 1023`
do
echo "sample $i"
	"$srcdir"/get_sample_N $datadir $i
done;

mv "$datadir"/index_sample_* "$sampledir"/
mv "$datadir"/update_sample_* "$sampledir"/


