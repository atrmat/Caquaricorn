echo "merge temp_n : 0 - 255 \n"
/root/cx_src/src/merge_N /tmp/data /tmp/result/ino.txt /tmp/result/lno.txt /tmp/result/cdo.txt 4 0 255 
echo "merge temp_n : 256 - 511 \n"
/root/cx_src/src/merge_N /tmp/data /tmp/result/ino.txt /tmp/result/lno.txt /tmp/result/cdo.txt 4 256 511 
echo "merge temp_n : 512 - 767 \n"
/root/cx_src/src/merge_N /tmp/data /tmp/result/ino.txt /tmp/result/lno.txt /tmp/result/cdo.txt 4 512 767 
echo "merge temp_n : 768 - 1023 \n"
/root/cx_src/src/merge_N /tmp/data /tmp/result/ino.txt /tmp/result/lno.txt /tmp/result/cdo.txt 4 768 1023 
