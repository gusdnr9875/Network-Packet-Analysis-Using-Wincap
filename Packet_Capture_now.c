#define _CRT_SECURE_NO_WARNINGS
#define WIN32
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <pcap\pcap.h>
#include <pcap.h>
#include <string.h>
#include <WinSock2.h>
#include <stdint.h>

#define IPHEADER 0x0800
#define ARPHEADER 0x0806
#define RARPHEADER 0x0835

/*�̴��� header ����ü*/
typedef struct Ethernet_Header
{
	u_char des[6];//������ MAC �ּ�
	u_char src[6];//�۽��� MAC �ּ�
	short int ptype;//�ڿ� ���� ��Ŷ�� �������� ����(��:ARP/IP/RARP)
	   //IP ����� ���� ��� : 0x0800
	   //ARP ����� ���� ��� : 0x0806
	   //RARP ����� ���� ��� : 0x0835
}Ethernet_Header;
/*Ip address ����ü*/
typedef struct ipaddress
{
	u_char ip1;
	u_char ip2;
	u_char ip3;
	u_char ip4;
}ip;
/*IP header ����ü*/
typedef struct IPHeader
{
	u_char HeaderLength : 4;//��� ���� *4
	u_char Version : 4;//IP v4 or IPv6
	u_char TypeOfService;//���� ����
	u_short TotalLength;//��� ���� + ������ ����/
	u_short ID;//������ ��Ʈ�� Identification
	u_short FlagOffset;//�÷��� + �����׸�Ʈ ������

	u_char TimeToLive;//TimeToL
	u_char Protocol;//�������� ����(1. ICMP 2. IGMP 6. TCP 17:UDP;
	u_short checksum;
	ip SenderAddress;
	ip DestinationAddress;
	u_int Option_Padding;

	// �߰�
	unsigned short source_port;
	unsigned short dest_port;
}IPHeader; 
/*TCP header ����ü*/
typedef struct TCPHeader
{
	unsigned short source_port;
	unsigned short dest_port;
	unsigned int sequence;
	unsigned int acknowledge;
	unsigned char ns : 1;
	unsigned char reserved_part1 : 3;
	unsigned char data_offset : 4;
	unsigned char fin : 1;
	unsigned char syn : 1;
	unsigned char rst : 1;
	unsigned char psh : 1;
	unsigned char ack : 1;
	unsigned char urg : 1;
	unsigned char ecn : 1;
	unsigned char cwr : 1;
	unsigned short window;
	unsigned short checksum;
	unsigned short urgent_pointer;
}TCPHeader;
/*UDP header ����ü*/
typedef struct udp_hdr
{
	unsigned short source_port; // Source port no.
	unsigned short dest_port; // Dest. port no.
	unsigned short udp_length; // Udp packet length
	unsigned short udp_checksum; // Udp checksum (optional)

} UDP_HDR;
/*HTTP header ����ü*/
typedef struct HTTPHeader
{
	uint16_t HTP[16];

}HTTPHeader;
/*CheckSum ����ü*/
typedef struct CheckSummer
{
	u_short part1;
	u_short part2;
	u_short part3;
	u_short part4;
	u_short part5;
	u_short checksum;
	u_short part6;
	u_short part7;
	u_short part8;
	u_short part9;

}CheckSummer; 
/*DNS ����ü*/
typedef struct DNS
{
	u_char transaction_ID[2];
	u_char domain_name[40];

}domain;

/*���͸��� ���� ���� ���� �ڵ鸵*/
void packet_handler(u_char* param, const struct pcap_pkthdr* h, const u_char* data);
/*�ð� ���*/
void print_first(const struct pcap_pkthdr* h, Ethernet_Header* EH);
/*��Ŷ ���� ���*/
void print_protocol(Ethernet_Header* EH, short int type, IPHeader* IH, TCPHeader* TCP, CheckSummer* CS);
/*ftp, */
void print_data(const u_char* data);
void print_packet_hex_data(u_char* data, int Size);

/*��������*/
u_int sel = 0;
char str_[20] = { 0, };

/***************************************/
/*                                     */
/*                 main                */
/*                                     */
/***************************************/

void main()
{
	pcap_if_t* allDevice; //ã�Ƴ� ����̽��� LinkedList�� ����, �� �� ù ��° ������Ʈ�� ���� ���� ����
	pcap_if_t* device; //Linked List�� ���� ������Ʈ�� ���� ����
	char errorMSG[256]; //���� �޽����� ���� ���� ����
	char counter = 0;

	pcap_t* pickedDev; //����� ����̽��� �����ϴ� ����

		   //1. ��ġ �˻� (ã�Ƴ� ����̽��� LinkedList�� ����)
	if ((pcap_findalldevs(&allDevice, errorMSG)) == -1)//���� �����ÿ��� 1 ����������, pcap_findallDevice�� ���°� ���� ����Ʈ�̹Ƿ� �ּҷ� �־�� ��.
					  //pcap_if_t�� int���¸� ��ȯ�ϸ�, -1�� ���� ���, ����̽��� ã�� ������ ����̴�.
		printf("��ġ �˻� ����");

	//2. ��ġ ���
	int count = 0;
	for (device = allDevice; device != NULL; device = device->next)
		//dev�� allDevice�� ù ���� �ּҸ� ������, dev�� ���� NULL(��)�� ��� ����, dev�� �� for���� ���� �ּҰ����� ��ȯ
	{
		printf("��  %d �� ��Ʈ��ũ ī�妬��������������������������������������������������������������������������������������������\n", count);
		printf("�� ����� ���� : %s ��\n", device->name);
		printf("�� ����� ���� : %s \n", device->description);
		printf("����������������������������������������������������������������������������������������������������������������������������������������\n");
		count = count + 1;
	}

	printf("��Ŷ�� ������ ��Ʈ��ũ ī�� ���� >> ");
	device = allDevice;//ī�带 �������� �ʰ� �׳� ù ��° ī��� ����

	int choice;
	scanf_s("%d", &choice);

	while (1) {
		printf("\n<���͸�>\n");
		printf(" 1. ICMP\n 2. TCP\n 3. UDP\n 4. HTTP\n 5. FTP\n 6. DNS\n 7. ALL\n 8. IP�� �˻�\n");
		printf(" >> ");
		scanf_s("%d", &sel);
		if (sel == 1 || sel == 2 || sel == 3 || sel == 4 || sel == 5 || sel == 6 || sel == 7 || sel == 8) {
			break;
		}
		else {
			printf("�ٽ��Է�\n");
		}
	}

	for (count = 0; count < choice; count++)
	{
		device = device->next;
	}

	//��Ʈ��ũ ��ġ�� ����, ������ ��Ŷ ���� �����Ѵ�.
	pickedDev = pcap_open_live(device->name, 65536, 0, 1000, errorMSG);
	//��ī���� �̸�, ������ ��Ŷ ũ��(�ִ� 65536), ���ι̽�ť����(��Ŷ ���� ���) ����, ��Ŷ ��� �ð�, ���� ������ ������ ����)

	//4. ��ī�� ����Ʈ ������ ������ �޸𸮸� ����ش�.
	pcap_freealldevs(allDevice);

	//5. ������ ��Ʈ��ũ ī�忡�� ��Ŷ�� ���� ĸ�� �� �Լ��� ����� ĸ�ĸ� �����Ѵ�.
	pcap_loop(pickedDev, 0, packet_handler, NULL);
}

/***************************************/
/*                                     */
/*            packet_handler           */
/*                                     */
/***************************************/
void packet_handler(u_char* param, const struct pcap_pkthdr* h, const u_char* data)
//���� = �Ķ����, ��Ŷ ���, ��Ŷ ������(������ MAC �ּ� �κ� ����)
{
	/*
	 * unused variables
	 */
	(VOID)(param);
	(VOID)(data);

	Ethernet_Header* EH = (Ethernet_Header*)data;//data �ּҿ� ����� 14byte �����Ͱ� ����ü Ethernet_Header ���·� EH�� ����ȴ�.
	short int type = ntohs(EH->ptype);
	//EH->ptype�� �� ����� ������ ���ϹǷ�,
	//�̸� ��Ʋ ����� �������� ��ȯ(ntohs �Լ�)�Ͽ� type�� �����Ѵ�.
	IPHeader* IH = (struct IPHeader*)(data + 14); //���� ó�� 14byte�� �̴��� ���(Layer 2) �� ������ IP���(20byte), �� ������ TCP ���...
	TCPHeader* TCP = (struct TCPHeader*)(data + 34); // TCP ��� 
	CheckSummer* CS = (struct CheckSummer*)(data + 14); //üũ���� ���� �� ����
	domain* dns = (struct DNS*) (data + 42);
	UDP_HDR* UDP = (struct UDP_HDR*)(data + IH->HeaderLength * 4 + sizeof(Ethernet_Header));

	char ip_comp[20]; //ip ���͸���

	/* 1. ICMP 2. TCP 3. UDP 4. HTTP 5. FTP 6. ALL */
	switch (sel) {
	case 1: //ICMP
		if (IH->Protocol == IPPROTO_ICMP) {
			print_first(h, EH);
			print_protocol(EH, type, IH, TCP, CS);
			printf("��\t���� �������� : ICMP               \n");
			printf("����������������������������������������������������������������������������������������������\n\n");
		}
		break;
	case 2: //TCP
		if (IH->Protocol == IPPROTO_TCP) {
			print_first(h, EH);
			print_protocol(EH, type, IH, TCP, CS);

			printf("��  --------------------------------------------  \n");
			printf("��\t\t*[ TCP ��� ]*\t\t\n");
			printf("��\tSCR PORT : %d\n", ntohs(TCP->source_port));
			printf("��\tDEST PORT : %d\n", ntohs(TCP->dest_port));
			printf("��\tSeg : %u\n", ntohl(TCP->sequence));
			printf("��\tAck : %u\n", ntohl(TCP->acknowledge));
			printf("����������������������������������������������������������������������������������������������\n\n");
		}
		break;
	case 3: //UDP
		if (IH->Protocol == IPPROTO_UDP) {
			print_first(h, EH);
			print_protocol(EH, type, IH, TCP, CS);

			printf("��  --------------------------------------------  \n");
			printf("��\t\t*[ UDP ��� ]*\t\t\n");
			printf("��\tSCR PORT : %d\n", ntohs(UDP->source_port));
			printf("��\tDEST PORT : %d\n", ntohs(UDP->dest_port));
			printf("����������������������������������������������������������������������������������������������\n\n");
		}
		break;
	case 4: //HTTP
		if (IH->Protocol == IPPROTO_TCP) {
			if (ntohs(TCP->source_port) == 80 || ntohs(TCP->dest_port) == 80 || ntohs(TCP->source_port) == 443 || ntohs(TCP->dest_port) == 443) {
				print_first(h, EH);
				print_protocol(EH, type, IH, TCP, CS);
				 
				printf("��  --------------------------------------------  \n");
				printf("��\t\t*[ TCP ��� ]*\t\t\n");
				printf("��\tSCR PORT : %d\n", ntohs(TCP->source_port));
				printf("��\tDEST PORT : %d\n", ntohs(TCP->dest_port));
				printf("��\tSeg : %u\n", ntohl(TCP->sequence));
				printf("��\tAck : %u\n", ntohl(TCP->acknowledge));
				printf("��\tHTTP �������� \n");
				uint8_t* packet = data + 34 + (IH->HeaderLength) * 4;
				printf("��  ---------------HTTP Header-------------- \n");
				printf("��\t%s \n", packet);
				printf("\n");
				printf("����������������������������������������������������������������������������������������������\n\n");
			}
		}
		break;
	case 5: //FTP
		if (IH->Protocol == IPPROTO_TCP) {
			if (ntohs(TCP->source_port) == 21 || ntohs(TCP->dest_port) == 21) {

				print_first(h, EH);
				print_protocol(EH, type, IH, TCP, CS);
				printf("��  --------------------------------------------  \n");
				printf("��\t\t*[ TCP ��� ]*\t\t\n");
				printf("��\tSCR PORT : %d\n", ntohs(TCP->source_port));
				printf("��\tDEST PORT : %d\n", ntohs(TCP->dest_port));
				printf("��\tSeg : %u\n", ntohl(TCP->sequence));
				printf("��\tAck : %u\n", ntohl(TCP->acknowledge));
				printf("��\tFTP �������� \n");
				u_char* packet_ = data + 34 + (IH->HeaderLength) * 4;
				printf("��  --------------FTP DATA--------------\n");
				printf("��\t%s \n", packet_);
				printf("����������������������������������������������������������������������������������������������\n\n");
			}
		}
		break;
	case 6: //DNS
		if (ntohs(TCP->source_port) == 53 || ntohs(TCP->dest_port) == 53) {
			print_first(h, EH);
			print_protocol(EH, type, IH, TCP, CS);

			printf("��  --------------------------------------------  \n");
			printf("��\t\t*[ TCP ��� ]*\t\t\n");
			printf("��\tSCR PORT : %d\n", ntohs(TCP->source_port));
			printf("��\tDEST PORT : %d\n", ntohs(TCP->dest_port));
			printf("��\tSeg : %u\n", ntohl(TCP->sequence));
			printf("��\tAck : %u\n", ntohl(TCP->acknowledge));
			printf("��\tDNS �������� \n");

			for (int i = 0; i < 40; i++) {
				if (dns->domain_name[i] > 60)
					printf("%c", dns->domain_name[i]);
				else if (dns->domain_name[i - 1] > 60)
					printf(".");
				if ((dns->domain_name[i - 2] == 'o' || dns->domain_name[i - 2] == 'k') && (dns->domain_name[i - 1] == 'm' || dns->domain_name[i - 1] == 'r') && dns->domain_name[i] < 60)
					break;
			}
			//printf("%s\n", dns->domain_name);
			printf("\n");
			printf("����������������������������������������������������������������������������������������������\n\n");
		}
		break;
	case 7: //ALL
		print_first(h, EH);
		print_protocol(EH, type, IH, TCP, CS);

		switch (IH->Protocol) {
		case IPPROTO_ICMP: //ICMP
			printf("��\t���� �������� : ICMP               \n");
		case IPPROTO_IGMP: //IGMP
			printf("��\t���� �������� : IGMP              \n");
			break;
		case IPPROTO_TCP: //TCP
			printf("��\t���� �������� : TCP              \n");
			printf("��  --------------------------------------------  \n");
			printf("��\t\t*[ TCP ��� ]*\t\t\n");
			printf("��\tSCR PORT : %d\n", ntohs(TCP->source_port));
			printf("��\tDEST PORT : %d\n", ntohs(TCP->dest_port));
			printf("��\tSeg : %u\n", ntohl(TCP->sequence));
			printf("��\tAck : %u\n", ntohl(TCP->acknowledge));

			/*FTP*/
			if (ntohs(TCP->source_port) == 21 || ntohs(TCP->dest_port) == 21) {
				printf("��\tFTP �������� \n");
				u_char* packet_ = data + 34 + (IH->HeaderLength) * 4;
				printf("��  --------------FTP DATA--------------\n");
				printf("��\t%s \n", packet_);
			}

			/*HTTP*/
			if (ntohs(TCP->source_port) == 80 || ntohs(TCP->dest_port) == 80 || ntohs(TCP->source_port) == 443 || ntohs(TCP->dest_port) == 443) {
				printf("��\tHTTP �������� \n");
				uint8_t* packet__ = data + 34 + (IH->HeaderLength) * 4;
				printf("��  ---------------HTTP Header-------------- \n");
				printf("��\t%s \n", packet__);
			}
			break;
		case IPPROTO_PUP:
			printf("��\t���� �������� : PUP                                    \n");
			break;
		case IPPROTO_UDP: //UDP
			printf("��\t���� �������� : UDP                    \n");
			break;
		case IPPROTO_IDP:
			printf("��\t���� �������� : XNS IDP                                \n");
			break;
		case IPPROTO_PIM:
			printf("��\t���� �������� : Independent Multicast                  \n");
			break;
		case IPPROTO_RAW:
			printf("��\t���� �������� : Raw IP Packets                         \n");
			break;
		default:
			printf("��\t���� �������� : Unknown                                \n");
		}
		printf("����������������������������������������������������������������������������������������������\n\n");

		break;
	case 8: //IP ���͸�
		if (str_[0] == 0) {
			printf("\nIP �Է� >> ");
			scanf_s("%s", str_);
			sprintf(ip_comp, "%d.%d.%d.%d", IH->SenderAddress.ip1, IH->SenderAddress.ip2, IH->SenderAddress.ip3, IH->SenderAddress.ip4);
		}
		if (strcmp(str_, ip_comp) == 0) {
			print_first(h, EH);
			print_protocol(EH, type, IH, TCP, CS);
			printf("����������������������������������������������������������������������������������������������\n\n");
		}
		break;
	default:
		break;
	}
}

void print_first(const struct pcap_pkthdr* h, Ethernet_Header* EH) {
	struct tm ltime;
	char timestr[16];
	time_t local_tv_sec;

	/* convert the timestamp to readable format */
	local_tv_sec = h->ts.tv_sec;
	localtime_s(&ltime, &local_tv_sec);
	strftime(timestr, sizeof timestr, "%H:%M:%S", &ltime);
	printf("\n\n==================================================\n");
	printf("\tTime: %s,%.6d len:%d\n", timestr, h->ts.tv_usec, h->len);
	printf("\tNext Packet : %04x\n", EH->ptype);
}

void print_protocol(Ethernet_Header* EH, short int type, IPHeader* IH, TCPHeader* TCP, CheckSummer* CS) {

	printf("������������������������������������������������������������������������������������������������\n");
	printf("��\t\t*[ Ethernet ��� ]*\t\t\n");
	printf("��\tSrc MAC : %02x-%02x-%02x-%02x-%02x-%02x\n", EH->src[0], EH->src[1], EH->src[2], EH->src[3], EH->src[4], EH->src[5]);//�۽��� MAC
	printf("��\tDst MAC : %02x-%02x-%02x-%02x-%02x-%02x\n", EH->des[0], EH->des[1], EH->des[2], EH->des[3], EH->des[4], EH->des[5]);//������ MAC
	printf("��--------------------------------------------\n");

	//���� ������ 01010101�̹Ƿ� ������ �ڸ���� ���ص� ��.
	//����� �ٴ� Layer2�� �����͸�ũ �������� �ڸ��� ��.

	if (type == IPHEADER)
	{
		printf("��\t\t*[ IP ��� ]*\n");
		printf("��\tProtocol : IP\n");
		int partSum = ntohs(CS->part1) + ntohs(CS->part2) + ntohs(CS->part3) + ntohs(CS->part4) + ntohs(CS->part5) + ntohs(CS->part6) + ntohs(CS->part7) + ntohs(CS->part8) + ntohs(CS->part9);
		u_short Bit = partSum >> 16;
		printf("��\t��Ʈ �� : %08x\n", partSum);
		// printf("��\t4ĭ �̵� : %08x\n", Bit);
		partSum = partSum - (Bit * 65536);
		// printf("��\t�ѱ�� ���� ��Ʈ �� : %04x\n", partSum + Bit);
		// printf("��\t���� ���ϱ� : %04x\n", (u_short)~(partSum + Bit));
		printf("��\tüũ�� : %04x\n", ntohs(CS->checksum));
		if (ntohs(CS->checksum) == (u_short)~(partSum + Bit))
			printf("��\t�ջ���� ���� ���� ��Ŷ�Դϴ�.\n");
		else
			printf("��\t�ջ�� ��Ŷ�Դϴ�. ������ ��û�� �ؾ� �մϴ�.\n");
		printf("��\t���� : IPv%d\n", IH->Version);
		printf("��\t��� ���� : %d\n", (IH->HeaderLength) * 4);
		printf("��\t���� ���� : %04x\n", IH->TypeOfService);
		printf("��\t��ü ũ�� : %d\n", ntohs(IH->HeaderLength));//2 bytes �̻� ���ʹ� ������ ������� �ϹǷ� ntohs�Լ��� �Ἥ �����´�.
		printf("��\t�����׸�Ʈ ������ : %d[byte]\n", (0x1FFF & ntohs(IH->FlagOffset) * 8));
		printf("��\tTTL : %d\n", IH->TimeToLive);
		//  printf("��\tüũ�� : %04x\n", ntohs(IH->checksum));//��) 0x145F
		printf("��\t��� IP �ּ� : %d.%d.%d.%d\n", IH->SenderAddress.ip1, IH->SenderAddress.ip2, IH->SenderAddress.ip3, IH->SenderAddress.ip4);
		printf("��\t���� IP �ּ� : %d.%d.%d.%d\n", IH->DestinationAddress.ip1, IH->DestinationAddress.ip2, IH->DestinationAddress.ip3, IH->DestinationAddress.ip4);
		//   printf("��\t�ɼ�/�е� : %d\n", IH->Option_Padding);
		printf("\n");
		
		int iphdrlen = 0;
		iphdrlen = IH->HeaderLength * 64;

		print_packet_hex_data((u_char*)IH, iphdrlen);
	}
	else if (type == ARPHEADER)
	{
		printf("��\tProtocol : ARP\n");
	}
	else if (type == RARPHEADER)
		printf("��\tProtocol : RARP\n");
}

void print_data(const u_char* data) {
	printf("��  ----------------DATA--------------\n");
	printf("��\t%s \n", data);
}

void print_packet_hex_data(u_char* data, int Size)
{
	unsigned char a, line[17], c;
	int j;
	//loop over each character and print
	for (int i = 0; i < Size; i++)
	{
		c = data[i];
		//Print the hex value for every character , with a space
		printf(" %.2x", (unsigned int)c);
		//Add the character to data line
		a = (c >= 32 && c <= 128) ? (unsigned char)c : '.';
		line[i % 16] = a;
		//if last character of a line , then print the line - 16 characters in 1 line
		if ((i != 0 && (i + 1) % 16 == 0) || i == Size - 1)
		{
			line[i % 16 + 1] = '\0';
			//print a big gap of 10 characters between hex and characters
			printf("          ");
			//Print additional spaces for last lines which might be less than 16 characters in length
			for (j = strlen((const char*)line); j < 16; j++)
			{
				printf("   ");
			}
			printf("%s \n", line);
		}
	}
	printf("\n");
}
