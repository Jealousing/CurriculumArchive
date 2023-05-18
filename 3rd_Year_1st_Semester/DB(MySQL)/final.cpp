#pragma comment (lib, "libmysql.lib")
#include <stdio.h>
#include <mysql.h>
#include <string.h>

#define HOST "127.0.0.1" //�����ּ�
#define PORT 3306 //mysql ��Ʈ��ȣ
#define USER "root" //������
#define PASSWORD "1234" //��й�ȣ
#define DATABASE "gamedb" //���� �����ͺ��̽� �̸�

//DB control�� state
enum State
{
	init = 0,
	selectDB,			//Db ����
	selectTable,		//table ����
	selectWork,		//���� ����
	Insert,				//�Է�
	Update,				//����
	Delete,				//����
	Search,			//�˻�
	exitstate			//����
}m_State;

//gamedb�� ���̺��
enum GameDB
{
	arr=0,
	user_table,
	character_table,
	item_table,
	user_log_table,
	character_log_table,
	item_log_table
};


int main()
{
	MYSQL mysql; //�����Ҷ� ���
	MYSQL_RES* sql_result; //���
	MYSQL_ROW sql_row; //���ε��� �� ���

	mysql_init(&mysql); //�ʱ�ȭ

	//scanf�� ����
	char scan_char;
	int scan_int;
	
	// ���ڿ� ���� 
	char DB[100];				//db�̸�
	char Table[100];			//���̺��
	char TableType[100];	//���̺� Ÿ��
	char command[100]; //Ŀ�ǵ��

	//data�� ����
	int datanum = 0;

	//dbcontrol ���࿩��
	bool Isplay = true;

	//temp ���ڿ�
	char temp[100];
	char temp2[100];
	
	while (Isplay)
	{
		//�޸��ʱ�ȭ
		memset(command, 0, sizeof(command));
		memset(temp, 0, sizeof(temp));
		memset(temp2, 0, sizeof(temp2));

		//������Ʈ���� ����
		switch (m_State)
		{
		case init: //ó�� ���� intro
			printf(" Mysql DB�� �����ϴ� ���α׷��Դϴ�\n ����(Y/N)");
			scanf_s(" %c", &scan_char);
			if (scan_char == 'Y' || scan_char == 'y')
				m_State = selectDB;
			else
				m_State = exitstate;
			break;
		case selectDB: //db����
			printf(" �����Ͻ� DB�� �����ϼ���\n ");
			printf(" 1. GameDB \n");
			scanf_s("%d", &scan_int);
			printf("\n");
			switch (scan_int)
			{
			case 1:
				//db�� ����
				strcpy_s(DB, "GameDB");
				//�ش� �����ͺ��̽� ����
				//����õ�  (�����, �����ּ�, ������, ��й�ȣ, ������ �����ͺ��̽�, ��Ʈ��ȣ, ����, flag)
				if (!mysql_real_connect(&mysql, HOST, USER, PASSWORD, DATABASE, PORT, NULL, 0))
				{
					//������ ���ڿ��� �����ͼ� �Է�
					printf("mysql connect error : %s \n", mysql_error(&mysql));
					return -1;
				}
				//�ѱ��� ����� ���� ����
				if (mysql_query(&mysql, "set session character_set_results = euckr"))
				{
					//������ ���ڿ��� �����ͼ� �Է�
					printf("mysql query error : %s \n", mysql_error(&mysql));
				}
				//������Ʈ ����
				m_State = selectTable;
				break;
			default:
				printf("���� ��ȣ�� �Է��ϼ̽��ϴ�,\n �����(Y/N)");
				scanf_s(" %c", &scan_char);
				if (scan_char == 'Y' || scan_char == 'y')
				{

				}
				else
				{
					m_State = exitstate;
				}
				break;
			}
		case selectTable: //���̺� ����
			//������ db�� gamedb�ϰ�� ����
			if (strcmp(DB, "GameDB")==0)
			{
				//���̺� ����
				printf(" � ���̺��� �۾��Ͻðڽ��ϱ�? \n");
				printf("1. user_table\n");
				printf("2. character_table\n");
				printf("3. item_table\n");
				printf("4. user_log_table\n");
				printf("5. character_log_table\n");
				printf("6. item_log_table\n");

				scanf_s(" %d", &scan_int);
				printf("\n");
				switch (scan_int)
				{
					//������ ���̺� ���� ����
					//table �� ����
					//���̺� Ÿ�Լ���
					//data���� ����
					//������Ʈ ����
				case 1: 
					strcpy_s(Table, "user_table");
					strcpy_s(TableType, "NotLog");
					datanum = 4;
					m_State = selectWork;
					break;
				case 2:
					strcpy_s(Table, "character_table");
					strcpy_s(TableType, "NotLog");
					datanum = 6;
					m_State = selectWork;
					break;
				case 3:
					strcpy_s(Table, "item_table");
					datanum = 4;
					strcpy_s(TableType, "NotLog");
					m_State = selectWork;
					break;
				case 4:
					strcpy_s(Table, "user_log_table");
					strcpy_s(TableType, "Log");
					datanum = 5;
					m_State = selectWork;
					break;
				case 5:
					strcpy_s(Table, "character_log_table");
					strcpy_s(TableType, "Log");
					datanum = 5;
					m_State = selectWork;
					break;
				case 6:
					strcpy_s(Table, "item_log_table");
					strcpy_s(TableType, "Log");
					datanum = 5;
					m_State = selectWork;
					break;
					
				default:
					printf(" �߸��Է��ϼ̽��ϴ� \n\n");
					break;
				}
			}
			else
			{
				break;
			}
			
			break;
		case selectWork:
			//�Է¹��� ���̺� Ÿ���� log������ �ƴѰ�� ����
			if (strcmp(TableType, "NotLog")==0)
			{
				//�Է�, ����, �˻�, �������
				printf(" � �۾��� �Ͻðڽ��ϱ�? \n");
				printf("1. �Է�\n");
				printf("2. ����\n");
				printf("3. ����\n");
				printf("4. �˻�\n");
				scanf_s(" %d", &scan_int);
				printf("\n");

				//������ ��ɿ����� ������Ʈ ����
				switch (scan_int)
				{
				case 1:
					m_State = Insert;
					break;
				case 2:
					m_State = Update;
					break;
				case 3:
					m_State = Delete;
					break;
				case 4:
					m_State = Search;
					break;
				default:
					printf(" �߸��Է��ϼ̽��ϴ� \n\n");
					break;
				}
			}
			else
			{
				//log������ ���̺��� �˻��� ����
				printf(" log �� ���̺��� ���̺� �˻��ۿ� ���� �ʽ��ϴ�.\n");
				m_State = Search;
			}
			break;
			
		case Insert: //������ �߰��Է�
			//Ŀ�ǵ� ����
			strcpy_s(command, "Insert into ");
			strcat_s(command, Table);
			strcat_s(command, " values (");

			//������ �߰��� Ŀ�ǵ带 �Է¹���
			printf("%s���̺��� �߰��� ������ ������ �Է��ϼ��� : ", Table);
			scanf_s(" %[^\n]s", temp, sizeof(temp));
			strcat_s(command, temp);
			strcat_s(command, "); ");

			printf("%s", command);
			if (mysql_query(&mysql, command))
			{
				//������ ���ڿ��� �����ͼ� �Է�
				printf("mysql query error : %s \n", mysql_error(&mysql));
			}

			printf(" \n �ٸ� �۾��� �̾ �Ͻðڽ��ϱ�? (Y/N) \n");
			scanf_s(" %c", &scan_char);
			if (scan_char == 'Y' || scan_char == 'y')
			{
				m_State = selectTable;
			}
			else
			{
				m_State = exitstate;
			}
			break;
		case Update: //������ ����
			//����� �Է°� ����
			strcpy_s(command, "update  ");
			strcat_s(command, Table);
			strcat_s(command, " set ");
			printf("%s���̺��� �����͸� �����ϼ��� : ", Table);
			printf("ex) pw=0000 where user_code=7\n");
			scanf_s(" %[^\n]s", temp, sizeof(temp));
			strcat_s(command, temp);
			strcat_s(command, "; ");

			printf("%s", command);
			if (mysql_query(&mysql, command))
			{
				//������ ���ڿ��� �����ͼ� �Է�
				printf("mysql query error : %s \n", mysql_error(&mysql));
			}
			printf(" \n �ٸ� �۾��� �̾ �Ͻðڽ��ϱ�? (Y/N) \n");
			scanf_s(" %c", &scan_char);
			if (scan_char == 'Y' || scan_char == 'y')
			{
				m_State = selectTable;
			}
			else
			{
				m_State = exitstate;
			}
			break;
		case Delete: //������ ����
			//����� �Է°� ����
			strcpy_s(command, "delete from  ");
			strcat_s(command, Table);
			strcat_s(command, " where ");
			printf("%s���̺��� ������ ������ ������ �Է��Ͻÿ� : ", Table);
			printf("ex) user_code=8\n");
			scanf_s(" %[^\n]s", temp, sizeof(temp));
			strcat_s(command, temp);
			strcat_s(command, "; ");

			printf("%s", command);
			if (mysql_query(&mysql, command))
			{
				//������ ���ڿ��� �����ͼ� �Է�
				printf("mysql query error : %s \n", mysql_error(&mysql));
			}
			printf(" \n �ٸ� �۾��� �̾ �Ͻðڽ��ϱ�? (Y/N) \n");
			scanf_s(" %c", &scan_char);
			if (scan_char == 'Y' || scan_char == 'y')
			{
				m_State = selectTable;
			}
			else
			{
				m_State = exitstate;
			}
			break;

		case Search: //�˻�
			//�ش� ���̺��� �Էµ� �����͸� ���� �޾ƿ´�.
			strcpy_s(command, "select * from ");
			strcat_s(command, Table);
			//��ɹ����� ������=0
			if (mysql_query(&mysql, command))
			{
				//������ ���ڿ��� �����ͼ� �Է�
				printf("mysql query error : %s \n", mysql_error(&mysql));
			}

			//��� ���� �����͸� ��������
			sql_result = mysql_store_result(&mysql);

			//��ġ�� �Ҷ����� ���پ� ������ null�� �ƴϸ� �ݺ�
			while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
			{
				//������ ���������� ���
				if (datanum == 4)
				{
					printf("%s %s %s %s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
				}
				else if (datanum == 5)
				{
					printf("%s %s %s %s %s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3] ,sql_row[4]);
				}
				else if (datanum == 6)
				{
					printf("%s %s %s %s %s %s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4],sql_row[5]);
				}
				else
				{
					printf("%s %s %s %s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]);
				}
				
			}
			
			printf(" �ٸ� �۾��� �̾ �Ͻðڽ��ϱ�? (Y/N) \n");
			scanf_s(" %c", &scan_char);
			if (scan_char == 'Y' || scan_char == 'y')
			{
				
				m_State = selectTable;
			}
			else
			{
				mysql_free_result(sql_result);
				m_State = exitstate;
			}
			break;

		case exitstate: //����
			printf("���� �����մϴ�.");
			Isplay = false;
			break;

		}

	}
	
	
	
	//mysql ����
	mysql_close(&mysql);
	return 0;
}
