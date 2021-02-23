package com.example.designhouse;

        import androidx.appcompat.app.AlertDialog;
        import androidx.appcompat.app.AppCompatActivity;

        import android.bluetooth.BluetoothDevice;
        import android.bluetooth.BluetoothSocket;
        import android.content.DialogInterface;
        import android.content.Intent;

        import android.bluetooth.BluetoothAdapter;
        import android.content.Intent;
        import android.os.Bundle;
        import android.os.Handler;
        import android.os.HandlerThread;
        import android.os.Looper;
        import android.os.Message;
        import android.text.TextUtils;
        import android.view.View;
        import android.widget.Toast;
        import android.os.Bundle;
        import android.view.View;
        import android.widget.Button;
        import android.widget.EditText;
        import android.widget.TextView;
        import android.widget.Toast;

        import java.io.IOException;
        import java.io.InputStream;
        import java.io.OutputStream;
        import java.io.UnsupportedEncodingException;
        import java.util.ArrayList;
        import java.util.List;
        import java.util.Set;
        import java.util.UUID;




public class MainActivity extends AppCompatActivity {
    private String val;
    private String val2;
    private static String mode;
    private static String light;
    private static String blind;
    private static String L_lev;
    private static String B_lev;
    private static String OnOff;
    public static int btcon = 0;
    private static String colorTemp = "c";
    private String writeblind = "0"; //수동모드 활성화시 블라인드 상태 텍스트
    private String writelight; //수동모드 활성화시 조명 상태 텍스트
    private String txstring = "연동 X" ; //현재 모드 상태 텍스트
    private static boolean trigger;
    private long time = 0;
    static boolean first = true;
    private static String bluetoothManage;
    private static final int REQUEST_ENABLE_BT = 10; // 블루투스 활성화 상태
    private int pariedDeviceCount = 0;
    private static BluetoothAdapter bluetoothAdapter; // 블루투스 어댑터
    private static Set<BluetoothDevice> devices; // 블루투스 디바이스 데이터 셋
    private static BluetoothDevice bluetoothDevice; // 블루투스 디바이스
    static public BluetoothSocket bluetoothSocket = null; // 블루투스 소켓
    private static OutputStream outputStream = null; // 블루투스에 데이터를 출력하기 위한 출력 스트림
    private static InputStream inputStream = null; // 블루투스에 데이터를 입력하기 위한 입력 스트림
    private Thread workerThread = null; // 문자열 수신에 사용되는 쓰레드
    private byte[] readBuffer; // 수신 된 문자열을 저장하기 위한 버퍼
    private int readBufferPosition; // 버퍼 내 문자 저장 위치
    private TextView textViewReceive; // 수신 된 데이터를 표시하기 위한 텍스트 뷰
    private EditText editTextSend; // 송신 할 데이터를 작성하기 위한 에딧 텍스트
    private Button buttonSend; // 송신하기 위한 버튼
    private static int numBT = 0;
    private static String text;
    Handler BThan; //화면 갱신 핸들러
    String pt = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final TextView txBT = (TextView) findViewById(R.id.textViewBT); //상단 왼쪽의 블루투스 연결 여부
        final TextView txBT2 = (TextView) findViewById(R.id.textViewBT2); //하단 왼쪽의 작품으로부터 받아지는 텍스트
        final TextView tx1 = (TextView) findViewById(R.id.textView8); // 현재 모드 디스플레이 텍스트
        final TextView tx2 = (TextView) findViewById(R.id.textView9); // 수동모드(블라인드) 디스플레이 텍스트
        final TextView tx3 = (TextView) findViewById(R.id.textViewA); // 수동모드(조명) 디스플레이 텍스트
        final TextView tx4 = (TextView) findViewById(R.id.textView10); // 현재 실내조도 디스플레이 텍스트
        final TextView tx5 = (TextView) findViewById(R.id.textView11); //현재 외부조도 디스플레이 텍스트
        Intent intent0 = getIntent(); //액티비티간 통신하는 intent
        Intent intent1 = getIntent(); //액티비티간 통신하는 intent
        String bluetooth = intent0.getStringExtra("bluetooth");
        if (first == true) {
            bluetoothManage = "0";  //bluetoothManage 한번이라도 다른 액티비티를 간다면 1 아니면 0
        } else { //다른 액티비티를 갔다왔으면, 갱신된 모드, 수동모드 여부, 수동모드시 blind, 조명 단계, 색온도를 가져옴
            bluetoothManage = bluetooth;
            mode = intent0.getStringExtra("value"); // 받은 데이터 : 현재 모드
            blind = intent1.getStringExtra("blind");// 받은 데이터: 수동 블라인드 밝기
            light = intent1.getStringExtra("light");// 받은 데이터: 수동 조명 밝기
            colorTemp = intent1.getStringExtra("colorTemp"); //받은 데이터 : 색온도
            trigger = intent0.getBooleanExtra("OnOff", trigger); //받은 데이터 : ONOFF모드
        }
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter(); // 블루투스 어댑터를 디폴트 어댑터로 설정
        if (bluetoothManage.equals("1")) {  //한번이라도 다른 액티비티를 간 후 돌아왔을 때 그 때 보내야하는 데이터를 보냄
            sendData();
        }
        if (bluetoothManage.equals("0")) {  //최초로 켜질 때 페어링을 수행함
            setBT();
        }
        BThan=  new Handler(Looper.getMainLooper()){ //핸들러: 메인 액티비티에서 보여지는 화면을 실시간으로 갱신해주기 위해 생성
            @Override
            // writeblind, writelight,txstring은 만들어진 텍스트 함수로서 밑의 write_mode()함수에서 만들어짐
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                if(btcon == 1){ //블루투스 현재 연결시(상단의 왼쪽에 표시)
                    txBT.setText("블루투스 연결됨 : " + bluetoothDevice);
                }
                if(btcon == 0){ //블루투스 현재 연결 아닐시
                    txBT.setText("블루투스가 연결되지 않았습니다");
                }
                //여기부턴 블루투스가 연결되있을 때의 행동
                if(msg.arg2 == 1 && btcon == 1){ //수동모드가 켜져있을 때
                    trigger = true;
                    tx2.setText("blind : " + writeblind + "단계");
                    tx3.setText("light  : " + writelight + "단계");
                }
                else //그렇지 않을 때는 수동모드의 단계를 보여주지 않음
                {
                    trigger = false;
                    tx2.setText("");
                    tx3.setText("");
                }

                if(val != null && btcon == 1){ //모드가 NULL이 아닐때
                    write_mode();
                    tx1.setText(txstring);
                    //txBT2.setText("실내조도" + val + "외부조도" + val2 + "모드"+ mode + "L"+L_lev + "B" + B_lev+ "수동"+ OnOff);
                    txBT2.setText("  " + val + "  " + val2 + "  "+ mode + "  "+L_lev + "  " + B_lev+ "  "+ OnOff);
                    tx4.setText("실내조도:   "+val +"LUX");
                    tx5.setText("외부조도:   "+val2+"LUX");
                }
                else{ //연결이 안되있을 때
                    tx1.setText("작품과 연동안됨");
                    tx4.setText("실내조도:   ");
                    tx5.setText("외부조도:   ");
                    txBT2.setText("송신 도착 안함");
                }
            }
        };
        receiveData(); //맨 마지막에는 정보를 보냄을 수행행
   }

    @Override
    public void onBackPressed() {// 한번 더 누르면 정료하는 함수
        if (System.currentTimeMillis() - time >= 2000) {
            time = System.currentTimeMillis();
            Toast.makeText(getApplicationContext(), "뒤로 버튼을 한번 더 누르면 종료합니다.", Toast.LENGTH_SHORT).show();
        } else if (System.currentTimeMillis() - time < 2000) { // 눌린지 2초 안에 뒤로 가 눌리면
            finish(); // 종료
        }
    }

    public void onClick01(View v) { //모드 설정 페이지 이동 함수
        if (trigger) { //수동모드가 활성화일시
            Toast.makeText(getApplicationContext(), "먼저 수동모드를 해제해 주세요.", Toast.LENGTH_SHORT).show();
            //Toast.makeText(getApplicationContext(), pariedDeviceCount, Toast.LENGTH_SHORT).show();
        } else { //활성화가 아닐시
            Intent intent_01 = new Intent(getApplicationContext(), SubActivity_01.class);
            //Toast.makeText(MainActivity.this, " bt 0= "+bluetoothManage, Toast.LENGTH_SHORT).show();
            startActivity(intent_01); //activity sub1으로 이동
        }
    }

    public void onClick02(View v) { //수동모드 페이지 이동 함수
        Intent intent_02 = new Intent(getApplicationContext(), SubActivity_02.class);
        intent_02.putExtra("value", mode);
        intent_02.putExtra("OnOff", trigger);
        if(trigger) { //수동모드 활성화시 조명 레벨과 블라인드레벨을 추가로보냄
            intent_02.putExtra("val", L_lev);
            intent_02.putExtra("val2", B_lev);
        }
        //비활성화시 현재의 모드와 OnOff값만 보냄
        intent_02.putExtra("name", "현재 실행중인 모드 :" + mode + "mode"); /*송신*/
        startActivity(intent_02); //activity sub2으로 이동
        //finish();
    }

    public void onClick03(View v) { //시간 설정 페이지 이동 함수
        Intent intent_03 = new Intent(getApplicationContext(), SubActivity_03.class);
        startActivity(intent_03);
    }

    public void onClick04(View v) { //블루투스 연결 버튼 함수
        if (bluetoothSocket.isConnected()) { //현재 소켓이 연결되어있으면
            try {
                bluetoothSocket.close(); //블루투스 연결을 끊음
                btcon = 0; //그와 동시에 블루투스에 관한 상태를 갱신하기 위해 연결상태 0으로 갱신
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        else{ //연결이 안되어있으면
            setBT(); //연결하는 함수로 이동
        }
    }
    public void write_mode() { //txstring, writelight, writeblind
        //현재 모드
        if (TextUtils.equals(mode, "S")) {
            txstring = "  SLEEP";
        } else if (TextUtils.equals(mode, "C")) {
            txstring = "COOLING";
        } else if (TextUtils.equals(mode, "H")) {
            txstring = "HEALING";
        } else if (TextUtils.equals(mode, "R")) {
            txstring = "   CARE";
        } else {
            txstring = "";
        }
        //현재 조명
        if (TextUtils.equals(L_lev, "0")) {
            writelight = "0";
        } else if (TextUtils.equals(L_lev, "2")) {
            writelight = "1";
        } else if (TextUtils.equals(L_lev, "4")) {
            writelight = "2";
        } else if (TextUtils.equals(L_lev, "6")) {
            writelight = "3";
        } else {
            writelight = "";
        }
        //현재 블라인드
        if (TextUtils.equals(B_lev, "0")) {
            writeblind = "0";
        } else if (TextUtils.equals(B_lev, "1")) {
            writeblind = "1";
        } else if (TextUtils.equals(B_lev, "2")) {
            writeblind = "2";
        } else if (TextUtils.equals(B_lev, "3")) {
            writeblind = "3";
        } else {
            writeblind = "";
        }
    }
    public void setBT() { //블루투스연결함수
        if (bluetoothAdapter == null) { // 디바이스가 블루투스를 지원하지 않을 때
            Toast.makeText(this, "블루투스를 지원하지 않는 단말기 입니다.", Toast.LENGTH_SHORT).show();
            finish();
            return;
        } else { // 디바이스가 블루투스를 지원 할 때
            if (bluetoothAdapter.isEnabled()) { // 블루투스가 활성화 상태 (기기에 블루투스가 켜져있음)
                selectBluetoothDevice(); // 블루투스 디바이스 선택 함수 호출
                first = false;  //연결이되면 처음앱을 켰을 때일 경우를 해제함5
            } else { // 블루투스가 비 활성화 상태 (기기에 블루투스가 꺼져있음)
                Toast.makeText(getApplicationContext(), "블루투스가 꺼져있어요.", Toast.LENGTH_SHORT).show();
            }
        }
    }
    public void selectBluetoothDevice() { //블루투스 디바이스 선택 함수 호출
        // 이미 페어링 되어있는 블루투스 기기를 찾습니다.
        devices = bluetoothAdapter.getBondedDevices();
        // 페어링 된 디바이스의 크기를 저장
        pariedDeviceCount = devices.size();
        // 페어링 되어있는 장치가 없는 경우
        if (pariedDeviceCount == 0) {
            // 페어링을 하기위한 함수 호출
        }
        // 페어링 되어있는 장치가 있는 경우
        else {
            // 디바이스를 선택하기 위한 다이얼로그 생성
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("페어링 되어있는 블루투스 디바이스 목록");
            // 페어링 된 각각의 디바이스의 이름과 주소를 저장
            List<String> list = new ArrayList<>();
            // 모든 디바이스의 이름을 리스트에 추가
            for (BluetoothDevice bluetoothDevice : devices) {
                list.add(bluetoothDevice.getName());
            }
            list.add("취소");
            // List를 CharSequence 배열로 변경
            final CharSequence[] charSequences = list.toArray(new CharSequence[list.size()]);
            list.toArray(new CharSequence[list.size()]);
            // 해당 아이템을 눌렀을 때 호출 되는 이벤트 리스너
            builder.setItems(charSequences, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    // 해당 디바이스와 연결하는 함수 호출
                    connectDevice(charSequences[which].toString());
                }
            });

            // 다이얼로그 생성
            AlertDialog alertDialog = builder.create();
            alertDialog.show();
        }
    }

    public void connectDevice(String deviceName) {
        // 페어링 된 디바이스들을 모두 탐색

        for (BluetoothDevice tempDevice : devices) {
            // 사용자가 선택한 이름과 같은 디바이스로 설정하고 반복문 종료
            if (deviceName.equals(tempDevice.getName())) {
                bluetoothDevice = tempDevice;
                break;
            }
        }
        // UUID 생성
        UUID uuid = java.util.UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");
        // Rfcomm 채널을 통해 블루투스 디바이스와 통신하는 소켓 생성
        try {
            bluetoothSocket = bluetoothDevice.createRfcommSocketToServiceRecord(uuid);
            bluetoothSocket.connect();
            // 데이터 송,수신 스트림을 얻어옴
            outputStream = bluetoothSocket.getOutputStream();
            inputStream = bluetoothSocket.getInputStream();
            btcon = 1;
            // 데이터 수신 함수 호출
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void receiveData() {
        // 데이터를 수신하기 위한 버퍼를 생성
        readBufferPosition = 0;
        readBuffer = new byte[1024];

        // 데이터를 수신하기 위한 쓰레드 생성
        workerThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (true) { //쓰레드는 계속 실행됨됨
                   Message msg = BThan.obtainMessage();
                    if(bluetoothSocket != null ) { //블루투스에 연결되었을 때
                        if (btcon == 1) {
                            msg.arg1 = 1; //msg.arg1 =
                        }
                    }
                        else {
                        msg.arg1 = 0;
                        msg.arg2 = 0;
                    }
                    if (!Thread.currentThread().isInterrupted() && btcon == 1) { //블루투스 연결된 상태에서 쓰레드 인터럽트가 발생하지 않은 경우
                        try {
                            int bytesAvailable = inputStream.available();    // 수신 데이터 확인
                            if (bytesAvailable > 0) {                     // 데이터가 수신된 경우
                                byte[] packetBytes = new byte[bytesAvailable];
                                inputStream.read(packetBytes);
                                for (int i = 0; i < bytesAvailable; i++) {
                                    byte b = packetBytes[i];
                                    if (b == 'n') {
                                        msg.arg2 = 1;
                                        byte[] encodedBytes = new byte[readBufferPosition];
                                        System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                        try {
                                            pt = text;
                                            text = new String(encodedBytes, "US-ASCII"); //string으로 바꾸어줌
                                        } catch (UnsupportedEncodingException e) {
                                            e.printStackTrace();
                                        }
                                        readBufferPosition = 0;
                                    } else {
                                        readBuffer[readBufferPosition++] = b;
                                    }
                                }
                            }
                        } catch (IOException e) {
                            e.printStackTrace();
                        }

                        if (TextUtils.equals(OnOff, "1")) { //Onoff 플래그가 1일때 -> 수동모드 활성화
                            msg.arg2 = 1; //msg.arg2을 1로 맞추어줌
                        } else {
                            msg.arg2 = 0;
                        }
                        try {
                            // 0.2초마다 받아옴

                            Thread.sleep(200);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        if (!(TextUtils.equals(text, pt))) { //text가 갱신되었을 때(과거 상태와 현재 상태가 다를때)
                            int point1 = 0;
                            int point2 = 0;
                            int point3 = 0;
                            int point4 = 0;
                            int point5 = 0;
                            int point6 = 0;
                            //작품에서 받아오는 코드는
                            //sprintf(s1, "a%db%dc%cd%de%df%dn", val, val2, a, B_lev, L_lev, OnOff); 꼴이기 때문에
                            //이것을 변환해줌
                            //392~440까지는 a,b,c,d,e,f,n을 만나면 순차적으로 point에 따라 다음 단계로 넘어가고
                            //그 전까지는 문자를 만나기전 까지 해당된 값을 더함(point1 == 1일때는 b를 만나기 전까지 val의 문자열에 추가
                            for (int i = 0; i < text.length(); i++) {//392
                                if (point1 == 1 && text.charAt(i) != 'b') { //실내조도
                                    val += text.charAt(i);
                                }
                                if (point2 == 1 && text.charAt(i) != 'c') { //외부조도
                                    val2 += text.charAt(i);
                                }
                                if (point3 == 1 && text.charAt(i) != 'd') { //모드
                                    mode += text.charAt(i);
                                }
                                if (point4 == 1 && text.charAt(i) != 'e') { //수동모드 시의 블라인드 단계
                                    B_lev += text.charAt(i);
                                }
                                if (point5 == 1 && text.charAt(i) != 'f') {//수동모드 시의 조명 단계
                                    L_lev += text.charAt(i);
                                }
                                if (point6 == 1 && text.charAt(i) != 'n') { //수동모드 여부
                                    OnOff += text.charAt(i);
                                }

                                if (text.charAt(i) == 'a') {
                                    point1 = 1;
                                    val = "";
                                } else if (text.charAt(i) == 'b') {
                                    point1 = 0;
                                    point2 = 1;
                                    val2 = "";
                                } else if (text.charAt(i) == 'c') {
                                    point2 = 0;
                                    point3 = 1;
                                    mode = "";
                                }else if (text.charAt(i) == 'd') {
                                    point3 = 0;
                                    point4 = 1;
                                    B_lev = "";
                                }
                                else if (text.charAt(i) == 'e') {
                                    point4 = 0;
                                    point5 = 1;
                                    L_lev = "";
                                }
                                else if (text.charAt(i) == 'f') {
                                    point5 = 0;
                                    point6 = 1;
                                    OnOff = "";
                                }
                                else if (text.charAt(i) == 'n') {
                                    point6 = 0;
                                }//440
                            }
                        }
                        BThan.sendMessage(msg);
                    }
                }
            }
        });
        workerThread.start();
    }

    void sendData() {//문자를 보내는 함수
        // 문자열에 개행문자("\n")를 추가해줍니다.
        try {
            if (mode != null && !(light != null || blind != null)) { //모드 값이 NULL 이 아닐때 + 수동모드 비활성화시
                outputStream.write(mode.getBytes());//작품에 문자를 보냄
            }
            if (colorTemp != null) { //색온도 값이 NULL이 아닐때
                outputStream.write(colorTemp.getBytes());
            }
            if (light != null) { //조명 값이 NULL이 아닐때
                if (!light.equals("n")) {
                    outputStream.write(light.getBytes());
                }
            }
            if (blind != null) { //블라인드 값이 NULL이 아닐때
                if (!blind.equals("n")) {
                    outputStream.write(blind.getBytes());
                }
            }
            Toast.makeText(MainActivity.this, "정보를 저장했어요" + mode + light + blind + colorTemp, Toast.LENGTH_SHORT).show();
        } catch (Exception e) {
            Toast.makeText(MainActivity.this, "에러", Toast.LENGTH_SHORT).show();
            e.printStackTrace();
        }
    }
}





