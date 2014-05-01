/**
 * name: Georgi Angelov
 * date: 4/30/2014
 * Project 4, Version 2
 */
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

/**
 *	This class is the Client in a Client-Server socket chat room.
 *	This client simply accepts messages from the Server and also sends messages to the server
 */
public class ChatClient {

    BufferedReader in;
    PrintWriter out;
    public static final String SERVER_LOGOUT_MESSAGE = "SERVER:logout";
    private static final int PORT = 10841;
    
    /**
     * Connects to the server then enters the processing loop.
     */
    private void run() throws IOException {
    	System.out.println("My chat room client. Version Two.");
        // Make connection and initialize the input/output streams
        final Socket socket = new Socket("localhost", PORT);
        in = new BufferedReader(new InputStreamReader(
            socket.getInputStream()));
        out = new PrintWriter(socket.getOutputStream(), true);
        
        //intialize a buffer reader object to read from the Standard In
        final BufferedReader br = 
                new BufferedReader(new InputStreamReader(System.in));
        
        //Start a new thread that will handle all messages that this client is trying to type
        new Thread() {  
            public void run() {  
            	//synchronize the socket so we don't create race condition
                synchronized(socket) {  
            		while(true){
	                	try {
	            			String name = null;
	            			name = br.readLine();
	            			out.println(name);
	            			//if the user is trying to logout, exit
	            			if(name.equals("logout")){
	            				return;
	            			}
	            		} catch (IOException e) {
	            			// TODO Auto-generated catch block
	            			e.printStackTrace();
	            			System.out.println("ERROR READING NAME!!!");
	            		}
            		}
                }  
                // Process response  
            }  
        }.start();
        
        // Process all messages from server, according to the protocol.
        while (true) {
        	String line = in.readLine();
        	if(line.equals(SERVER_LOGOUT_MESSAGE)){
        		return;
        	}
        	System.out.println(line);
        }
    }

    /**
     * Instantiate the client object and start the thread
     */
    public static void main(String[] args) throws Exception {
        ChatClient client = new ChatClient();
        client.run();
    }
}