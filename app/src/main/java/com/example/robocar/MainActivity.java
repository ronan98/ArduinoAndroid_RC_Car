package com.example.robocar;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    Button speedSubmit, lBlink, rBlink, autopilot, headlights;
    ImageButton speedUp, speedDown, up, down, left, right, horn;
    TextView currentSpeed, btName, btAddress;

    int speedVal = 1;

    String address = null , name = null;

    BluetoothAdapter btAdapter = null;
    BluetoothSocket btSocket = null;


    static final UUID mUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        try {setw();} catch (Exception e) {}

        speedSubmit=(Button)findViewById(R.id.speedSubmit);
        headlights=(Button)findViewById(R.id.headlights);
        autopilot = (Button)findViewById(R.id.autopilot);
        lBlink = (Button)findViewById(R.id.lBlink);
        rBlink = (Button)findViewById(R.id.rBlink);
        speedUp=(ImageButton)findViewById(R.id.speedUp);
        speedDown=(ImageButton)findViewById(R.id.speedDown);
        currentSpeed=(TextView)findViewById(R.id.currentspeedLabel);

        headlights.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendData("h");
            }
        });

        autopilot.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendData("a");
            }
        });

        lBlink.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendData("w");
            }
        });

        rBlink.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendData("e");
            }
        });

        speedSubmit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendData(String.valueOf(speedVal));
            }
        });

        speedUp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                changeSpeed(0);
                currentSpeed.setText(String.valueOf(speedVal));
            }
        });

        speedDown.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                changeSpeed(1);
                currentSpeed.setText(String.valueOf(speedVal));
            }
        });



    }

    @SuppressLint({"ClickableViewAccessability", "ClickableViewAccessibility"})
    private void setw() throws IOException
    {
        currentSpeed=(TextView)findViewById(R.id.currentspeedLabel);
        btAddress=(TextView)findViewById(R.id.btAddress);
        btName=(TextView)findViewById(R.id.btName);
        bluetoothConnect();

        up=(ImageButton)findViewById(R.id.upButton);
        down=(ImageButton)findViewById(R.id.downButton);
        left=(ImageButton)findViewById(R.id.leftButton);
        right=(ImageButton)findViewById(R.id.rightButton);
        horn=(ImageButton)findViewById(R.id.horn);

        up.setOnTouchListener(new View.OnTouchListener()
        {   @Override
            public boolean onTouch(View v, MotionEvent event){
            if(event.getAction()==MotionEvent.ACTION_DOWN) {sendData("u");}
            if(event.getAction()==MotionEvent.ACTION_UP) {sendData("s");}
            return true;}
        });

        down.setOnTouchListener(new View.OnTouchListener()
        {   @Override
        public boolean onTouch(View v, MotionEvent event){
            if(event.getAction()==MotionEvent.ACTION_DOWN) {sendData("d");}
            if(event.getAction()==MotionEvent.ACTION_UP) {sendData("s");}
            return true;}
        });

        left.setOnTouchListener(new View.OnTouchListener()
        {   @Override
        public boolean onTouch(View v, MotionEvent event){
            if(event.getAction()==MotionEvent.ACTION_DOWN) {sendData("l");}
            if(event.getAction()==MotionEvent.ACTION_UP) {sendData("s");}
            return true;}
        });

        right.setOnTouchListener(new View.OnTouchListener()
        {   @Override
        public boolean onTouch(View v, MotionEvent event){
            if(event.getAction()==MotionEvent.ACTION_DOWN) {sendData("r");}
            if(event.getAction()==MotionEvent.ACTION_UP) {sendData("s");}
            return true;}
        });

        horn.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if(event.getAction()==MotionEvent.ACTION_DOWN) {sendData("hon");}
                if(event.getAction()==MotionEvent.ACTION_UP) {sendData("hoff");}
                return true;}
        });
    }


    private void bluetoothConnect() throws IOException
    {
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        BluetoothDevice hc05 = btAdapter.getRemoteDevice("98:D3:C1:FD:6F:FA");
        btSocket = hc05.createInsecureRfcommSocketToServiceRecord(mUUID);
        btSocket.connect();

        address = hc05.getAddress().toString();
        name = hc05.getName().toString();

        try
        {
            btAddress.setText("BT Address: "+address);
            btName.setText("BT Name: "+name);
        } catch(Exception e){}
    }


    private void sendData(String i)
    {
        try {
            if (btSocket!=null)
            {
                btSocket.getOutputStream().write(i.getBytes());
                System.out.println(i);
            }
        }
        catch (Exception e)
        {
            Toast.makeText(getApplicationContext(),e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void changeSpeed(int i)
    {
        if (i == 0) // add 1 to speed
        {
            if(speedVal < 10) { speedVal++; }
        }
        if (i == 1) // minus 1 to speed
        {
            if(speedVal > 1) { speedVal--; }
        }
    }
}