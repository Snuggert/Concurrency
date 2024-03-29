/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.uva;

import java.io.IOException;
import java.util.StringTokenizer;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.impl.NoOpLog;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

/**
 * This mapper reads matrices A and B. Each on these matrices starts with a line
 * number. 
 * This mapper responsible for attaching the correct key to the correct value. 
 * Hint: you need to emit a line multiple times with different keys
 *
 * @author S. Koulouzis
 */
public class MapMultiplication extends MapReduceBase implements Mapper<LongWritable, Text, Text, Text> {

    Log log = LogFactory.getLog(nl.uva.MapMultiplication.class);
    // NoOpLog log = new NoOpLog();
    
    private String matrixName;
    private Text elements = new Text();
    private Text emitKey = new Text();
    private int numOfMul;
    private int numOfRowsInC;
    private int numOfColumnsInC;

    @Override
    public void configure(JobConf conf) {
        String[] fileNameTokens = conf.get("map.input.file").split("/");
        String tmp = fileNameTokens[fileNameTokens.length - 1];
        if (tmp.startsWith("B")) {
            matrixName = "B";
        } else if (tmp.startsWith("A")) {
            matrixName = "A";
        } else {
            matrixName = tmp.substring(tmp.length() - 1, tmp.length());
        }
        numOfRowsInC = conf.getInt("rows.in.matrixC", 1);
        numOfColumnsInC = conf.getInt("columns.in.matrixC", 1);
        int numOfColumnsInA = conf.getInt("columns.in.matrixA", 1);
        int tmpNumOfMul = conf.getInt("multiplications.per.reducer", 1);
        numOfMul = Math.min(numOfColumnsInA, tmpNumOfMul);
    }

    @Override
    public void map(LongWritable key, Text value, OutputCollector<Text, Text> oc, Reporter rprtr) throws IOException {
        String rowColNum = value.toString().split("\t")[0];
        String rowCol = value.toString().split("\t")[1];
        String[] rowColSplit = rowCol.split(" ");
        Text newKey;
        Text newValue;
        StringBuilder sb;
        if (matrixName.equals("A")){
        //Insert you code here. In this case you know you are reading from 
        //matrix A. Decide how to counstruct your key.
            for(int i = 0; i<rowColSplit.length; i+=numOfMul){
                sb = new StringBuilder();
                for(int j = i; j<numOfMul; j++){
                    sb.append(rowColSplit[j]);
                    sb.append(" ");
                }
                newValue = new Text(sb.toString());

                for(int h = 0; h<numOfColumnsInC; h++){
                    sb = new StringBuilder();
                    sb.append(h);
                    sb.append("\t");
                    sb.append(rowColNum);
                    sb.append("\t");
                    sb.append(i/numOfMul);
                    newKey = new Text(sb.toString());               
                    oc.collect(newKey, newValue);
                }
            }
        } 
        else if (matrixName.equals("B")){
        //Insert you code here. In this case you know you are reading from 
        //matrix B. Decide how to counstruct your key.
            for(int i = 0; i<rowColSplit.length; i+=numOfMul){
                sb = new StringBuilder();
                for(int j = i; j<numOfMul; j++){
                    sb.append(rowColSplit[j]);
                    sb.append(" ");
                }
                newValue = new Text(sb.toString());

                for(int h = 0; h<numOfRowsInC; h++){
                    sb = new StringBuilder();
                    sb.append(rowColNum);
                    sb.append("\t");
                    sb.append(h);
                    sb.append("\t");
                    sb.append(i/numOfMul);
                    newKey = new Text(sb.toString());               
                    oc.collect(newKey, newValue);
                }
            }  
        }
    }
}
