import java.util.Random;

public class JavaMeasurements {

    static final int SIZE_OF_ARRAY = 100000;
    static final int TRIES = 1000;

    private static long dynamicMemoryAllocationTime(){

        long startTime, endTime, sum = 0;
        for(int i = 0 ; i < TRIES; i ++){
            startTime = System.nanoTime();

            int[] array = new int[SIZE_OF_ARRAY];

            endTime = System.nanoTime();
            sum += endTime - startTime;
        }

        return sum/TRIES;
    }

    private static long dynamicMemoryAccessTime() {
        long sum = 0;

        Random random = new Random();

        int[] array = new int[SIZE_OF_ARRAY];
        for (int i = 0; i < SIZE_OF_ARRAY; i++)
            array[i] = random.nextInt(10000);

        int a = 0;

        for (int i = 0; i < TRIES; i++) {

            long startTime = System.nanoTime();

            for (int j = 0; j < SIZE_OF_ARRAY; j++)
               a = array[j];

            long endTime = System.nanoTime();
            sum += endTime - startTime;
        }
        return sum / TRIES;
    }

    private static long threadCreationTime(){

        long sum = 0;
        for (int i = 0; i < TRIES; i++) {
        
            long startTime = System.nanoTime();
            Thread thread = new Thread();
            thread.setDaemon(true); // don't wait for threads ending; does not affect the measurements
            thread.start();

            long endTime = System.nanoTime();
            sum += endTime - startTime;
        }
        return sum/TRIES;
    }

    public static void main(String[] args) {
        
        System.out.println("JAVA MEASUREMENTS");
        System.out.println("Dynamic memory allocation: " + dynamicMemoryAllocationTime());
        System.out.println("Dynamic memory access: " + dynamicMemoryAccessTime());
        System.out.println("Thread creation: " + threadCreationTime());
        System.out.println("_______________________________________________________\n");
    }
}
