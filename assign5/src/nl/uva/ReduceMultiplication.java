package nl.uva;

import java.io.IOException;
import java.util.Iterator;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.*;


/**
 * This reducer is performing the actual multiplication between rows and
 * columns. As long as the mapper is given the correct keys, the correct rows
 * and columns will be grouped together.
 *
 * @author S. Koulouzis
 */
public class ReduceMultiplication extends MapReduceBase implements Reducer<Text, Text, Text, Text> {

    Log log = LogFactory.getLog(ReduceMultiplication.class);
    Text emitKey = new Text();
    Text emitValue = new Text();

    static enum Counters {

        RESAULT
    }

    @Override
    public void reduce(Text key, Iterator<Text> valueItrtr, OutputCollector<Text, Text> output, Reporter rprtr) throws IOException {
        int index = 0;
        //The rows and columns will be grouped on the same keys. 
        //You have to split  them 
        while (valueItrtr.hasNext()) {
             //Incert your code here to get row from matrix A and column from matrix B
        }
        
   
        Double result = Double.valueOf(0);
        //Incert your code here to perform the actual multiplication. 
        
        
        
        rprtr.incrCounter(ReduceMultiplication.Counters.RESAULT, 1);
        
        //Set these values so the output key represents the correct element of the 
        //result matrix C 
        String rowKey = null;
        String colKey = null;
        String elementKey = null;
        emitKey.set(rowKey + "," + colKey);

        emitValue.set(elementKey + "," + result);
        output.collect(emitKey, emitValue);
    }
}