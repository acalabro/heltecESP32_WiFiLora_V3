package it.cnr.isti.labsedc.transponder.blockchain;

import java.util.Date; 

public class Block { 
   
    // Every block contains 
    // a hash, previous hash and 
    // data of the transaction made 
    public String hash; 
    public String previousHash; 
    private String data; 
    private long timeStamp; 
   
    // Constructor for the block 
    public Block(String data, 
                 String previousHash) 
    { 
        this.data = data; 
        this.previousHash 
            = previousHash; 
        this.timeStamp 
            = new Date().getTime(); 
        this.hash 
            = calculateHash(); 
    } 
   
    // Function to calculate the hash 
    public String calculateHash() 
    { 
        String calculatedhash 
            = Crypt.sha256( 
                previousHash 
                + Long.toString(timeStamp) 
                + data); 
   
        return calculatedhash; 
    } 
} 