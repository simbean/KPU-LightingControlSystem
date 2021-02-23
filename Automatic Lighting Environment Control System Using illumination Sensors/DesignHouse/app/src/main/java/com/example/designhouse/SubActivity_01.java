package com.example.designhouse;

import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.Switch;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class SubActivity_01 extends AppCompatActivity {
    String mode;
    String point;// 블루투스 어댑터
    private long time = 0;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sub_01);

        Switch sw = (Switch) findViewById(R.id.switch1);
        Switch sw2 = (Switch) findViewById(R.id.switch2);
        Switch sw3 = (Switch) findViewById(R.id.switch3);
        Switch sw4 = (Switch) findViewById(R.id.switch4);

        sw.setOnCheckedChangeListener(new OnCheckedChangeListener() {


            @Override

            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) { //COOLING 스위치 함수

                // TODO Auto-generated method stub


                if(isChecked) { // 눌리면
                    mode = "C"; //mode를 cooling으로 바꿈
                    point = "1";    //main문의 bluetoothmanage를 1로 만들어 바로 정보를 전송하기 위해
                } else {
                    mode = "NULL";
                }
                if(mode == "C") { //mode가 C면 , 즉 스위치가 눌리면
                    Intent intent = new Intent(SubActivity_01.this, com.example.designhouse.MainActivity.class);
                    //Toast.makeText(SubActivity_01.this, "Cooling모드 " + isChecked +" "+point + mode, Toast.LENGTH_SHORT).show();
                    intent.putExtra("value",mode); //모드를 매인액티비티로 전송
                    intent.putExtra("bluetooth",point); //한번 이상 이동했다는 블루투스 플래그를 전송
                    startActivity(intent); //메인 액티비티로 돌아감
                    finish();
                }
            }

        });
        //CARE, HEALING, SLEEP 함수 모두 COOLING과 로직이 동일하다.
        sw2.setOnCheckedChangeListener(new OnCheckedChangeListener() {


            @Override

            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) { //care 스위치 함수

                if(isChecked) {
                    mode = "R";//mode를 care로 바꿈
                    point = "1";
                } else {
                    mode = "NULL";
                }
                if(mode == "R") {
                    Intent intent = new Intent(SubActivity_01.this, com.example.designhouse.MainActivity.class);
                    //Toast.makeText(SubActivity_01.this, "Care모드 " + isChecked +" " + mode, Toast.LENGTH_SHORT).show();
                    intent.putExtra("value",mode);
                    intent.putExtra("bluetooth",point);
                    startActivity(intent);
                    finish();
                }
            }

        });

        sw3.setOnCheckedChangeListener(new OnCheckedChangeListener() {


            @Override

            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) { //healing 스위치 함수
                // TODO Auto-generated method stub
                if(isChecked) {
                    mode = "H";//mode를 healing으로 바꿈
                    point = "1";
                } else {
                    mode = "NULL";
                }
                if(mode == "H") {
                    Intent intent = new Intent(SubActivity_01.this, com.example.designhouse.MainActivity.class);
                    //Toast.makeText(SubActivity_01.this, "Healing모드 " + isChecked +" " + mode, Toast.LENGTH_SHORT).show();
                    intent.putExtra("value",mode);
                    intent.putExtra("bluetooth",point);
                    startActivity(intent);
                    finish();
                }

            }
        });

        sw4.setOnCheckedChangeListener(new OnCheckedChangeListener() {


            @Override

            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) { //sleep 스위치 함수

                // TODO Auto-generated method stub
                if(isChecked) {
                    mode = "S";//mode를 sleep으로 바꿈
                    point = "1";
                } else {
                    mode = "NULL";
                }
                if(mode == "S") {
                    Intent intent = new Intent(SubActivity_01.this, com.example.designhouse.MainActivity.class);
                    //Toast.makeText(SubActivity_01.this, "Sleep모드 " + isChecked +" " + mode, Toast.LENGTH_SHORT).show();
                    intent.putExtra("value",mode);
                    intent.putExtra("bluetooth",point);
                    startActivity(intent);
                    finish();
                }

            }

        });

    }
    @Override
    public void onBackPressed() { //두번 이상 눌리면 종료하는 함수
        if (System.currentTimeMillis() - time >= 2000) {
            time = System.currentTimeMillis();
            Toast.makeText(getApplicationContext(), "뒤로 버튼을 한번 더 누르면 종료합니다.", Toast.LENGTH_SHORT).show();
        } else if (System.currentTimeMillis() - time < 2000) { //2초이내로 눌리면
            finish();// 종료
        }
    }
}








