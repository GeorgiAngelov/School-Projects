/**
 * name: Georgi Angelov
 * date: 4/30/2014
 * Project 4, Version 2
 */

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Scanner;

/**
 *	This is the Server class in a Server-Client multi user chat room.
 *	The purpose of this class is to facilitate the responsibilities of a Server
 *	by allowing users to login(using credentials saved in a file) and to send messages
 *	to all users in the chat or to a specific user in the chat.
 *	The user sends a message using the "send all/<specific_user> <message>" pattern where
 *	anything inside a <> means variable data. The user is also able to view who is logged in 
 *	by executing the "who" command. Furthermore, the user is also able to logout using the "logout" command.
 *
 */
public class Server {

    /**
     * The port that the server listens on.
     */
    private static final int PORT = 10841;
    private static final int MAXCLIENTS = 3;
    public static final String SERVER_LOGOUT_MESSAGE = "SERVER:logout";

    /**
     * The set of all the print writers for all the clients.  This
     * set is kept so we can easily broadcast messages.
     */
    private static Map<String, PrintStream> writers = new HashMap<String, PrintStream>();
    
    private static Map<String, String> user_credentials = new HashMap<String,String>();
    private static Map<String, Boolean> user_logged = new HashMap<String, Boolean>();

    /**
     * The appplication main method, which just listens on a port and
     * spawns handler threads.
     */
    public static void main(String[] args) throws Exception {

    	System.out.println("My chat room client. Version Two.");
        ServerSocket listener = new ServerSocket(PORT);
        
        // Location of file to read that contains the username and password combinations
        File file = new File("users.txt");
 
        //Read the user and password pairs from the file
        try {
            Scanner scanner = new Scanner(file);
            while (scanner.hasNextLine()) {
                String line = scanner.nextLine();
                String[] user_pass = line.split(" ");
                user_credentials.put(user_pass[0],user_pass[1]);
            }
            scanner.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        try {
            //allow only MAXCLIENTS to be in the chat room at the same time
            while(true){
            	if(writers.size() < MAXCLIENTS)
            		new Handler(listener.accept()).start();
            }
        } finally {
            listener.close();
        }
    }

    /**
     * A handler thread class.  Handlers are spawned from the listening
     * loop and are responsible for a dealing with a single client
     * and broadcasting its messages.
     */
    private static class Handler extends Thread {
        private Socket socket;
        private BufferedReader in;
        private PrintStream out;

        /**
         * Constructs a handler thread, squirreling away the socket.
         * All the interesting work is done in the run method.
         */
        public Handler(Socket socket) {
            this.socket = socket;
        }

        /**
         * Services this thread's client by repeatedly requesting a
         * screen name until a unique one has been submitted, then
         * acknowledges the name and registers the output stream for
         * the client in a global set, then repeatedly gets inputs and
         * broadcasts them.
         */
        public void run() {
            try {
                // Create character streams for the socket.
                in = new BufferedReader(new InputStreamReader(
                    socket.getInputStream()));
                out =  new PrintStream(socket.getOutputStream());
                String message;
                String[] tok;
                String username;
                String stripped_message;
                String password;
                while(true){
                	message = in.readLine();
                	//if the client is trying to login
                    if (message.startsWith("login")) {
                    	tok = message.split(" ");
                    	//if the username is in our hashmap and if the passwords match, then login the user!
                    	if(tok.length == 3 && user_credentials.containsKey(tok[1]) && user_credentials.get(tok[1]).equals(tok[2])){
                    		//if someone is already logged in with the same username, kick them out
                    		if(user_logged.containsKey(tok[1]) && user_logged.get(tok[1]) == true){
                    			System.out.println("user is already logged in and someone is trying to login with the same username");
                    			out.println("You are a bad person! Someone is already logged in with this username. Leave now!");
                    			return;
                    		}
                    		username = tok[1];
                    		System.out.println(tok[1] +" login");
                        	out.println("login confirmed");
                    		user_logged.put(tok[1], true);
                            sendMessageToAll(username, username+ " joins", true);
                        	break;
                    	}
                    }
                    //if the client is trying to create a new account
                    else if(message.startsWith("newuser")){
                    	tok = message.split(" ");
                    	if(tok.length == 3){
                    		//if username is not already created
                    		if(!user_credentials.containsKey(tok[1])){
                    			username = tok[1];
                    			password = tok[2];
                    			int u_length = username.length();
                    			int p_length = password.length();
                    			//make sure the lengths are correct and if so, log the user in
                    			if(u_length > 0 && u_length < 32 && p_length > 3 && p_length < 9){
                    				//save the new username and password to the file containing all usernames and passwords
                    				try(PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter("users.txt", true)))) {
                    				    out.println(username + " " + password);
                    				}catch (IOException e) {
                    					//print to server console
                        				System.out.println(tok[1] +" FAILED TO SAVE TO FILE");
                        				//print to creating node
                                    	out.println("account NOT created. Please try again.");
                                    	continue;
                    				}
                    				
                    				//print to server console
                    				System.out.println(tok[1] +" created");
                    				//print to creating node
                                	out.println("account created");
                                	//store the new credentials in the map
                    				user_credentials.put(username, password);
                    				continue;
                    			}else{
                    				out.println("Error. Username should be less than 32 chars and Password should be between 4 and 8 characters");
                    				continue;
                    			}
                    		}else{
                    			out.println("The username is already created.");
                    			continue;
                    		}
                    	}else{
                    		out.println("Not enough arguments for creating an account supplied. Required 'newuser <username> <password>'");
                    		continue;
                    	}
                    }
                    out.println("Denied. Please login first.");
                }
                
                writers.put(username, out);

                // Accept messages from this client and broadcast them.
                // Ignore other clients that cannot be broadcasted to.
                while (true) {
                    message = in.readLine();
                    //if it is a send message
                    if(message.startsWith("send")){
                    	tok = message.split(" ");
                    	//if we are to send a message to all!
                    	if(tok.length > 2 && tok[1].equals("all")){
                        	//strip the first 2 words in the sentece as they are obsolete
                        	stripped_message = message.split("send all")[1];
                        	System.out.println(username+ ":" + stripped_message);
                            sendMessageToAll(username, username + ": " + stripped_message ,false);
                    	}else if(tok.length > 2 && writers.containsKey(tok[1])){
                    		//remove the first two words from the message, since they were the command sent to the server
                    		stripped_message = message.split("send "+tok[1])[1];
                    		System.out.println(username+ " (to " + tok[1] + "): " + stripped_message);
                    		//get the specific user to whom the message was intended and send them the message
                    		writers.get(tok[1]).println(username + ": " + message.split("send "+tok[1])[1]);
                    	}
                    }else if(message.startsWith("logout")){
                    	//set the logged in value for this username to false
                    	user_logged.put(username, false);
                    	//remove the user from the writers objects
                    	writers.get(username).println(SERVER_LOGOUT_MESSAGE);
                    	//send message to all except the sender node
                        sendMessageToAll(username, username + " left" , true);
                    	writers.remove(username);
                    	//close the user's socket
                    	socket.close();
                    	System.out.println(username + " logout");
                    }else if(message.startsWith("who")){
                    	Iterator it2 = user_logged.entrySet().iterator();
                    	StringBuilder logged_in_names = new StringBuilder();
                    	
                    	//build the list of active users
                    	while (it2.hasNext()) {
                            Map.Entry pairs = (Map.Entry)it2.next();
                            if(((Boolean)pairs.getValue()) == true){
                            	logged_in_names.append(pairs.getKey().toString() + ",");
                            }
                        }
                    	//send message to all
                        sendMessageToAll(username, " " + logged_in_names.toString().replaceAll(",$", "") , false);
                    }
                }
            } catch (IOException e) {
            } finally {
                // This client is going down!  Remove its name and its print
                // writer from the sets, and close its socket.
                if (out != null) {
                    writers.remove(out);
                }
                try {
                    socket.close();
                } catch (IOException e) {
                }
            }
        }
    }
    
    /**
     * 
     * @param username 		- a String value that holds the username of the client that is sending the message
     * @param message		- the message to be sent	
     * @param ignore_sender	- a flag that if set to true, means to ignore the sender, otherwise, transmit to all clients
     */
    private static void sendMessageToAll(String username, String message, boolean ignore_sender){
    	Iterator it = writers.entrySet().iterator();
        while (it.hasNext()) {
            Map.Entry pairs = (Map.Entry)it.next();
            //print only to the other users, or print to all depending on the flag
            if(!pairs.getKey().equals(username) || ignore_sender == false)
            	((PrintStream)pairs.getValue()).println(message);
        }
    }
}