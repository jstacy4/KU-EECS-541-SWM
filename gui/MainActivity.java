package com.example.android_battery_query;

import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.util.concurrent.TimeUnit;
import org.apache.commons.net.telnet.TelnetClient;
import android.annotation.TargetApi;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.StrictMode;
import android.text.Editable;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends Activity implements OnClickListener
{
	TextView text;
	EditText edit1, edit2;
	Editable server, command;
	private TelnetClient telnet = new TelnetClient();
	private InputStream in;
	private PrintStream out;
	StringBuffer sb;
	Handler mHandler = new Handler();
	int len;

	@TargetApi(9)
	@Override
	public void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		text = (TextView)findViewById(R.id.text);
		edit1 = (EditText)findViewById(R.id.edit1);
		edit2 = (EditText)findViewById(R.id.edit2);
		server = edit1.getEditableText();
		command = edit2.getEditableText();
		Button button = (Button)findViewById(R.id.button);
		button.setOnClickListener(this);
		text.setText("Android Socket" + "\n");
		
		StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
	    StrictMode.setThreadPolicy(policy);
	}
	
	@Override
	public void onClick(View arg0) 
	{
		// TODO Auto-generated method stub
		text.setText("Android Socket" + "\n");
		
		try 
		{
			telnet.connect(server.toString(), 23);
			in = telnet.getInputStream();
			out = new PrintStream(telnet.getOutputStream());
			telnet.setKeepAlive(true);
			Thread mThread = new Thread(new Runnable() 
			{
				@Override
				public void run() 
				{
					// TODO Auto-generated method stub
					try 
					{
						sb = new StringBuffer();
						while (true)
						{
							len = in.read();
							String s = Character.toString((char)len);
							sb.append( s );
							MainActivity.this.mHandler.post(new Runnable()
							{
								@Override
								public void run() {
									// TODO Auto-generated method stub
									MainActivity.this.text.getText();
									MainActivity.this.text.setText( sb.toString() );
									//MainActivity.this.text.append( sb.toString() );
								}
							});
							System.out.println( sb );
							mycommand();
						}
					} 
					catch (IOException e) 
					{
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			});
			mThread.start();
		}
		catch (Exception e) 
		{
			e.printStackTrace();
		}
	}
	
	private void mycommand() throws IOException 
	{
		// FIXME Determine how to receive response from sent command.
		if (sb.toString().endsWith("# ")) 
		{
			out.println(command.toString() + "\r\n");
			out.flush();
//			out.println("exit\r\n");
//			out.flush();
			try 
			{
				TimeUnit.SECONDS.sleep(10);
			} 
			catch (InterruptedException e) 
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			// If this is commented out the command is sent repeatedly.
			disconnect();			
		}
	}	
	
	public void disconnect() 
	{
		try 
		{
			in.close();
			out.close();
			telnet.disconnect();
		}
	
		catch (Exception e) 
		{
			e.printStackTrace();
		}
	}
}