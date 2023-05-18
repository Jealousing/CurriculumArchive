select * from usertbl u left outer join buytbl b on u.userid=b.userid where b.userid is null;

-- 새로운 테이블 생성
USE cookDB;
CREATE TABLE stdTBL 
( stdName VARCHAR(10) NOT NULL PRIMARY KEY, 
  addr CHAR(4) NOT NULL
);
CREATE TABLE clubTBL 
( clubName VARCHAR(10) NOT NULL PRIMARY KEY, 
  roomNo CHAR(4) NOT NULL
);
CREATE TABLE stdclubTBL 
( num int AUTO_INCREMENT NOT NULL PRIMARY KEY, 
  stdName VARCHAR(10) NOT NULL, 
  clubName VARCHAR(10) NOT NULL, 
  FOREIGN KEY(stdName) REFERENCES stdTBL(stdName), 
  FOREIGN KEY(clubName) REFERENCES clubTBL(clubName)
);
INSERT INTO stdTBL VALUES ('강호동', '경북'), ('김제동', '경남'), ('김용만', '서울'), ('이휘재', '경기'), ('박수홍', '서울');
INSERT INTO clubTBL VALUES ('수영', '101호'), ('바둑', '102호'), ('축구', '103호'), ('봉사', '104호');
INSERT INTO stdclubTBL VALUES (NULL, '강호동', '바둑'), (NULL, '강호동', '축구'), (NULL, '김용만', '축구'), (NULL, '이휘재', '축구'), (NULL, '이휘재', '봉사'), (NULL, '박수홍', '봉사');

select * from stdtbl s left outer join stdclubtbl sc on s.stdName = sc.stdName where sc.num is null;

-- 동아리를 기준으로 가입 학생을 출력하되, 가입 학생이 한 명도 없는 동아리도 출력 ( 동아리이름, 동아리방번호, 가입학생이름)
select c.clubname, c.roomno, sc.stdname from clubtbl c left outer join stdclubtbl sc on c.clubName=sc.clubName where sc.stdName is null;

select userid, tel from usertbl union select userid, price from buytbl;

USE cookDB;
CREATE TABLE empTBL(emp CHAR(3), manager CHAR(3), empTel VARCHAR(8));

INSERT INTO empTBL VALUES ('나사장', NULL, '0000');
INSERT INTO empTBL VALUES ('김재무', '나사장', '2222');
INSERT INTO empTBL VALUES ('김부장', '김재무', '2222-1');
INSERT INTO empTBL VALUES ('이부장', '김재무', '2222-2');
INSERT INTO empTBL VALUES ('우대리', '이부장', '2222-2-1');
INSERT INTO empTBL VALUES ('지사원', '이부장', '2222-2-2');
INSERT INTO empTBL VALUES ('이영업', '나사장', '1111');
INSERT INTO empTBL VALUES ('한과장', '이영업', '1111-1');
INSERT INTO empTBL VALUES ('최정보', '나사장', '3333');
INSERT INTO empTBL VALUES ('윤차장', '최정보', '3333-1');
INSERT INTO empTBL VALUES ('이주임', '윤차장', '3333-1-1');

select e1.emp, e1.manager, e2.emptel from emptbl e1 join emptbl e2 on e1.manager= e2.emp where e1.emp='우대리';

-- 총구매액이 1000이 넘는 고객들의 아이디 이름 주소 
select userid, username, addr from usertbl where userid in (select userid  from buytbl group by userid having sum(price*amount)>1000);
select userid, sum(price*amount) 총구매액 from buytbl group by userid;

select userid, username, addr from usertbl where userid in (select userid  from (select userid, sum(price*amount) 총구매액 from buytbl group by userid)tb where tb.총구매액>1000);

select userid, username, addr from usertbl where userid in (select userid  from umaxprice tb where tb.총구매액>1000);
create view umaxprice as (select userid, sum(price*amount) 총구매액 from buytbl group by userid);

select u.userid, u.username, u.addr, um.총구매액 from usertbl u join umaxprice um on u.userid=um.userid;
