#!/bin/bash 
# split big data file into small files
SPLIT_FILE=
FILES_PREFIX=
FILES_NUM=

usage()
{
    echo "Usage: `basename $0` [-s] split_file [-p] files_prefix [-n] files_num, default files num is 10"
    exit 1
}

[ $# -eq 0 ] && usage

while getopts :s:p:n: OPTION
do
    case $OPTION in
        s)
            SPLIT_FILE=$OPTARG
	    ;;
        p)
            FILES_PREFIX=$OPTARG
            ;;
	n)
	    FILES_NUM=$OPTARG
	    ;;
        \?)               
            usage
            ;;
    esac
done

shift $(($OPTIND - 1))

if [ -z "$SPLIT_FILE" ]; then  
    echo "You must specify SPILT_FILE with -s option"
    exit
fi
if [ -z "$FILES_PREFIX" ]; then  
    echo "You must specify FILES_PREFIX with -p option"
    exit
fi
if [ -z "$FILES_NUM" ]; then  
    echo "No specify FILES_NUM with -n option"
    echo "Set default files num: 10"
    FILES_NUM=10
fi

echo "you chose the following options.."
echo "SPILT_FILE=$SPLIT_FILE, FILES_PREFIX=$FILES_PREFIX, FILES_NUM=$FILES_NUM"
LINE_NUM=`wc -l $SPLIT_FILE | gawk -F " " '{print $1}'`
echo $LINE_NUM
LINE_NUM=`expr $LINE_NUM / $FILES_NUM`
echo $LINE_NUM
split -l $LINE_NUM $SPLIT_FILE $FILES_PREFIX
