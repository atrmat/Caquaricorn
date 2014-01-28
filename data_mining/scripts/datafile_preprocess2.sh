#preprocessing the data file before clustering
# the data file is preprocessed by the java program
# replace the dot with space, and split it
#!/bin/bash
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
# mahout using the space(not the ',') to split a num
cp $SRC_FILE $DST_FILE
sed -i "s/,/ /g" `grep "," -rl ./$DST_FILE`
gawk '{print NF}' $DST_FILE | sort -nu | more
rm -rf datafile;./spilt_big_file.sh -s `pwd`/$DST_FILE -p testData_ -n 10 -t csv
cd datafile/
for files in `ls`
do
    echo $files
    gawk '{print NF}' $files | sort -nu | more
done

