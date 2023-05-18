alter table buytbl drop primary key;
alter table buytbl modify column num int not null, drop primary key; -- primarykey 삭제전에 실행해야됨.
alter table buytbl change num number int not null; -- change는 이름 변경까지 해주고 싶을 때
alter table buytbl modify column number int not null auto_increment, add primary key(number); 
desc buytbl;

show index from buytbl;
show create table buytbl;
alter table buytbl drop foreign key buytbl_ibfk_1;
alter table buytbl drop index buytbl_ibfk_1;
alter table buytbl add constraint fk_usertbl foreign key (userid) references usertbl(userid)
on update cascade on delete cascade;

set foreign_key_checks=0;
DROP TABLE IF EXISTS userTBL;

CREATE TABLE userTBL 
( userID CHAR(8) NOT NULL PRIMARY KEY, 
  userName VARCHAR(10) NOT NULL, 
  birthYear INT NOT NULL, 
  email CHAR(30) NULL, 
  CONSTRAINT AK_email UNIQUE (email) 
);

alter table usertbl drop constraint Ak_email;
show index from usertbl;

DROP TABLE IF EXISTS buyTBL, userTBL;
CREATE TABLE userTBL 
( userID CHAR(8) NOT NULL PRIMARY KEY, 
  userName VARCHAR(10) NOT NULL, 
  birthYear INT NOT NULL DEFAULT -1, 
  addr CHAR(2) NOT NULL DEFAULT '서울', 
  mobile1 CHAR(3) NULL, 
  mobile2 CHAR(8) NULL, 
  height SMALLINT NULL DEFAULT 170, 
  mDate DATE NULL 
);

INSERT INTO userTBL VALUES ('YJS', '유재석', default, default, '010', '11111111', default, '2008-8-8');
INSERT INTO userTBL VALUES ('KHD', '강호동', 1970, '경북', '011', '22222222', 182, '2007-7-7');
INSERT INTO userTBL VALUES ('KKJ', '김국진', 1965, '서울', '019', '33333333', 171, '2009-9-9');

alter table usertbl modify column birthYear int not null default 1972;

INSERT INTO userTBL VALUES ('KYM', '김용만', 1967, '서울', '010', '44444444', default, '2015-5-5');
INSERT INTO userTBL VALUES ('KJD', '김제동', 1974, '경남', NULL , NULL, 173, '2013-3-3');
INSERT INTO userTBL VALUES ('NHS', '남희석', 1971, '충남', '016', '66666666', default, '2017-4-4');
INSERT INTO userTBL VALUES ('SDY', '신동엽', 1971, '경기', NULL, NULL, 176, '2008-10-10');
INSERT INTO userTBL VALUES ('LHJ', '이휘재', 1972, '경기', '011', '88888888', 180, '2006-4-4');
INSERT INTO userTBL VALUES ('LKK', '이경규', 1960, '경남', '018', '99999999', 170, '2004-12-12');
INSERT INTO userTBL VALUES ('PSH', '박수홍', 1970, '서울', '010', '00000000', 183, '2012-5-5');

show create table userTBL;

alter table userTBL add constraint ck_birthyear check(birthyear>=1900 and birthyear<=2021);
INSERT INTO userTBL VALUES ('kja', '김진아', 1865, '서울', '010', '00000000', 170, curdate());
alter table userTBL drop constraint ck_birthyear;

CREATE DATABASE IF NOT EXISTS compressDB;
USE compressDB;
CREATE TABLE normalTBL (emp_no INT, first_name VARCHAR(14));
CREATE TABLE compressTBL (emp_no INT, first_name VARCHAR(14)) 
      ROW_FORMAT=COMPRESSED;
      
INSERT INTO normalTbl 
      SELECT emp_no, first_name FROM employees.employees;
INSERT INTO compressTBL 
      SELECT emp_no, first_name FROM employees.employees;

USE employees;
CREATE TEMPORARY TABLE IF NOT EXISTS tempTBL (id INT, userName CHAR(5));
CREATE TEMPORARY TABLE IF NOT EXISTS employees (id INT, userName CHAR(5));
DESCRIBE tempTBL;
DESCRIBE employees;


 -- ppt ch09 36~46 해보기
USE tableDB;
DROP TABLE IF EXISTS buyTBL, userTBL;

CREATE TABLE userTBL 
( userID CHAR(8), 
  userName VARCHAR(10), 
  birthYear INT, 
  addr CHAR(2), 
  mobile1 CHAR(3), 
  mobile2 CHAR(8), 
  height SMALLINT, 
  mDate DATE
);

CREATE TABLE buyTBL 
( num INT AUTO_INCREMENT PRIMARY KEY, 
  userID CHAR(8), 
  prodName CHAR(6), usertbl
  groupName CHAR(4), 
  price INT, 
  amount SMALLINT 
);

