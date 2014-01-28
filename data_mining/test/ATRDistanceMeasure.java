package atrmat;

import org.apache.mahout.common.distance.SquaredEuclideanDistanceMeasure;
import org.apache.mahout.math.Vector;

public class ATRDistanceMeasure extends SquaredEuclideanDistanceMeasure{
	@Override
	public double distance(Vector v1, Vector v2){
		return Math.sqrt(Math.abs(v1.get(0)*v1.get(0) - v2.get(0)*v2.get(0)));
	}
}
