#preprocessing the data file before clustering
#generate the new data file and replaced the dot with space in csv files
SRC_FILE=
DST_FILE=
usage()
{
    echo "Usage: `basename $0` [-s] src_file [-d] dst_file"
    exit 1
}

[ $# -eq 0 ] && usage

while getopts :s:d: OPTION
do
    case $OPTION in
        s)
            SRC_FILE=$OPTARG
            ;;
        d)
            DST_FILE=$OPTARG
            ;;
        \?)
            usage
            ;;
    esac
done

shift $(($OPTIND - 1))

if [ -z "$SRC_FILE" ]; then
    echo "You must specify SRC_FILE with -s option"
    exit
fi
if [ -z "$DST_FILE" ]; then
    echo "You must specify DST_FILE with -d option"
    exit
fi
#awk 'BEGIN{FS="," ; OFS="," }{print 1,3}' $SRC_FILE > $DST_FILE
#gawk -F"," '{print $1","$2","$3","$8","$9","$10","$11","$16","$17","$18","$19","$20","$21","$22","$23","$24","$25","$26","$31","$33","$34","$35",";if('$36' == 'CMNET')print 0; else if('36' == 'CMWAP') print 1; else print $36; print ","$38","$39}' $SRC_FILE > $DST_FILE
gawk -F"," '{if(NR!=1) print $1","$2","$3","$8","$9","$10","$11","$16","$17","$18","$19","$20","$21","$22","$23","$24","$25","$26","$31","$33","$34","$35","$36","$38","$39}' $SRC_FILE > $DST_FILE
#gawk -F"," '{print $1","$2","$3","$6","$7","$8","$9","$10","$11","$14","$15","$16","$17","$18","$19","$20","$21","$22","$23","$24","$25","$26","$31","$32","$33","$34","$35","$36","$38","$39}' $SRC_FILE > $DST_FILE
sed -i "s/null/0/g" `grep "null" -rl ./$DST_FILE`
sed -i "s/CMNET/1/g" `grep "CMNET" -rl ./$DST_FILE`
sed -i "s/cmnet/1/g" `grep "cmnet" -rl ./$DST_FILE`
sed -i "s/CMWAP/2/g" `grep "CMWAP" -rl ./$DST_FILE`
sed -i "s/cmwap/2/g" `grep "cmwap" -rl ./$DST_FILE`

gawk '{print NF}' $DST_FILE |sort -nu|more
sed -i "s/,/ /g" `grep "," -rl ./$DST_FILE`
gawk '{print NF}' $DST_FILE | sort -nu | more
