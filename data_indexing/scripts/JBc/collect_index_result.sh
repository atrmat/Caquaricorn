cephfs_mnt="/mnt/cephfs"
indexresult=$cephfs_mnt/indexresult.txt
#indexresult=/tmp/indexresult.txt
resultdir=/tmp/result_ceph
#resultdir=/tmp/result

if [  -f "$indexresult" ]
then
	echo "rm the old index result and make new one!"
	rm $indexresult
fi
echo -e "Index,us,seeks,keys" >>$indexresult
for i in `seq 0 1023`
do
	echo -e "$i,\c" >> $indexresult
	cat "$resultdir"/jit_"$i" | grep "index" | gawk -F " " '{gsub(/[[:blank:]]*/,"",$2);print $2","$6","$10}' >> $indexresult
done;
