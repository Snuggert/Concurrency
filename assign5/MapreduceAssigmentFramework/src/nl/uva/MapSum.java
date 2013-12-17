/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.uva;

import java.io.IOException;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

/**
 * This mapper reads the results from the multiplication reducer. It should 
 * read/attach the right keys to the value and emit them to the reducer
 * 
 * @author S. Koulouzis
 */
public class MapSum extends MapReduceBase implements Mapper<LongWritable, Text, Text, Text> {

    Log log = LogFactory.getLog(nl.uva.MapSum.class);

    static enum Counters {

        INPUT_LINES
    }

    @Override
    public void map(LongWritable k1, Text v1, OutputCollector<Text, Text> oc, Reporter rprtr) throws IOException {
        //Insert you code here. In this case you know you are reading from 
    }
}
