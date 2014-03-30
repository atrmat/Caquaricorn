package atrmat;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;

/**
 * @author atrmat
 * @func put N different balls into M same drawers
 * */
public class Combinatorics {

	public static int N = 4;
	public static int M = 6;
	public static int p[]= new int[M];
	public static int result[][] = new int[1000][M];
	public static int resultSum = 0;
	
	public static void permute(int pos, int val){
		if (pos == M){
			int tmp = 0;
			for (int i = 0; i < p.length; i++){
				tmp += p[i];
			}
			if (tmp == N){
//				result[resultSum] = p;
				for(int i = 0 ;i < p.length; i++){
					System.out.print(p[i]+ " ");
					result[resultSum][i] = p[i];
				}
				System.out.print("\n");
				resultSum++;
			}
			return;
		}
		for (int i = 0; i <= val; i++){
			p[pos] = i;
			permute(pos+1, val-i);
		}
	}
	
	public static void main(String[] args) throws IOException {
		// TODO Auto-generated method stub
		permute(0,N);
		System.out.println(resultSum);
		File file = new File("permute.csv");
		FileOutputStream out = new FileOutputStream(file);
		OutputStreamWriter osw = new OutputStreamWriter(out, "UTF8");
		BufferedWriter bw = new BufferedWriter(osw);
		bw.write("A,B,C,D,E,F\n");
		for (int i = 0; i < resultSum; i++){
			for (int j = 0; j < M-1; j++)
				bw.write(result[i][j]+",");
			bw.write(result[i][M-1]+"\n");
		}
		bw.close();
		osw.close();
		out.close();
	}

}
