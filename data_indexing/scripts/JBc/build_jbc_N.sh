srcdir=/root/cx_src/src
datadir=/tmp/data
btreedir=/tmp/btree
resultdir=/tmp/result

if [ ! -d "$datadir" ]
then
	echo "fail due to no temp data dir exist!" >> $resultdir/JBc_build_error.txt
	return -1
fi
if [ ! -d "$resultdir"  ]
then
	echo "no result dir exist, mkdir one" >> $resultdir/JBc_build_error.txt
	mkdir $resultdir
fi
if [ ! -d "$btreedir" ]
then
	echo "no btree dir exisit, mkdir one" >> $resultdir/JBc_build_error.txt
	mkdir $btreedir
fi

for i in `seq 0 1023`
do
	if [ ! -f "$datadir/temp_$i"".txt"  ]
	then
		echo "no $datadir/temp_$i"".txt exist!" >> $resultdir/JBc_build_error.txt
		continue;
	else
		$srcdir/jbc_build_N $datadir/ $btreedir/btree $resultdir/jbt $i
	fi
done;

