package atrmat;

import java.io.File;
import java.io.IOException;
import java.util.List;

import org.apache.mahout.cf.taste.common.TasteException;
import org.apache.mahout.cf.taste.impl.model.file.FileDataModel;
import org.apache.mahout.cf.taste.impl.neighborhood.NearestNUserNeighborhood;
import org.apache.mahout.cf.taste.impl.recommender.GenericItemBasedRecommender;
import org.apache.mahout.cf.taste.impl.recommender.GenericUserBasedRecommender;
import org.apache.mahout.cf.taste.impl.similarity.LogLikelihoodSimilarity;
import org.apache.mahout.cf.taste.impl.similarity.PearsonCorrelationSimilarity;
import org.apache.mahout.cf.taste.model.DataModel;
import org.apache.mahout.cf.taste.neighborhood.UserNeighborhood;
import org.apache.mahout.cf.taste.recommender.ItemBasedRecommender;
import org.apache.mahout.cf.taste.recommender.RecommendedItem;
import org.apache.mahout.cf.taste.recommender.Recommender;
import org.apache.mahout.cf.taste.similarity.ItemSimilarity;
import org.apache.mahout.cf.taste.similarity.UserSimilarity;

public class RecommenderIntro {

    public static void main(String[] args) throws TasteException {
        try {
            DataModel dataModel=new FileDataModel(new File("H://intro.csv"));
            UserSimilarity similarity=new PearsonCorrelationSimilarity(dataModel);
            UserNeighborhood neighborhood=new NearestNUserNeighborhood(2,similarity,dataModel);
            Recommender recommender=new GenericUserBasedRecommender(dataModel,neighborhood,similarity);
            List<RecommendedItem> recommendations=recommender.recommend(1,1);
            for(RecommendedItem recommendation : recommendations)
                System.out.println(recommendation);
            System.out.println("===============================================");
            //Create an ItemSimilarity
            ItemSimilarity itemSimilarity = new LogLikelihoodSimilarity(dataModel);
            //Create an Item Based Recommender
            ItemBasedRecommender itemBasedRecommender =
                    new GenericItemBasedRecommender(dataModel, itemSimilarity);
            //Get the recommendations
            List<RecommendedItem> itemBasedRecommendations =
            		itemBasedRecommender.recommend(1, 5);
            for(RecommendedItem itemBasedRecommendation : itemBasedRecommendations)
            	System.out.println(itemBasedRecommendation);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
}
