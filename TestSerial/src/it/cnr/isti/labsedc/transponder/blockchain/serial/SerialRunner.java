package it.cnr.isti.labsedc.transponder.blockchain.serial;

import com.fazecast.jSerialComm.SerialPort;

public class SerialRunner {

	public void main(String[] args) {
		SerialPort[] commPorts;
		commPorts = SerialPort.getCommPorts();
		
		for (int i = 0; i<commPorts.length; i++) {
			System.out.println(commPorts[i].getSystemPortName());
		}
	}
	
	public void ReadFromSerial(SerialPort comPort) {
		comPort.openPort();
		try {
		   while (true)
		   {
		      while (comPort.bytesAvailable() == 0)
		         Thread.sleep(20);

		      byte[] readBuffer = new byte[comPort.bytesAvailable()];
		      int numRead = comPort.readBytes(readBuffer, readBuffer.length);
		      System.out.println("Read " + numRead + " bytes.");
		   }
		} catch (Exception e) { e.printStackTrace(); }
		comPort.closePort();	
	}
}
