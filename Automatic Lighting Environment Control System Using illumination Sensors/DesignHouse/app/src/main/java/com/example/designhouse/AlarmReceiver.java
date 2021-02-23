package com.example.designhouse;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class AlarmReceiver extends BroadcastReceiver {
    static String TAG="AlarmReceiver";
    String mode;
    String point;

    @Override
    public void onReceive(Context context, Intent intent) {

        Toast.makeText(context, "알람~!!", Toast.LENGTH_SHORT).show();
        boolean[] week = intent.getBooleanArrayExtra("weekday");


        Log.d(TAG,week.toString());

        Calendar cal = Calendar.getInstance();
        Log.d(TAG,cal.get(Calendar.DAY_OF_WEEK)+"");

        if (!week[cal.get(Calendar.DAY_OF_WEEK)]) return; // 체크한 요일이 아니면

        SimpleDateFormat format1 = new SimpleDateFormat ( "HH:mm");
        Date time = new Date();
        String time1 = format1.format(time);

        Intent intent4= new Intent(context, MainActivity.class);
        point = "1";
        intent4.putExtra("bluetooth", point);
        mode = "S";
        intent4.putExtra("value", mode);
        intent4.putExtra("setDay",cal.get(Calendar.DAY_OF_WEEK));
        intent4.putExtra("setTime",time1.toString());
        intent4.setFlags(intent4.FLAG_ACTIVITY_NEW_TASK);
        context.startActivity(intent4);


        Log.e("TAG","알람입니다~!!"+"알람 요일"+cal.get(Calendar.DAY_OF_WEEK)+" 알람 울린 시간  : "+time1);
        Toast.makeText(context, "알람~!!", Toast.LENGTH_SHORT).show();

    }// onReceive()..


}// Alarm class..