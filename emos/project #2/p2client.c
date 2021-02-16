#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pthread.h> //pthread ���
#define SERV_IP "220.149.128.103" 
#define SERV_PORT 4539
pthread_t chatview;//ä���� ��� ���� ���� ������
pthread_t chatwrite;//ä���� �Է��� ������ �����ϱ� ���� ���� ������
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
void* chatview_thread(void* argu) { // ������ chatview�� �����ϴ� �Լ�
	char recvBuf[512];
	while (1) {
		c.rcv_byte = recv_t(c.sockfd, recvBuf, 512, 12);
		if (c.rcv_byte > -1) { //������
			if (c.writeFlag == 0) { //�ڽ��� ���� ä���� �ƴϸ�
				printf("%s", recvBuf); //���
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
		if (strcmp(message, ":q\n") == 0) { //:q�� ġ��
			//printf("bye");
			c.exitFlag = 1; //exitFlag 1�� ??��?
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
	send_t(c.sockfd, "ack", 4, 8);//�б� 2, i = 8
	//chat
	c.rcv_byte = recv_t(c.sockfd, buf2, sizeof(buf2), 11);//�б� 1�϶� i = 9, �б� 2�϶� i = 9
	printf("buf: %s\n", buf2);
	if (strcmp(buf2, "chat") == 0) { //buf2�� 
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

