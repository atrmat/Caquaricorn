#!/bin/bash 
# split big data file into small files
SPLIT_FILE=
FILES_PREFIX=
FILES_NUM=
FILES_TYPE=
usage()
{
    echo "Usage: `basename $0` [-s] split_file [-p] files_prefix [-n] files_num [-t] files_type, default files num is 10"
    exit 1
}

[ $# -eq 0 ] && usage

while getopts :s:p:n:t: OPTION
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
	t)
	    FILES_TYPE=$OPTARG
	    ;;
        \?)               
            usage
            ;;
    esac
done

shift $(($OPTIND - 1))

if [ -z "$SPLIT_FILE" ]; then  
    echo "You must specify SPLIT_FILE with -s option"
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
mkdir -p ./datafile
cd datafile
split -l $LINE_NUM $SPLIT_FILE $FILES_PREFIX -d
for files in `ls`
do 
    mv $files `echo "$files.$FILES_TYPE" `
done
