#pragma comment (lib, "libmysql.lib")
#include <stdio.h>
#include <mysql.h>
#include <string.h>

#define HOST "127.0.0.1" //서버주소
#define PORT 3306 //mysql 포트번호
#define USER "root" //계정명
#define PASSWORD "1234" //비밀번호
#define DATABASE "gamedb" //접근 데이터베이스 이름

//DB control의 state
enum State
{
	init = 0,
	selectDB,			//Db 선택
	selectTable,		//table 선택
	selectWork,		//할일 선택
	Insert,				//입력
	Update,				//수정
	Delete,				//삭제
	Search,			//검색
	exitstate			//종료
}m_State;

//gamedb의 테이블명
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
	MYSQL mysql; //연결할때 사용
	MYSQL_RES* sql_result; //결과
	MYSQL_ROW sql_row; //바인딩할 때 사용

	mysql_init(&mysql); //초기화

	//scanf용 변수
	char scan_char;
	int scan_int;
	
	// 문자열 변수 
	char DB[100];				//db이름
	char Table[100];			//테이블명
	char TableType[100];	//테이블 타입
	char command[100]; //커맨드용

	//data의 갯수
	int datanum = 0;

	//dbcontrol 진행여부
	bool Isplay = true;

	//temp 문자열
	char temp[100];
	char temp2[100];
	
	while (Isplay)
	{
		//메모리초기화
		memset(command, 0, sizeof(command));
		memset(temp, 0, sizeof(temp));
		memset(temp2, 0, sizeof(temp2));

		//스테이트따라 진행
		switch (m_State)
		{
		case init: //처음 진입 intro
			printf(" Mysql DB를 관리하는 프로그램입니다\n 진행(Y/N)");
			scanf_s(" %c", &scan_char);
			if (scan_char == 'Y' || scan_char == 'y')
				m_State = selectDB;
			else
				m_State = exitstate;
			break;
		case selectDB: //db선택
			printf(" 연결하실 DB를 선택하세요\n ");
			printf(" 1. GameDB \n");
			scanf_s("%d", &scan_int);
			printf("\n");
			switch (scan_int)
			{
			case 1:
				//db명 저장
				strcpy_s(DB, "GameDB");
				//해당 데이터베이스 연결
				//연결시도  (연결용, 서버주소, 계정명, 비밀번호, 연결할 데이터베이스, 포트번호, 소켓, flag)
				if (!mysql_real_connect(&mysql, HOST, USER, PASSWORD, DATABASE, PORT, NULL, 0))
				{
					//에러난 문자열을 가져와서 입력
					printf("mysql connect error : %s \n", mysql_error(&mysql));
					return -1;
				}
				//한국어 출력을 위한 설정
				if (mysql_query(&mysql, "set session character_set_results = euckr"))
				{
					//에러난 문자열을 가져와서 입력
					printf("mysql query error : %s \n", mysql_error(&mysql));
				}
				//스테이트 변경
				m_State = selectTable;
				break;
			default:
				printf("없는 번호를 입력하셨습니다,\n 재시작(Y/N)");
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
		case selectTable: //테이블 선택
			//선택한 db가 gamedb일경우 실행
			if (strcmp(DB, "GameDB")==0)
			{
				//테이블 선택
				printf(" 어떤 테이블에서 작업하시겠습니까? \n");
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
					//선택한 테이블에 따라 설정
					//table 명 설정
					//테이블 타입설정
					//data갯수 설정
					//스테이트 설정
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
					printf(" 잘못입력하셨습니다 \n\n");
					break;
				}
			}
			else
			{
				break;
			}
			
			break;
		case selectWork:
			//입력받은 테이블 타입이 log형식이 아닌경우 실행
			if (strcmp(TableType, "NotLog")==0)
			{
				//입력, 수정, 검색, 삭제기능
				printf(" 어떤 작업을 하시겠습니까? \n");
				printf("1. 입력\n");
				printf("2. 수정\n");
				printf("3. 삭제\n");
				printf("4. 검색\n");
				scanf_s(" %d", &scan_int);
				printf("\n");

				//선택한 기능에따라 스테이트 변경
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
					printf(" 잘못입력하셨습니다 \n\n");
					break;
				}
			}
			else
			{
				//log저장형 테이블은 검색만 실행
				printf(" log 형 테이블은 테이블 검색밖에 되지 않습니다.\n");
				m_State = Search;
			}
			break;
			
		case Insert: //데이터 추가입력
			//커맨드 세팅
			strcpy_s(command, "Insert into ");
			strcat_s(command, Table);
			strcat_s(command, " values (");

			//데이터 추가할 커맨드를 입력받음
			printf("%s테이블의 추가할 데이터 정보를 입력하세요 : ", Table);
			scanf_s(" %[^\n]s", temp, sizeof(temp));
			strcat_s(command, temp);
			strcat_s(command, "); ");

			printf("%s", command);
			if (mysql_query(&mysql, command))
			{
				//에러난 문자열을 가져와서 입력
				printf("mysql query error : %s \n", mysql_error(&mysql));
			}

			printf(" \n 다른 작업을 이어서 하시겠습니까? (Y/N) \n");
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
		case Update: //데이터 수정
			//방식은 입력과 동일
			strcpy_s(command, "update  ");
			strcat_s(command, Table);
			strcat_s(command, " set ");
			printf("%s테이블의 데이터를 수정하세요 : ", Table);
			printf("ex) pw=0000 where user_code=7\n");
			scanf_s(" %[^\n]s", temp, sizeof(temp));
			strcat_s(command, temp);
			strcat_s(command, "; ");

			printf("%s", command);
			if (mysql_query(&mysql, command))
			{
				//에러난 문자열을 가져와서 입력
				printf("mysql query error : %s \n", mysql_error(&mysql));
			}
			printf(" \n 다른 작업을 이어서 하시겠습니까? (Y/N) \n");
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
		case Delete: //데이터 삭제
			//방식은 입력과 동일
			strcpy_s(command, "delete from  ");
			strcat_s(command, Table);
			strcat_s(command, " where ");
			printf("%s테이블의 삭제할 데이터 조건을 입력하시오 : ", Table);
			printf("ex) user_code=8\n");
			scanf_s(" %[^\n]s", temp, sizeof(temp));
			strcat_s(command, temp);
			strcat_s(command, "; ");

			printf("%s", command);
			if (mysql_query(&mysql, command))
			{
				//에러난 문자열을 가져와서 입력
				printf("mysql query error : %s \n", mysql_error(&mysql));
			}
			printf(" \n 다른 작업을 이어서 하시겠습니까? (Y/N) \n");
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

		case Search: //검색
			//해당 테이블의 입력된 데이터를 전부 받아온다.
			strcpy_s(command, "select * from ");
			strcat_s(command, Table);
			//명령문실행 정상적=0
			if (mysql_query(&mysql, command))
			{
				//에러난 문자열을 가져와서 입력
				printf("mysql query error : %s \n", mysql_error(&mysql));
			}

			//결과 셋의 포인터를 리턴해줌
			sql_result = mysql_store_result(&mysql);

			//패치를 할때마다 한줄씩 가져옴 null이 아니면 반복
			while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
			{
				//데이터 갯수에따라 출력
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
			
			printf(" 다른 작업을 이어서 하시겠습니까? (Y/N) \n");
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

		case exitstate: //종료
			printf("정상 종료합니다.");
			Isplay = false;
			break;

		}

	}
	
	
	
	//mysql 종료
	mysql_close(&mysql);
	return 0;
}
