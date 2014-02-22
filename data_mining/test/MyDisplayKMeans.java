/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.apache.mahout.clustering.display;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.io.IOException;
import java.util.Collection;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.mahout.clustering.Cluster;
import org.apache.mahout.clustering.canopy.CanopyDriver;
import org.apache.mahout.clustering.classify.ClusterClassifier;
import org.apache.mahout.clustering.iterator.ClusterIterator;
import org.apache.mahout.clustering.iterator.KMeansClusteringPolicy;
import org.apache.mahout.clustering.kmeans.KMeansDriver;
import org.apache.mahout.clustering.kmeans.RandomSeedGenerator;
import org.apache.mahout.common.HadoopUtil;
import org.apache.mahout.common.RandomUtils;
import org.apache.mahout.common.distance.DistanceMeasure;
import org.apache.mahout.common.distance.ManhattanDistanceMeasure;
import org.apache.mahout.math.Vector;

import com.google.common.collect.Lists;

public class MyDisplayKMeans extends DisplayClustering {
  /**
   * MyDisplayKmeans: compare the original Kmeans and the Canopy-Kmeans
   * Based on the Mahout 0.9 - example
   * put it under the mahout-examples project
   * */
  MyDisplayKMeans() {
    initialize();
    this.setTitle("k-Means Clusters (>" + (int) (significance * 100) + "% of population)");
  }
  MyDisplayKMeans(boolean Canopy) {
	    initialize();
	    this.setTitle("Canopy k-Means Clusters (>" + (int) (significance * 100) + "% of population)");
  }
  
  public static void main(String[] args) throws Exception {
    DistanceMeasure measure = new ManhattanDistanceMeasure();
    Path samples = new Path("samples");
    Path output = new Path("Kmeans output");
    Path output1 = new Path("Canopy-Kmeans output");
    Configuration conf = new Configuration();
    HadoopUtil.delete(conf, samples);
    HadoopUtil.delete(conf, output);
    HadoopUtil.delete(conf, output1);
    
    RandomUtils.useTestSeed();
    generateSamples();
    writeSampleData(samples);
    boolean runClusterer = true;
    double convergenceDelta = 0.001;
    int numClusters = 3;
    int maxIterations = 10;
    if (runClusterer) {
      runSequentialKMeansClusterer(conf, samples, output, measure, numClusters, maxIterations, convergenceDelta);
    } else {
      runSequentialKMeansClassifier(conf, samples, output, measure, numClusters, maxIterations, convergenceDelta);
    }
    new MyDisplayKMeans();
    runSequentialKMeansCanopyClusterer(conf, samples, output1, measure, numClusters, maxIterations, convergenceDelta);
    new MyDisplayKMeans(true);
  }
  
  private static void runSequentialKMeansClassifier(Configuration conf, Path samples, Path output,
      DistanceMeasure measure, int numClusters, int maxIterations, double convergenceDelta) throws IOException {
    Collection<Vector> points = Lists.newArrayList();
    for (int i = 0; i < numClusters; i++) {
      points.add(SAMPLE_DATA.get(i).get());
    }
    List<Cluster> initialClusters = Lists.newArrayList();
    int id = 0;
    for (Vector point : points) {
      initialClusters.add(new org.apache.mahout.clustering.kmeans.Kluster(point, id++, measure));
    }
    ClusterClassifier prior = new ClusterClassifier(initialClusters, new KMeansClusteringPolicy(convergenceDelta));
    Path priorPath = new Path(output, Cluster.INITIAL_CLUSTERS_DIR);
    prior.writeToSeqFiles(priorPath);
    
    ClusterIterator.iterateSeq(conf, samples, priorPath, output, maxIterations);
    loadClustersWritable(output);
  }
  
  private static void runSequentialKMeansClusterer(Configuration conf, Path samples, Path output,
    DistanceMeasure measure, int numClusters, int maxIterations, double convergenceDelta)
    throws IOException, InterruptedException, ClassNotFoundException {
    Path clustersIn = new Path(output, "random-seeds");
    RandomSeedGenerator.buildRandom(conf, samples, clustersIn, numClusters, measure);
    KMeansDriver.run(samples, clustersIn, output, measure, convergenceDelta, maxIterations, true, 0.0, true);
    loadClustersWritable(output);
  }
  private static void runSequentialKMeansCanopyClusterer(Configuration conf, Path samples, Path output,
		    DistanceMeasure measure, int numClusters, int maxIterations, double convergenceDelta)
		    throws IOException, InterruptedException, ClassNotFoundException {
		    Path canopyOutput = new Path("Canopies");
		    HadoopUtil.delete(conf, canopyOutput);
//		    RandomSeedGenerator.buildRandom(conf, samples, clustersIn, numClusters, measure);
		    CanopyDriver.buildClusters(conf, samples, canopyOutput, new ManhattanDistanceMeasure(), T1, T2, 0, true);
		    KMeansDriver.run(samples, new Path(canopyOutput,Cluster.INITIAL_CLUSTERS_DIR + "-final"), output, measure, convergenceDelta, maxIterations, true, 0.0, true);
		    loadClustersWritable(output);
		  }
  // Override the paint() method
  @Override
  public void paint(Graphics g) {
    plotSampleData((Graphics2D) g);
    plotClusters((Graphics2D) g);
  }
}
