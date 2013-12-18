package nl.uva;

import java.io.IOException;
import java.util.Iterator;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.impl.NoOpLog;
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
    // NoOpLog log = new NoOpLog();
    
    Text emitKey = new Text();
    Text emitValue = new Text();

    static enum Counters {

        RESAULT
    }

    @Override
    public void reduce(Text key, Iterator<Text> valueItrtr, OutputCollector<Text, Text> output, Reporter rprtr) throws IOException {
        //The rows and columns will be grouped on the same keys. 
        //You have to split  them 
        Text a = valueItrtr.next();
        Text b = valueItrtr.next();
        String[] aSplit = a.toString().split(" ");
        String[] bSplit = b.toString().split(" ");

        if(bSplit.length != aSplit.length){
            System.out.println("Matrices are not aligned");
            System.exit(-1);
        }

        System.out.println("A: " + a.toString());
        System.out.println("B: " + b.toString());
        System.out.println("Key location: " + key.toString());

        Double result;
        StringBuilder sb = new StringBuilder();
        for(int i = 0; i < aSplit.length; i++){
            result = Double.parseDouble(aSplit[i]) * 
                     Double.parseDouble(bSplit[i]);
            sb.append(result);
            sb.append(" ");
        }
        emitValue = new Text(sb.toString());
        
        rprtr.incrCounter(ReduceMultiplication.Counters.RESAULT, 1);
        
        //Set these values so the output key represents the correct element of the 
        //result matrix C
        String rowKey = key.toString().split("\t")[0];
        String colKey = key.toString().split("\t")[1];
        emitKey.set(colKey + "," + rowKey);

        output.collect(emitKey, emitValue);
    }
}