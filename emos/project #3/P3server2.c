#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h> //pthread ���
#define SERV_IP "220.149.128.103" //���� IP
#define SERV_PORT 4538 //���� PORT
#define BACKLOG 10
#define INIT_MSG "==============================\nHello! I'm P2P File \n Please, LOG_IN!\n ==============================\n"
#define Success_MSG "Log-in success!!"
#define Fail_MSG "Log-in fail: Incorrect password..."
#define USER1_ID	"user1"
#define USER1_PW	"passwd1"
#define USER2_ID	"user2"
#define USER2_PW	"passwd2"
#define UI1	"==============================\nUser Information\n"
#define UI2	"==============================\n"
char msg[512]; //�α��� ���� �޽���
char buf[512] = ""; //ä�� ������ char* ���·� ������ ����
char chatbuf[512]; //ä�� ���뿡 Ŭ���̾�Ʈ�� �α����� ���� ����
int writeflag = 0; // chatbuf�� ����� �ٸ� Ŭ���̾�Ʈ�� �����ֱ� ���� ���� flag(0�̸� �غ� �ȵ�, 1�̸� �غ��)
int sendflag = 0;
char sendmessage[512];
int new_fd; 
//char user[50];
int buffer = -1; //��� �����忡�� �����Ǵ� INDEX ��(main()�� ����)
int indexfd = -1; // 3���� [FILE user2]���� ������ �� ���� user2�� � Ŭ���̾�Ʈ���� �α��� �Ǿ����� �Ǻ��ϱ� ���� ������ ����
int add[10]; //main������ �̿��� their_addr.sin_addr.s_addr�� �̿��Ͽ�
//�� IP�� ���´ٸ� �� INDEX�� �߱��Ͽ� �α����� �����ϰ�, �׷��� ������ IP�� ��ϵ� INDEX�� �α����� �����Ѵ�..
pthread_t controlbox; //main������ /help�� ġ�� ���� ������ ������ ������ִ� ������
int recv_t(int socket, char* buf, int buff, int i) { //�帧 ���� ���� ���� �Լ�
	int t;
	t = recv(socket, buf, buff, 0);
	if (t > -1) {
		//printf("recv : %d, %d\n", i, t);//���� ���� ������ ���¸� ������ش�.
	}
	return t;
}
int send_t(int socket, char* buf, int buff, int i) {//�帧 ���� ���� ���� �Լ�
	int t;
	t = send(socket, buf, buff, 0);
	if (t > -1) {
		//printf("send : %d, %d\n", i, t); //���� ���� ������ ���¸� ������ش�.
	}
}
struct client { //Ŭ���̾�Ʈ�� ������ �����ϴ� ����ü
	int new_fd;
	int correctFlag;
	int exitFlag;
	char usingId[20];
	char address[20];
	char PORT[10];
	int waitFlag;
	pthread_t chatview; //ê�� ������ ����ϴ� ������
	pthread_t chatwrite; //ê�� Ŭ���̾�Ʈ�� �����ִ� ������
	pthread_t login; //�α��� �����ϴ� ������
};
struct client c[10]; //Ŭ���̾�Ʈ�� ������ ������� Thread, add�� ������ ���ڿ����Ѵ�.
//�ִ� 10������ ���� ����
void cleanup(void* argu) {
	int socket = *((int*)argu);
	//printf("clean %d\n",socket);
}
void* control(void* argu) { //control �����尡 ������ �Լ�
	char message[50];
	while (1) {
		fgets(message, sizeof(message), stdin); // /help��� ���ڰ� ������ 
		if (strcmp(message, "/help\n") == 0) {
			printf("\n----------control----------\n");
			printf("buffer :\n"); //���� ������ ���� Ŭ���̾�Ʈ�� new_fd
			for (int i = 0; i < 10; i++) {
				printf("%d\t", c[i].new_fd);
			}
			printf("\n");
			printf("address :\n"); //�ּ�(new_fd�� �ٸ��� �ѹ� ������ ���������� �ּҰ� �����)
			for (int i = 0; i < 10; i++) {
				printf("Index %d : %s\n", i, c[i].address);
			}
			printf("port :\n");//PORT(new_fd�� �ٸ��� �ѹ� ������ ���������� �ּҰ� �����)
			for (int i = 0; i < 10; i++) {
				printf("Index %d : %s\n", i, c[i].PORT);
			}
			printf("USINGID :\n"); //Ŭ���̾�Ʈ�� �α����� ���̵� 
			for (int i = 0; i < 10; i++) {
				printf("Index %d : %s\n", i, c[i].usingId);
			}
			//�� ����Ѵ�
		}
	}
	pthread_exit(NULL);
}

int compareWord(char* ID, char* PW, int count) { //�α����� �� �� ���̵�� ��й�ȣ�� ��ġ�ϴ��� �˻��ϴ� �Լ�
	char compareNum1[50];	
	char compareNum2[50];
	char compare[50];
	int flag = 0; //int �� ��ȯ����
	sprintf(compare, "%s/%s", ID, PW);
	sprintf(compareNum1, "%s/%s", USER1_ID, USER1_PW);//user1
	sprintf(compareNum2, "%s/%s", USER2_ID, USER2_PW);//user2
	if (strcmp(compare, compareNum1) == 0) {
		strcpy(c[count].usingId, USER1_ID); //user1�� ��ġ�ϸ� flag = 1 ��ȯ
		flag = 1;
		return flag;
	}
	else if (strcmp(compare, compareNum2) == 0) { //user2�� ��ġ�ϸ� flag = 2 ��ȯ
		strcpy(c[count].usingId, USER2_ID);
		flag = 2;
		return flag;
	}
	else { //��ġ���� ������ flag = 0 ��ȯ
		strcpy(c[count].usingId, ""); 
		flag = 0;
		return flag;
	}
}
char* distingish(int a, char* ID, char* PW) { //�α��� ����, ���� �޽��� ��� �Լ�
	static char str[50] = "";
	sprintf(str, "ID: %s, PW: %s\n", ID, PW);
	if (a == 1) {
		sprintf(msg, "%s [%s] *^^*", Success_MSG, USER1_ID);
	}
	else if (a == 2) {
		sprintf(msg, "%s [%s] *^^*", Success_MSG, USER2_ID);
	}
	else sprintf(msg, "%s", Fail_MSG);
	return str;
}

int sendfile_userfound(int index) { //������ sendfile�� ������ �Լ� � Ŭ���̾�Ʈ�� txt�� ������ �ĺ��ϴ� �Լ�
	int count = index;
	char user[20];
	int senddata = 11;
	//printf("%d", index);
	if (strcmp(buf, "[FILE-user1]\n") == 0) { 
		strcpy(user, USER1_ID);
	}
	else if (strcmp(buf, "[FILE-user2]\n") == 0) { 
		strcpy(user, USER2_ID);
	}
	else {
		strcpy(user, "NOT USER");
	}
	//user�� ������ ã�Ƴ��� if��

	//printf("user: %s\n", user);
	for (int i = 0; i <= buffer; i++) { //���� ��ϵ� Ŭ���̾�Ʈ ������ŭ
		if (strcmp(user, c[i].usingId) == 0) { //���̵� �´��� Ȯ���ϴµ�, ���� Ŭ���̾�Ʈ�� �ؽ�Ʈ ������ �޾ƿ��� ���̹Ƿ�
			if (strcmp(user, c[index].usingId) != 0) { //�ڽ��� �α����� ���̵�� �ȵ�
				senddata = i;  //�ش� Ŭ���̾�Ʈ�� index�� �޾ƿ�
				break;
			}
		}
	}
	return senddata; //�ε��� ��ȯ
}

void* chatview_thread(void* argu) { //������chatview�� ������ �Լ��̸�, ������ ä���� ����ϴ� ����� �ϴ� �Լ�
	int rcv_byte;
	int point = 0;
	int pointcount = 0;
	char message[512];
	char using[20];
	int count = *((int*)argu);
	point = c[count].new_fd; //�ش� Ŭ���̾�Ʈ�� new_fd
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_cleanup_push(cleanup, (void*)&point);//pthread_cancel �ݹ� �Լ� ���
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);//pthread_cancel�� �ٷ� ����
	sendflag = 0;
	//�����带 �α��� ������� �������� ���� �����尡 ����Ǿ �� ������� ������� �ʰ��Ѵ�.
	//printf("hello viewthread %d", c[count].new_fd);
	strcpy(using, c[count].usingId); //using = �ش� Ŭ���̾�Ʈ�� �α��� �Ǿ��� �� ����ϴ� ���̵�
	while (1) {		
		if (c[count].waitFlag == 0) {
			rcv_byte = recv_t(point, buf, sizeof(buf), 8);// i = 8
			if (rcv_byte > -1) { //�������� �ִٸ�
				writeflag = 1; //write flag �� 1�� ��������
				sprintf(chatbuf, "[%s] %s", using, buf); //���� ���̵�� �Բ� ���
				if (strcmp(buf, "\n") != 0) { //������ ������
					printf("%s", chatbuf); //������ ���
				}
				if (strcmp(buf, ":q\n") == 0) { //���� :q�� ������
					c[count].exitFlag = 1; //�ش� Ŭ���̾�Ʈ�� exitflag�� 1�� ����
					printf("bye");
					//pthread_exit(NULL);
				}
				else if (strncmp(buf, "[FILE", 5) == 0) { //���� [FILE�� ���� 5���ڿ� ���ԵǾ��ִٸ�
					indexfd = sendfile_userfound(count);
					if (indexfd > -1 && indexfd < 10) {
						send_t(point, "sendmsg", 8, 131);
						sprintf(sendmessage, "SENDFILEUSERQPWOEIRUTY/%d/%s/%s\n", count, c[count].address, c[count].PORT);
						send_t(c[indexfd].new_fd, sendmessage, sizeof(sendmessage), 14);
					}
					else {
						printf("%dnot", indexfd);
						send_t(point, "sendmnsogt", 11, 132);
					}
				}
				else if (strcmp(buf, "endsend") == 0) { //���� :q�� ������
					indexfd = -1;
					send_t(point,"\nyou can chatting\n", 50, 9); //(ä�� �޽���)
				}
			}
		}
	}
	pthread_cleanup_pop(0);
}
void* chatwrite_thread(void* argu) { //������ chatwrite �� ������ �Լ��̸�, ������� ä���� client�� �����ϴ� ����� �ϴ� �Լ�
	int t;
	int point;
	int count = *((int*)argu);
	writeflag = 0;
	point = c[count].new_fd; //�ش� Ŭ���̾�Ʈ�� new_fd��);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_cleanup_push(cleanup, (void*)&point);//pthread_cancel �ݹ� �Լ� ���
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);//pthread_cancel�� �ٷ� ����
	//�����带 �α��� ������� �������� ���� �����尡 ����Ǿ �� ������� ������� �ʰ��Ѵ�.
	while (1) {
		if (writeflag == 1) {
			//chatbuf�� �ƹ��͵� ���� �޽����� �ƴϰ� ���� chatview_thread���� ������� writeflag�� 1�϶�
			if (indexfd > -1 && indexfd < 11) {
				continue;
			}
			else { //�׷��� ������
				t = send_t(point, chatbuf, sizeof(chatbuf), 9); //i = 9(ä�� �޽����� Ŭ���̾�Ʈ�� ����)
			}
			writeflag = 0; //writeflag�� 0���� �ٲ����ν�, �̹� �ѹ��� ������ ���� �޽����� ���� writeflag�� 1�� �� ������ ���
			strcpy(chatbuf, ""); //writeflag�� ���� ������ �ʱ�ȭ
		}
	}
	pthread_cleanup_pop(0);
}

void* login_thread(void* argu) { //�α����Լ�
	int rcv_byte;
	char idbuf[20];//id�޴� ����
	char pwbuf[20];//passwd�޴� ����
	char buf[512];
	char UI[512];
	char* UIstr;
	int val = *((int*)argu);
	pthread_cleanup_push(cleanup, (void*)&val);//pthread_cancel �ݹ� �Լ� ���
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);//pthread_cancel�� �ٷ� ����
	c[val].exitFlag = 0; //exitFlag �ʱ�ȭ = :q�� ���������� exitFlag�� �ݵ�� 0�̾�� �Ѵ�.
	rcv_byte = recv_t(c[val].new_fd, idbuf, sizeof(idbuf), 2);//i = 2 id�� ����
	send_t(c[val].new_fd, "idack", 6, 3);// i = 3 Ȯ�� ��ȣ
	rcv_byte = recv_t(c[val].new_fd, pwbuf, sizeof(pwbuf), 4);//i = 4 pw�� ����
	c[val].correctFlag = compareWord(idbuf, pwbuf, val);
	UIstr = distingish(c[val].correctFlag, idbuf, pwbuf);
	sprintf(UI, "%s%s%s", UI1, UIstr, UI2);
	printf("%s\n", UI);
	printf("msg: %s\n", msg);
	send_t(c[val].new_fd, msg, strlen(msg) + 1, 5);//i = 5 Ȯ�� ��ȣ��� �α��� ���� �޽���
	rcv_byte = recv_t(c[val].new_fd, buf, sizeof(buf), 6);//i = 6 client���� �α��� �Ϸ� Ȯ�� ��ȣ�� ����
	strcpy(buf, "");
	if (c[val].waitFlag == 0) { //���� ���� Ŭ���̾�Ʈ�� ��쿡 (3�� ���� �ش�)
		//c[val].waitFlag = 0;
		send_t(c[val].new_fd, "ack", 4, 7);//�б� 1, i = 7 Ȯ�ν�ȣ�� ����
		rcv_byte = recv_t(c[val].new_fd, c[val].address, sizeof(c[val].address), 8);//�б� 1,i = 8 Ŭ���̾�Ʈ�� IP�ּҸ� ����
		send_t(c[val].new_fd, "ack", 4, 9);//�б� 1, i = 9 Ȯ�� ��ȣ�� ����
		rcv_byte = recv_t(c[val].new_fd, c[val].PORT, sizeof(c[val].PORT), 10);//�б� 1, i =10 Ŭ���̾�Ʈ�� ��Ʈ ��ȣ�� ����
		if (c[val].correctFlag > 0) { //�α��� ������
			send_t(c[val].new_fd, "chat", 5, 11);  //�б� 1-1 , i = 11ä�ø�带 �����ϰڴٴ� �޽����� ����Ѵ�.
			pthread_create(&c[val].chatview, NULL, &chatview_thread, (void*)&val); //�ش� Ŭ���̾�Ʈ�� ä�� ������ ����
			pthread_create(&c[val].chatwrite, NULL, &chatwrite_thread, (void*)&val);
		}
		else {
			send_t(c[val].new_fd, "not", 4, 11); //�б� 1-2 , i = 11, �α��� �����ߴٴ� ��ȣ�� ������
		}
	}
	else { //�ѹ� exitflag�� 1�̾ ���� ���� �ִ� Ŭ���̾�Ʈ�� �ٽ� ���� ��
		if (c[val].correctFlag > 0) { //�α��� ������
			send_t(c[val].new_fd, "new", 4, 70); //�б� 2, i = 7 IP�ּҿ� ��Ʈ��ȣ�� ����Ǿ��ֱ� ������ ���� ���� �ʿ���ٴ� ��ȣ�� ����
			rcv_byte = recv_t(c[val].new_fd, buf, sizeof(buf), 80); //Ȯ�ν�ȣ�� ����
			send_t(c[val].new_fd, "chat", 4, 90);//�б� 2, i = 9 ä�ø�� ����
			pthread_create(&c[val].chatview, NULL, &chatview_thread, (void*)&val); //�ش� Ŭ���̾�Ʈ�� ä�� ������ ����
			pthread_create(&c[val].chatwrite, NULL, &chatwrite_thread, (void*)&val);
			c[val].waitFlag = 0;
		}
		else {
			send_t(c[val].new_fd, "new", 4, 170); //�б� 2, i = 7 IP�ּҿ� ��Ʈ��ȣ�� ����Ǿ��ֱ� ������ ���� ���� �ʿ���ٴ� ��ȣ�� ����
			rcv_byte = recv_t(c[val].new_fd, buf, sizeof(buf), 180); //Ȯ�ν�ȣ�� ����
			send_t(c[val].new_fd, "not", 4, 190); //�б� 2-2, i = 7 IP�ּҿ� ��Ʈ��ȣ�� ����Ǿ��ֱ� ������ ���� ���� �ʿ���ٴ� ��ȣ�� ����
		}
	}
	while (1) {
		if (c[val].exitFlag == 1) { //exitflag, �� �����Ҷ�
			strcpy(c[val].usingId, ""); //�α����� ��ҽ�Ű��
			close(c[val].new_fd); //�ش� Ŭ���̾�Ʈ�� new_fd������ �ݰ�
			pthread_cancel(c[val].chatview);
			pthread_cancel(c[val].chatwrite);
			c[val].new_fd = -1; //�ش��ϴ� new_fd���� -1�� ��ȯ
			c[val].waitFlag = 1; //���� accept ��� �÷��׸� 1�� �������
			c[val].exitFlag = 0;
		}
		//wait
	}
	pthread_cleanup_pop(0);
}

int main(void) //accept��
{
	int sockfd;
	int flag = 0;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	unsigned int sin_size;
	int th;

	int val = 1;
	int status = -1;//�ߺ��� �ּҹ�ȣ�϶�
	//char buf[512];
	//int rcv_byte;
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) {
		perror("Server-socket() error lol!");
		exit(1);
	}
	else printf("Server-socket() is OK...\n");
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(SERV_PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), 0, 8);
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val)) < 0) {
		perror("setsockopt");
		close(sockfd);
		return -1;
	}

	if (bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("server-bind() error lol!");
		close(sockfd);
		exit(1);
	}
	else printf("server-bind() is OK..\n");

	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen() is error lol!");
		exit(1);
	}
	else printf("listen() is OK...\n");
	pthread_create(&controlbox, NULL, &control, (void*)&buffer); // accept��,Ŭ���̾�Ʈ ���� ���� Ȯ���ϴ� ������ ���� 
	while (1) {
		//���ο� Ŭ���̾�Ʈ�� ������ �� ���ο� �ε����� �Ҵ��ϰ�
		//���� accept�� ���� �ѹ� �̻� �������� Ŭ���̾�Ʈ�� ���
		//������ accept�Ǿ��� �� �Ҵ���� �ε����� �޾ƾ��ϴµ�,
		//���� �̶� buffer �Ҵ���� �ε����� ���ư���, buffer + 1�� ���ο� �ε����� �ƴ϶��,
		//���� accept�� ������ �� �ش� �ε����� ���� Ŭ���̾�Ʈ�� �浹�Ѵ�.
		//�̸� �����ϱ� ���� status�� ���ο� �ε����� ��� �����Ѵ�.
		if (status != -1) { 
			buffer = status;
			status = -1; //status �ʱ�ȭ
		}
		sin_size = sizeof(struct sockaddr_in);
		new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size);
		buffer++;//���� ó������ buffer�� -1�̹Ƿ� -1 + 1 = 0���� �����Ѵ�.

		for (int i = 0; i < 10; i++) {
			if (add[i] == their_addr.sin_addr.s_addr) { //Ŭ���̾�Ʈ ������ ���� �Ǻ�(�������� ���)
				status = buffer-1; //ī��Ʈ������(�̶� buffer�� accept�� �Ǹ� �ʼ������� 1�� ���ϱ� ������ 1�� ���� �����Ͽ��� �Ѵ�)
				pthread_cancel(c[i].login);
				buffer = i; 
				c[buffer].new_fd = new_fd; //new_fd�� ������ �Ҵ���� �ε����� �����Ѵ�.
				break;
			}
			if (i == buffer) { //���� ������ ���
				c[buffer].new_fd = new_fd;
				break;
			}
		}
		add[buffer] = their_addr.sin_addr.s_addr;

		if (c[buffer].new_fd == -1) { //accept�Ǻ�
			perror("accept() is error lol!...");
			exit(1);
		}
		else printf("accept() is OK...\n");

		send_t(c[buffer].new_fd, INIT_MSG, strlen(INIT_MSG) + 1, 1); //i = 1 ȯ���ϴ� �޽��� ����
		th = pthread_create(&c[buffer].login, NULL, &login_thread, (void*)&buffer); //�α��� ������ ����
		if (th < 0) {
			printf("error");
		}
	}
}


