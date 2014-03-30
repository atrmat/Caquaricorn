package atrmat;

import org.apache.mahout.common.distance.SquaredEuclideanDistanceMeasure;
import org.apache.mahout.math.Vector;

public class ATRDistanceMeasure extends SquaredEuclideanDistanceMeasure{
	@Override
	public double distance(Vector v1, Vector v2){
		int sizeNum = v1.size();
		double distanceResult = 0.0;
		if(v2.size()!=v1.size()){
			System.out.println("vector 1 isn't same size with vector 2");
		}
		System.out.println("vector(0):"+v1.get(0)+","+v2.get(0));
		for(int i = 1; i < sizeNum; i++){
			distanceResult += Math.abs(v1.get(i)*v1.get(i) - v2.get(i)*v2.get(i));
		}
		return Math.sqrt(distanceResult);
	}
}
