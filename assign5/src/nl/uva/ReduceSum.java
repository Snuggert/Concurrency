package nl.uva;

import java.io.IOException;
import java.util.Iterator;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.impl.NoOpLog;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;

/**
 * This reducer should calculate the sums for each row in matrix
 * @author S. Koulouzis
 */
public class ReduceSum extends MapReduceBase implements Reducer<Text, Text, Text, Text> {

    Log log = LogFactory.getLog(nl.uva.ReduceSum.class);
    // NoOpLog log = new NoOpLog();
    
    private String rowNum;
    private String columnNum;
    private String elementValue;
    private Text value = new Text();
    private Text key = new Text();

    static enum Counters {

        OUTPUT_LINES
    }

    @Override
    public void reduce(Text k2, Iterator<Text> itrtr, OutputCollector<Text, Text> oc, Reporter rprtr) throws IOException {
        Double result = Double.valueOf(0);
        while (itrtr.hasNext()) {
            result += Double.parseDouble(itrtr.next().toString());
        }
        // System.out.println("Key location: " + k2.toString());
        
        //Here you have to calculate the rowNum and columnNum to use them as a key
        rowNum = k2.toString().split(",")[0];
        columnNum = k2.toString().split(",")[1];
        
        value.set(columnNum + "," + result);
        key.set(rowNum);
        oc.collect(key, value);
    }
}
