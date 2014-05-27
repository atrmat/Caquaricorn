awk 'BEGIN{srand();while(i<10000){k=int(rand()*100000000);if(!(k in a)){a[k]++;i++}}}(NR in a)' urfile
