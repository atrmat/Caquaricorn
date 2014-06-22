all:
	clang -Wall -O2 -o prepare -lJudy prepare.c
	clang -Wall -O2 -o prepare_N -lJudy prepare_N.c
	clang -Wall -O2 -o merge_N -lJudy merge_N.c
	clang -Wall -O2 -o get_sample -lJudy get_sample.c -g
	clang -Wall -O2 -o get_sample_N -lJudy get_sample_N.c -g
	clang -Wall -O2 -o jbc_build -lJudy jbc_build.c
	clang -Wall -O2 -o jbc_build_blooms -fPIC -lJudy jbc_build_blooms.c -lm -ldablooms -g
#	clang -Wall -O2 -o jbc_build_blooms -fPIC -lJudy /usr/local/lib/libdablooms.a jbc_build_blooms.c -lm -ldablooms
	clang -Wall -O2 -o jbc_build_N -lJudy jbc_build_N.c
	clang -Wall -O2 -o jbc_index -fPIC -lJudy jbc_index.c -lm -ldablooms -g
	clang -Wall -O2 -o jbc_index_N -lJudy jbc_index_N.c
	clang -Wall -O2 -o jbc_update -lJudy jbc_update.c
	clang -Wall -O2 -o jbc_update_N -lJudy jbc_update_N.c
	clang -Wall -O2 -o redis_build -lhiredis -lJudy redis_build.c
	clang -Wall -O2 -o redis_index -lhiredis redis_index.c
	clang -Wall -O2 -o redis_update -lhiredis redis_update.c
	clang -Wall -O2 -o leveldb_build -lJudy -lleveldb leveldb_build.c
	clang -Wall -O2 -o leveldb_index -lleveldb leveldb_index.c
	clang -Wall -O2 -o leveldb_update -lleveldb leveldb_update.c
	clang -Wall -O2 -o performance performance.c
#	g++ gearman_client_bg.cc -lgearman -lboost_program_options -I /usr/include/ -o gearman_client_bg 
#	g++ gearman_client_merge.cc -lgearman -lboost_program_options -I /usr/include/ -o gearman_client_merge -g 
#	g++ gearman_worker.cc -lgearman -lboost_program_options -I /usr/include/ -o gearman_worker
debug:
	clang -Wall -g -o prepare -lJudy prepare.c
	clang -Wall -g -o get_sample -lJudy get_sample.c
	clang -Wall -g -o jbc_build -lJudy jbc_build.c
	clang -Wall -g -o jbc_index -lJudy jbc_index.c
	clang -Wall -g -o jbc_update -lJudy jbc_update.c
	clang -Wall -g -o redis_build -lhiredis -lJudy redis_build.c
	clang -Wall -g -o redis_index -lhiredis redis_index.c
	clang -Wall -g -o redis_update -lhiredis redis_update.c
	clang -Wall -g -o leveldb_build -lJudy -lleveldb leveldb_build.c
	clang -Wall -g -o leveldb_index -lleveldb leveldb_index.c
	clang -Wall -g -o leveldb_update -lleveldb leveldb_update.c
	clang -Wall -g -o performance performance.c
gcc:
	gcc -Wall -O2 -o prepare -lJudy prepare.c
	gcc -Wall -O2 -o get_sample -lJudy get_sample.c
	gcc -Wall -O2 -o jbc_build -lJudy jbc_build.c
	gcc -Wall -O2 -o jbc_index -lJudy jbc_index.c
	gcc -Wall -O2 -o jbc_update -lJudy jbc_update.c
	gcc -Wall -O2 -o redis_build -lhiredis -lJudy redis_build.c
	gcc -Wall -O2 -o redis_index -lhiredis redis_index.c
	gcc -Wall -O2 -o redis_update -lhiredis redis_update.c
	gcc -Wall -O2 -o leveldb_build -lJudy -lleveldb leveldb_build.c
	gcc -Wall -O2 -o leveldb_index -lleveldb leveldb_index.c
	gcc -Wall -O2 -o leveldb_update -lleveldb leveldb_update.c
	gcc -Wall -O2 -o performance performance.c

gcc_debug:
	gcc -Wall -g -o prepare -lJudy prepare.c
	gcc -Wall -g -o get_sample -lJudy get_sample.c
	gcc -Wall -g -o jbc_build -lJudy jbc_build.c
	gcc -Wall -g -o jbc_index -lJudy jbc_index.c
	gcc -Wall -g -o jbc_update -lJudy jbc_update.c
	gcc -Wall -g -o redis_build -lhiredis -lJudy redis_build.c
	gcc -Wall -g -o redis_index -lhiredis redis_index.c
	gcc -Wall -g -o redis_update -lhiredis redis_update.c
	gcc -Wall -g -o leveldb_build -lJudy -lleveldb leveldb_build.c
	gcc -Wall -g -o leveldb_index -lleveldb leveldb_index.c
	gcc -Wall -g -o leveldb_update -lleveldb leveldb_update.c
	gcc -Wall -g -o performance performance.c


performance_test:
	./performance mnt/btree.bin
download:
	./download.sh 5 0 800
run_prepare:
	./prepare ../5-gram/googlebooks-eng-all-5gram-20090715- 800 ../data/temp ../results/ino.txt ../results/lno.txt ../results/cdo.txt
	python2 ./Length_Distribution.py ../results/lno.txt ../results/Length_Distribution.dat
	./get_sample ../data/temp
run_jbc_build:
	./jbc_build ../data/temp mnt/btree.bin ../results/jbt.txt
	cp mnt/btree.bin ../data/
	python2 ./JBc_Build_Information.py ../results/jbt.txt ../data/temp/temp_ ../results/JBI.dat
	python2 ./Range.py ../results/JBI.dat
	python2 ./Merge.py ../results/JBI.dat ../results/JBc_Build_Information.dat 1024 16
	python2 ./Range.py ../results/JBc_Build_Information.dat
run_jbc_index:
	./jbc_index mnt/btree.bin n ../data/index_sample.txt ../results/jbc_index_out.txt ../results/jit.txt
	python2 ./JBc_Index_Information.py ../results/jit.txt ../results/JII.dat
	python2 ./Range.py ../results/JII.dat
	python2 ./Merge.py ../results/JII.dat ../results/JBc_Index_Information.dat 1024 16
	python2 ./Range.py ../results/JBc_Index_Information.dat
run_jbc_index_cache:
	./jbc_index mnt/btree.bin y ../data/index_sample.txt ../results/jbc_index_cache_out.txt ../results/jit_c.txt
	python2 ./JBc_Index_Information.py ../results/jit_c.txt ../results/JII_c.dat
	python2 ./Range.py ../results/JII_c.dat
	python2 ./Merge.py ../results/JII_c.dat ../results/JBc_Index_Information_Cache.dat 1024 16
	python2 ./Range.py ../results/JBc_Index_Information_Cache.dat
run_jbc_update:
	./jbc_update ../data/update_sample.txt ../data/btree.bin mnt/modified_btree.bin ../results/jut.txt
	python2 ./JBc_Update_Information.py ../results/jut.txt ../results/JUI.dat
	python2 ./Range.py ../results/JUI.dat
	python2 ./Merge.py ../results/JUI.dat ../results/JBc_Update_Information.dat 1024 16
	python2 ./Range.py ../results/JBc_Update_Information.dat
run_redis_build:
	./redis_build ../data/temp/ ../results/rbt.txt
	python2 ./Redis_Build_Information.py ../results/rbt.txt ../results/RBI.dat
	python2 ./Range.py ../results/RBI.dat
	python2 ./Merge.py ../results/RBI.dat ../results/Redis_Build_Information.dat 256 16
	python2 ./Range.py ../results/Redis_Build_Information.dat
run_redis_index:
	./redis_index ../results/rit.txt
	python2 ./Redis_Index_Information.py ../results/rit.txt ../results/RII.dat
	python2 ./Range.py ../results/RII.dat
	python2 ./Merge.py ../results/RII.dat ../results/Redis_Index_Information.dat 10000 16
	python2 ./Range.py ../results/Redis_Index_Information.dat
run_redis_update:
	./redis_update ../results/rut.txt
	python2 ./Redis_Update_Information.py ../results/rut.txt ../results/RUI.dat
	python2 ./Range.py ../results/RUI.dat
	python2 ./Merge.py ../results/RUI.dat ../results/Redis_Update_Information.dat 10000 16
	python2 ./Range.py ../results/Redis_Update_Information.dat
run_leveldb_build:
	./leveldb_build ../data/temp mnt ../results/lbt.txt
	cp mnt/* ../data/leveldb/
	python2 ./LevelDB_Build_Information.py ../results/lbt.txt ../results/LBI.dat
	python2 ./Range.py ../results/LBI.dat
	python2 ./Merge.py ../results/LBI.dat ../results/LevelDB_Build_Information.dat 1024 16
	python2 ./Range.py ../results/LevelDB_Build_Information.dat
run_leveldb_index:
	./leveldb_index mnt ../data/index_sample.txt ../data/leveldb_index_out.txt lit.txt
	python2 ./JBc_Index_Information.py ../results/lit.txt ../results/LII.dat
	python2 ./Range.py ../results/LII.dat
	python2 ./Merge.py ../results/LII.dat ../results/LevelDB_Index_Information.dat 1024 16
	python2 ./Range.py ../results/LevelDB_Index_Information.dat
run_leveldb_update:
	./leveldb_update ../data/update_sample.txt mnt/ ../result/lut.txt
	python2 ./JBc_Update_Information.py ../results/lut.txt ../results/LUI.dat
	python2 ./Range.py ../results/LUI.dat
	python2 ./Merge.py ../results/LUI.dat ../results/LevelDB_Update_Information.dat 1024 16
	python2 ./Range.py ../results/LevelDB_Update_Information.dat
clean:
	rm -f prepare jbc_build get_sample jbc_index jbc_index jbc_update redis_build redis_index redis_update leveldb_build leveldb_index leveldb_update performance
