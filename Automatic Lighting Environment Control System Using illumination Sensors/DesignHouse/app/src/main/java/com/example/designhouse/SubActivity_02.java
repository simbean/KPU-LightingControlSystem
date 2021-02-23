package com.example.designhouse;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.appcompat.app.AppCompatActivity;

public class SubActivity_02 extends AppCompatActivity {
    static String mode;
    static boolean trigger = false;
    static String blind = "0"; // 수동 모드 단계
    static String Light = "4"; // 수동 모드 단계
    static String L_lev = ""; //조명 레벨
    static int L_lv = 0;
    static String B_lev = ""; //블라인드 레벨
    static int B_lv = 0;
    private long time = 0;
    private static String modew = "L"; //색온도 상태
    TextView textView;
    TextView textView2;
    ImageView imageview = null;
    ImageView imageview2 = null; //
    ToggleButton toggleButton;
    static String point = "0";// 블루투스 어댑터
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sub_02);
        toggleButton = (ToggleButton)findViewById(R.id.chkState);
        SharedPreferences pref = getSharedPreferences("pref", AppCompatActivity.MODE_PRIVATE);
        Intent intent02 = getIntent();
        mode = intent02.getStringExtra("value");
        L_lev = intent02.getStringExtra("val");
        B_lev = intent02.getStringExtra("val2");
        trigger = intent02.getBooleanExtra("OnOff", trigger);
        final String name = intent02.getStringExtra("name");
        final Switch sw5 = (Switch) findViewById(R.id.switch5); //블루투스 활성화 여부
        Boolean s5 = pref.getBoolean("switch5", trigger);
        final ToggleButton TB1 = (ToggleButton)findViewById(R.id.chkState);
        Boolean T1 = pref.getBoolean("chkState", false);
        final TextView tx1 = (TextView)findViewById(R.id.textView8); /*TextView선언*/
        level();
        textView = findViewById(R.id.textView);
        textView2 = findViewById(R.id.textView2);
        final SeekBar seekBar = findViewById(R.id.seekBar3); //블라인드 seekbar
        final SeekBar seekBar2 = findViewById(R.id.seekBar); //조명 seekbar
        imageview = (ImageView)findViewById(R.id.imageView);
        imageview2= (ImageView)findViewById(R.id.imageView2);
        image();
        textView.setText("커튼 설정 : "+B_lv+ "단계");
        textView2.setText("조명 설정 : "+L_lv+ "단계");
        seekBar.setProgress(B_lv);
        seekBar2.setProgress(L_lv);
        TB1.setChecked(T1);
        if(trigger) { //트리거가 1일때는 수동모드가 활성화되어있는 것
            sw5.setChecked(trigger); //활성화되어있음을 표시
            tx1.setText("현재 실행중인 모드 : 수동설정모드");
        }
        else{ //둘다 꺼져있을 때는 블라인드, 조명 단계 0으로 설정
            Light = "4";
            blind = "0";
            seekBar.setProgress(0);
            seekBar2.setProgress(0);
            tx1.setText(name);
        }
        toggleButton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked == true) {
                    modew = "W";
                        } else {
                    modew = "L";
                }
            }
        });


        sw5.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {



            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {

                if (isChecked) {
                    //Toast.makeText(SubActivity_02.this, "수동설정모드 " + isChecked + " " + mode, Toast.LENGTH_SHORT).show();
                    trigger = true;
                    tx1.setText("현재 실행중인 모드 : 수동설정모드");
                } else {
                    trigger = false;
                    seekBar.setProgress(0);
                    seekBar2.setProgress(0);
                    tx1.setText(name);
                    //Toast.makeText(SubActivity_02.this, "수동설정모드 " + isChecked + " " + mode, Toast.LENGTH_SHORT).show();
                }
            }
        });

//


//
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {  //SeekBar Listener 리스너설정
            @Override
            public void onProgressChanged(SeekBar seekBar, int i , boolean b) {

                textView.setText("커튼 설정 : "+i+ "단계"); //식바를 움직이면 움직인 정도에 따라 단계를 맞춤
                if(i == 0) {
                    blind = "0";
                    image();
                }
                if(i == 1) {
                    blind = "1";
                    image();
                }
                if(i == 2)
                {
                    blind = "2";
                    image();
                }
                if(i == 3)
                {
                    blind = "3";
                    image();
                }
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar3) {

            }
            @Override
            public void onStopTrackingTouch(SeekBar seekBar3){

            }
        });

        seekBar2.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {  //SeekBar Listener 리스너설정
            @Override
            public void onProgressChanged(SeekBar seekBar2, int i , boolean b) {

                textView2.setText("조명 설정 : "+ i+ "단계"); //식바를 움직이면 움직인 정도에 따라 단계를 맞춤
                if(i == 0) {
                    Light = "4";
                    image();
                }

                if(i == 1) {
                    Light = "5";
                    image();
                }
                if(i == 2)
                {
                    Light = "6";
                    image();
                }
                if(i == 3)
                {
                    Light = "7";
                    image();
                }
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }
            @Override
            public void onStopTrackingTouch(SeekBar seekBar){

            }
        });



    }
    @Override
    public void onBackPressed() { //두번이상 눌리면 종료
        if (System.currentTimeMillis() - time >= 2000) {
            time = System.currentTimeMillis();
            Toast.makeText(getApplicationContext(), "뒤로 버튼을 한번 더 누르면 종료합니다.", Toast.LENGTH_SHORT).show();
        } else if (System.currentTimeMillis() - time < 2000) {
            finish();
        }
    }

    public void level() { //블루투스로 받아온 레벨 확인하는 함수
        //스트링을 int형의 자료로 변환하는 함수이다.
            if(TextUtils.equals(B_lev, "0")){
                B_lv = 0;
            }
            else if(TextUtils.equals(B_lev, "1")){
                B_lv = 1;
            }
            else if(TextUtils.equals(B_lev, "2")){
                B_lv = 2;
            }
            else if(TextUtils.equals(B_lev, "3")){
                B_lv = 3;
            }
            if(TextUtils.equals(L_lev, "0")){
                L_lv = 0;
            }
            else if(TextUtils.equals(L_lev, "2")){
                L_lv = 1;
            }
            else if(TextUtils.equals(L_lev, "4")){
                L_lv = 2;
            }
            else if(TextUtils.equals(L_lev, "7")){
                L_lv = 3;
            }
        }
        public void image(){ //레벨 변환에 따른 이미지 변환
        //커튼과 , 조명의 레벨이 바뀔 때 그림을 변환해주는 함수이다.
        if(TextUtils.equals(blind, "0")){
            imageview.setImageResource(R.drawable.curtain78);
        }
        else if(TextUtils.equals(blind, "1")){
            imageview.setImageResource(R.drawable.curtain6);
        }
        else if(TextUtils.equals(blind, "2")){
            imageview.setImageResource(R.drawable.curtain55);
        }
        else if(TextUtils.equals(blind, "3")){
            imageview.setImageResource(R.drawable.curtain5);
        }
        if(TextUtils.equals(Light, "4")){
            imageview2.setImageResource(R.drawable.light1);
        }
        else if(TextUtils.equals(Light, "5")){
            imageview2.setImageResource(R.drawable.light2);
        }
        else if(TextUtils.equals(Light, "6")){
            imageview2.setImageResource(R.drawable.light3);
        }
        else if(TextUtils.equals(Light, "7")){
            imageview2.setImageResource(R.drawable.light4);
        }
    }
    public void onClickMode(View v)
    {
        //저장버튼 구현함수
        point = "1";    //정보를 바로 보내주기 위하여
        Intent intent = new Intent(SubActivity_02.this, com.example.designhouse.MainActivity.class);
        intent.putExtra("bluetooth",point);
        intent.putExtra("OnOff", trigger);
        if(trigger == true) {       //모드 => 이전 모드 기억하기 위해서, blind,light 수동모드, trigger/ 이전 트리거값기억하기 위해서
            intent.putExtra("value", mode);
            intent.putExtra("colorTemp", modew);
            intent.putExtra("blind", blind);
            intent.putExtra("light", Light);
        }
        else{ //수동모드 비활성화시에는 모드만 보낸다
            intent.putExtra("value", mode);
        }
        startActivity(intent); //메인 액티비티로 이동
        finish();
    }
    public void onStop(){ // 어플리케이션이 화면에서 사라질때
        super.onStop();
        SharedPreferences pref = getSharedPreferences("pref", AppCompatActivity.MODE_PRIVATE); // UI 상태를 저장합니다.
        SharedPreferences.Editor editor = pref.edit(); // Editor를 불러옵니다.


        Switch sw5 = (Switch) findViewById(R.id.switch5);
        ToggleButton TB1 = (ToggleButton)findViewById(R.id.chkState);
        textView = findViewById(R.id.textView);
        textView2 = findViewById(R.id.textView2);
        SeekBar seekBar = findViewById(R.id.seekBar3);
        SeekBar seekBar2 = findViewById(R.id.seekBar);

        editor.putBoolean("switch5", sw5.isChecked());
        editor.putBoolean("chkState",TB1.isChecked());
        editor.putString("textView", textView.getText().toString());
        editor.putString("textView2", textView2.getText().toString());
        editor.putInt("seekBar3", seekBar.getProgress());
        editor.putInt("seekBar", seekBar2.getProgress());

        editor.commit(); // 저장합니다.
    }
}