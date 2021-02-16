#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <ctype.h>
#include <pthread.h> //pthread ���
#define SERV_IP "220.149.128.103" 
#define SERV_PORT 4538
#define CLIT_IP "220.149.128.102" //Ŭ���̾�Ʈ IP
#define CLIT_PORT "4537" //Ŭ���̾�Ʈ ��Ʈ ��ȣ
#define CLIT_PORT_INT 4537
pthread_t chatview;//ä���� ��� ���� ���� ������
pthread_t chatwrite;//ä���� �Է��� ������ �����ϱ� ���� ���� ������
pthread_t sendfile;//������ ������ ���� ���� ������
pthread_t recvfile;//������ �ޱ� ���� ���� ������
char sendmessage[100]; //3�� �������� �������� ���� ��Ʈ�� IP�� ������
struct flag { //�÷��׸� ����ü�� �����Ͽ���.
	int sockfd; //����
	int rcv_byte; 
	int writeFlag; //Ŭ���̾�Ʈ ȭ�鿡 ����ϴ� �÷��� 
	int exitFlag; //Ŭ���̾�Ʈ ���� �÷���
};
struct flag c;
int recv_t(int socket, char* buf, int buff, int i) {//�帧 ���� ���� ���� �Լ�
	int t;
	t = recv(socket, buf, buff, 0);
	return t;
}
int send_t(int socket, char* buf, int buff, int i) {//�帧 ���� ���� ���� �Լ�
	int t;
	t = send(socket, buf, buff, 0);
	if (t > -1) {
		//printf("send : %d, %d\n", i,t);
	}
	return t;
}

void cleanup(void* argu) {
	int socket = *((int*)argu);
	printf("clean");
	close(socket); //���� ������ �ϴ� Ŭ���̾�Ʈ�� ������ ���� 
	printf("----------END SEND----------\n");
}

void* recvfile_thread(void* argu) { 
	//������ recvfile�� ������ �Լ�, �޽����� �޾� ���Ͽ� �����ϴ� ����� �Ѵ�.
	int recv_sockfd, recv_new_fd;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	unsigned int sin_size;
	FILE* pFile = NULL;
	int rcv_byte;
	char buf[512];
	char arg[512];
	char filename[25]; //���� ����
	char choose[10] = "";
	int val = 1;
	printf("\n----------FILE SEND----------\n");
	FILE* rFile = NULL;
	/*server*/
	recv_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_cleanup_push(cleanup, (void*)&recv_sockfd);//pthread_cancel �ݹ� �Լ� ���
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);//pthread_cancel�� �ٷ� ����
	if (recv_sockfd == -1) {
		perror("Server-socket() error lol!");
		exit(1);
	}
	else printf("Server-socket() sockfd is OK...\n");
	/* host byte order */
	my_addr.sin_family = AF_INET;
	/* short, network byte order */
	my_addr.sin_port = htons(CLIT_PORT_INT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	/* zero the rest of the struct */
	memset(&(my_addr.sin_zero), 0, 8);
	/* to prevent ��Address already in use���� */
	if (setsockopt(recv_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val)) < 0)
	{
		perror("setsockopt");
		close(recv_sockfd);
		exit(1);
		//return -1;
	}
	/* bind */
	if (bind(recv_sockfd, (struct sockaddr*) & my_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Server-bind() error lol!");
		exit(1);
	}
	else printf("Server-bind() is OK...\n");
	/* listen */
	if (listen(recv_sockfd, 10) == -1)
	{
		perror("listen() error lol!");
		exit(1);
	}
	else printf("listen() is OK...\n\n");
	/* ...other codes to read the received data... */
	sin_size = sizeof(struct sockaddr_in);
	recv_new_fd = accept(recv_sockfd, (struct sockaddr*) & their_addr, &sin_size);
	if (recv_new_fd == -1) {
		perror("accept() is error lol!...");
		exit(1);
	}
	else printf("accept() is OK...\n");
	/* receive file msg */
	rcv_byte = recv(recv_new_fd, buf, sizeof(buf), 0); //recvfile i = 1 ����Ʈ�� ���� 
	printf("%s\n", buf);
	fgets(choose, sizeof(choose), stdin);//����Ʈ�� ���� ����������� txt������ ��
	send_t(recv_new_fd, choose, sizeof(choose), 100);//recvfile i = 2 �� ��ȣ�� ������ 
	strcpy(buf, "");
	rcv_byte = recv(recv_new_fd, filename, sizeof(filename), 0);//recvfile i = 3 �ش� ��ȣ�� ������ �޾ƿ�
	if (rcv_byte > -1) {
		rFile = fopen(filename, "w");
		//��ȣ�� �������� ������ �޾ƿ�, �̶� ���� w�� �ش� ������ �����Ҷ� 
		//�ʱ�ȭ�ϰ� Ŭ���̾�Ʈ �������ϴ� Ŭ���̾�Ʈ�� �޾ƿ� ������ ������
		if (rFile == NULL) { //���н�
			printf("failed to makefile\n");
		}
		else {
			send_t(recv_new_fd, "ack", 4, 200);//recvfile i = 4 Ȯ�ν�ȣ ����
			rcv_byte = recv(recv_new_fd, arg, sizeof(arg), 0);//recvfile i = 5 ������ �޾ƿ�
			if (strcmp(arg, "exit") != 0) {
				fputs(arg, rFile); //���Ͽ� ������ �Է���
			}
		}
	}
	/*close*/
	pthread_cleanup_pop(0);
	fclose(rFile); //������ ����
	close(recv_new_fd); //Ŭ���̾�Ʈ ������ �ϴ� Ŭ���̾�Ʈ�� accept���� ����
	close(recv_sockfd); //���� ������ �ϴ� Ŭ���̾�Ʈ�� ������ ���� 
	printf("\n----------END SEND----------\n");
	return 0;
}

void* sendfile_thread(void* argu){
//������ sendfile�� ������ �Լ�, �޽����� �޾� ���Ͽ� �����ϴ� ����� �Ѵ�.
	char dirindex[20][25]; //���� ���� ���� �ִ� 20������ ���� �� ����
	char filename[25]; // ���� ����
	char sendmsg[512] = ""; //���� ����Ʈ
	int	i = 0;
	char charchoose[10] = ""; 
	char updatechoose[5] = ""; //charchoose�� updatechoose�� �ް����ϴ� Ŭ���̾�Ʈ�� �� txt������ ����Ʈ ��ȣ
	int choose=-1; //char -> int ������ ��ȯ�� ����Ʈ ��ȣ
	DIR* dir; 
	struct dirent* list;
	int Flag = 0;
	/*sendmessage �̿��� �� �ִ� �����ͷ� ��ȯ�ϴ� ����*/
	char* cut = strtok(sendmessage, "/");    //ù��° strtok ���. 
	char* fd = "";
	char* ad = "";
	char* po = "";
	int port = 0; //int������ ��ȯ�� po
	char addr[14]; //���� ũ��� ��ȯ�� ad
	FILE* pFile = NULL;
	int send_sockfd; /* will hold the destination addr */
	struct sockaddr_in dest_addr;
	int rcv_byte;
	char filemsg[512];
	char buf[512];
	while (cut != NULL)              //ptr�� NULL�϶����� (= strtok �Լ��� NULL�� ��ȯ�Ҷ�����)
	{
		printf("%s\n", cut);
		cut = strtok(NULL, "/");     //�ڸ� ���� �������� ������ �� ã��
		if (strcmp(fd, "") == 0) {
			fd = cut; //����ó���� ���� ���ۿ��� Ŭ���̾�Ʈ ������ �� Ŭ���̾�Ʈ�� �ε��� ��
		}
		else if (strcmp(ad, "") == 0) {
			ad = cut; //�ι�°�� ���� ���ۿ��� Ŭ���̾�Ʈ ������ �� Ŭ���̾�Ʈ�� IP
		}
		else if (strcmp(po, "") == 0) {
			po = cut;  //���� ���ۿ��� Ŭ���̾�Ʈ ������ �� Ŭ���̾�Ʈ�� ��Ʈ
		}
	}
	strcpy(addr, ad); //���������� ��ȯ
	port = atoi(po); //int������ ��ȯ
	//printf("%s/%d", addr, port);
	/*client*/
	send_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (send_sockfd == -1)
	{
		perror("Client-socket() error lol!");
		exit(1);
	}
	else printf("Client-socket() sockfd is OK...\n");
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	dest_addr.sin_addr.s_addr = inet_addr(addr);
	memset(&(dest_addr.sin_zero), 0, 8);
	/* connect */
	if (connect(send_sockfd, (struct sockaddr*) & dest_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Client-connect() error lol");
		exit(1);
	}
	else printf("Client-connect() is OK...\n\n");
	/*send file */
	printf("\n----------FILE SEND----------\n");
	dir = opendir(".");              // ���� ���丮�� ����
	if (NULL != dir && i < 50)
	{
		int k = 0;
		while (list = readdir(dir))  // ���丮 �ȿ� �ִ� ��� ���ϰ� ���丮 ���
		{
			if (strstr(list->d_name, ".txt") != NULL) {
				strcpy(dirindex[i], list->d_name);
				sprintf(sendmsg, "%s%d\t%s\n", sendmsg, i, dirindex[i]); //����Ʈ ����
				i++;
			}
			else {
				//printf("NULL");
			}
		}
	}
	send_t(send_sockfd, sendmsg, sizeof(sendmsg), 1);//sendfile,i = 1 ����Ʈ ����
	rcv_byte = recv(send_sockfd, charchoose, sizeof(charchoose), 0);
	for(int p = 0; p < strlen(charchoose) + 1; p++) {
		if (charchoose[p] == '\n') {
			break;
		}
		else {
			updatechoose[p] = charchoose[p]; //����Ʈ ��ȣ�� �ű�
			if (isdigit(updatechoose[p]) == 0) {
				printf("\n----------END SEND----------\n");
				rcv_byte = recv(send_sockfd, buf, sizeof(buf), 0);//sendfile i = 4 Ȯ�ν�ȣ�� ����
				send_t(send_sockfd, "exit", 5, 300);//sendfile i = 5 ������ ����
				close(send_sockfd); //send socket ����
				pthread_exit(NULL); //������ ����
			}
		}
	}
	choose = atoi(updatechoose); //int�� ��ȯ 
	for (int j = 0; j < i; j++) { //i ũ�⸸ŭ (i = ���� ���丮���� ������ ����) 
		if (j == choose) { //ã����
			printf("client is choose %s\n", dirindex[j]);
			strcpy(filename, dirindex[j]);
			send_t(send_sockfd, filename, sizeof(filename), 3);//sendfile i=3 ������ ����
			break;
		}
		else {
			//printf("no\n"); //������ ���ٰ� ���
		}
	}
	pFile = fopen(filename, "r"); //�ش� ������ ��
	while (1) {
		char arr[50];
		char* ptr = fgets(arr, 50, pFile);
		if (ptr == NULL) { //�ش� ���� NULL�϶�����
			break;
		}
		sprintf(filemsg, "%s%s", filemsg, ptr); //sprintf�� string�� �����.
	}
	rcv_byte = recv(send_sockfd, buf, sizeof(buf), 0);//sendfile i = 4 Ȯ�ν�ȣ�� ����
	//printf("%s", filemsg);
	send_t(send_sockfd, filemsg, strlen(filemsg)+1, 300);//sendfile i = 5 ������ ����
	printf("\n----------END SEND----------\n");
	/*end*/
	fclose(pFile); //���� ����
	close(send_sockfd); //send socket ����
	pthread_exit(NULL); //������ ����
}
void* chatview_thread(void* argu) { // ������ chatview�� �����ϴ� �Լ�
	char recvBuf[512];
	char resultBuf[512];
	char message[512];
	while (1) {
		c.rcv_byte = recv_t(c.sockfd, recvBuf, 512, 12);
		if (c.rcv_byte > -1) { //������
			if (c.writeFlag == 0) { //�ڽ��� ���� ä���� �ƴϸ�
				if (strncmp(recvBuf, "SENDFILEUSERQPWOEIRUTY", 22) == 0) { 
					//�������� ���� ������ Ŭ���̾�Ʈ ������ �ϰ� �Ǿ��ٴ� ������ ������
					strcpy(sendmessage, recvBuf); //���������� �ű�
					pthread_create(&sendfile, NULL, &sendfile_thread, NULL); //sendfile ������ ����
					//printf("hi~ : %s", recvBuf);
					pthread_join(sendfile, NULL); //������ ����� ���� ��ٸ�
				}
				else if (strncmp(recvBuf, "sendmsg", 8) == 0) {
					printf("\n");
				}
				else if (strncmp(recvBuf, "sendmnsogt",11) == 0) {
					pthread_cancel(recvfile);
				}
				else {
					printf("%s", recvBuf); //���
				}
			}
			else {
				c.writeFlag = 0;
			}
		}
		if (c.exitFlag == 1) { //���� �÷��װ� 1�̸�
			//printf("bye");
			pthread_exit(NULL); //������ ����
		}
	}
	return NULL;
}
void* chatwrite_thread(void* argu) { //������ chatwrite�� �����ϴ� �Լ�
	char buf[512];
	int t;
	int index = 0;
	char message[512];
	strcpy(message, "");
	while (1) {
		//printf("message: %s",message);
		fgets(message, sizeof(message), stdin); // �޽����� �Է¹���
		c.writeFlag = 1; //�Է��� �޽����� ������ �ڽ��� ���´ٴ� ���� �˱� ���� writeflag�� 1�� �����ؼ� 
		t = send_t(c.sockfd, message, sizeof(message), 13); //������ ����
		if (strncmp(message, "[FILE", 5) == 0) { //[FILE ~�� ġ��
			pthread_create(&recvfile, NULL, &recvfile_thread, NULL); //���� ���� ������� ����
			pthread_join(recvfile, NULL); //������ ����� ���� ��ٸ�
			send_t(c.sockfd, "endsend", 8, 15);//recvfile
			continue;
		}
		if (strcmp(message, ":q\n") == 0) { //:q�� ġ��
			//printf("bye");
			c.exitFlag = 1; //exitFlag 1�� ������
			strcpy(message, "");
			pthread_exit(NULL); //������ ����
		}
		//}
	}
	return NULL;
}
int main(int argc, char* argv[]) {
	struct sockaddr_in dest_addr;
	char buf[512];
	char buf2[512];
	char id[20];
	char pw[20];
	char chatReceive[512];
	c.writeFlag = 0;
	c.exitFlag = 0;
	c.sockfd = socket(AF_INET, SOCK_STREAM, 0);
	c.exitFlag = 0;
	if (c.sockfd == -1)
	{
		perror("client-socket() error lol!");
		exit(1);
	}
	else printf("client-socket() sockfd is OK...\n");
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(SERV_PORT);
	dest_addr.sin_addr.s_addr = inet_addr(SERV_IP);

	memset(&(dest_addr.sin_zero), 0, 8);

	if (connect(c.sockfd, (struct sockaddr*) & dest_addr, sizeof(struct sockaddr)) == -1) {
		perror("client-connect() error lol");
		exit(1);
	}
	else printf("client-connect() is ok...\n");
	c.rcv_byte = recv_t(c.sockfd, buf, sizeof(buf), 1);// i = 1 �������� ȯ���ϴ� �޽��� ���� 
	printf("%s\n", buf);
	printf("id: ");
	scanf("%s", id);
	send_t(c.sockfd, id, strlen(id) + 1, 2);//i = 2 id�� ����
	c.rcv_byte = recv_t(c.sockfd, buf, sizeof(buf), 3);//i = 3 Ȯ�� ����
	printf("pw: ");
	scanf("%s", pw);
	send_t(c.sockfd, pw, strlen(pw) + 1, 4);//i = 4 pw�� ����
	c.rcv_byte = recv_t(c.sockfd, buf, sizeof(buf), 5);//i = 5 ��������  �α��� �޽��� ����
	printf("buf :%s\n", buf); //�α��� �޽��� ���
	send_t(c.sockfd, "ack", 4, 6);//i = 6 �α��� �Ϸ� Ȯ�� ��ȣ�� ���� 
	//port
	strcpy(buf, "");
	printf("buf: %s\n", buf);
	c.rcv_byte = recv_t(c.sockfd, buf, sizeof(buf), 7);//7 �α��� �������� Ȯ�� ��ȣ�� ����
	printf("buf: %s\n", buf);
	if (strcmp(buf, "new") != 0) { //���� �������� �ƴ϶��
		send_t(c.sockfd, CLIT_IP, sizeof(CLIT_IP), 8);//�б� 1, 8 �� Ŭ���̾�Ʈ�� IP�� ����
		c.rcv_byte = recv_t(c.sockfd, buf, sizeof(buf), 9);//�б� 1, 9Ȯ�� ��ȣ�� ���� 
		send_t(c.sockfd, CLIT_PORT, sizeof(CLIT_PORT), 10);//�б� 1, 10 �� Ŭ���̾�Ʈ�� PORt�� ����
	}
	else {
		send_t(c.sockfd, "ack", 4, 8);//�б� 2, i = 8
	}
	//chat
	c.rcv_byte = recv_t(c.sockfd, buf2, sizeof(buf2), 11);//�б� 1�϶� i = 9, �б� 2�϶� i = 9
	printf("buf: %s\n", buf2);
	if (strcmp(buf2, "chat") == 0 || strcmp(buf2, "new") == 0) { //buf2�� 
		printf("---------------Chatting Room---------------\n");
		pthread_create(&chatview, NULL, &chatview_thread, NULL); //ä�� ������ ����
		pthread_create(&chatwrite, NULL, &chatwrite_thread, NULL); 
		pthread_join(chatwrite, NULL);// :q ������ ������ ���������� ���
		pthread_join(chatview, NULL);
	}
	//printf("exit %d", c.exitFlag);
	close(c.sockfd); //:q������ ������ �ݰ�
	exit(0); // ���μ��� ����
}

