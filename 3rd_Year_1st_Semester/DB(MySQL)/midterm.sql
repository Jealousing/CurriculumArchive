create schema GameDB; -- DB생성
use GameDB; -- 사용

-- 1 게임에 사용될 수 있는 테이블을 3개 이상 생성한다.

-- 유저 정보 테이블
create table User_table
(
	id char(20) not null primary key, -- 아이디는 빈공간 없이 프라이머리 키로 설정
    pw int not null, -- 비밀번호도 빈공간x
    _name char(20)  -- 이름
);

-- 길드 정보 테이블
create table Guild_table
(
	guild_name char(20) not null primary key, -- 길드 이름
    _master char(20) not null, -- 길드 마스터
    funds int not null, -- 길드 자금
    creat_date date not null, -- 길드 생성일
    
    -- 포링키 설정 길드마스터
    foreign key (_master) references User_table(id) on update cascade on delete cascade
);

-- 캐릭터 정보 테이블
create table Character_table
(
	nickname char(20) not null primary key,		  -- 닉네임
    user_id char(20) not null,	-- 유저 아이디
    guild char(20) not null,	-- 소속 길드
    lv int not null,			  -- 레벨
    job char(20),				  -- 직업
    gold int not null,			  -- 골드
    
    -- 포링키 설정 아이디 / 길드이름
    foreign key (user_id) references user_table(id) on update cascade on delete cascade ,
    foreign key (guild) references Guild_table(guild_name) on update cascade on delete cascade
);

-- 유저 테이블 정보 추가
insert into User_table values ('aaa',1234,'가가가');
insert into User_table values ('bbb',1234,'나나나');
insert into User_table values ('ccc',1234,'다다다');
insert into User_table values ('ddd',1234,'라라라');
insert into User_table values ('eee',1234,'마마마');
insert into User_table values ('fff',1234,'바바바');
insert into User_table values ('ggg',1234,'사사사');
insert into User_table values ('hhh',1234,'아아아');
insert into User_table values ('iii',1234,'자자자');
insert into User_table values ('jjj',1234,'차차차');
insert into User_table values ('kkk',1234,'카카카');
insert into User_table values ('lll',1234,'타타타');
insert into User_table values ('mmm',1234,'파파파');
insert into User_table values ('nnn',1234,'하하하');

-- 길드 테이블 정보 추가
insert into Guild_table values ('길드이름1','aaa',35000000,'2018-10-03');
insert into Guild_table values ('길드이름2','bbb',65000000,'2015-01-13');
insert into Guild_table values ('길드이름3','ccc',15000000,'2020-03-24');

-- 케릭터 테이블 정보 추가
insert into Character_table values ('별명1','aaa','길드이름1',80,'전사',800000);
insert into Character_table values ('별명2','aaa','길드이름1',30,'법사',50000);
insert into Character_table values ('별명3','aaa','길드이름1',10,'도적',132100);
insert into Character_table values ('별명4','bbb','길드이름2',85,'도적',4445501);
insert into Character_table values ('별명5','bbb','길드이름2',15,'사제',11111);
insert into Character_table values ('별명6','bbb','길드이름2',45,'궁수',444111);
insert into Character_table values ('별명7','ccc','길드이름3',90,'사제',1427501);
insert into Character_table values ('별명8','ccc','길드이름3',55,'전사',1343411);
insert into Character_table values ('별명9','ccc','길드이름3',35,'궁수',4111);
insert into Character_table values ('별명10','ddd','길드이름1',67,'도적',4211501);
insert into Character_table values ('별명11','ddd','길드이름1',5,'사제',999411);
insert into Character_table values ('별명12','ddd','길드이름1',55,'전사',111);
insert into Character_table values ('별명13','eee','길드이름2',35,'전사',124211);
insert into Character_table values ('별명14','fff','길드이름3',95,'사제',5112411);
insert into Character_table values ('별명15','ggg','길드이름4',88,'전사',1567373);
insert into Character_table values ('별명16','hhh','길드이름1',91,'사제',11);
insert into Character_table values ('별명14','iii','길드이름3',65,'도적',424212);
insert into Character_table values ('별명14','jjj','길드이름2',11,'궁수',1134);
insert into Character_table values ('별명15','kkk','길드이름3',34,'법사',132342);
insert into Character_table values ('별명16','lll','길드이름1',45,'궁수',222222);
insert into Character_table values ('별명17','mmm','길드이름2',1,'법사',100);
insert into Character_table values ('별명18','nnn','길드이름1',30,'사제',42412);

-- 2 다음 조건에 맞는 query문을 각각 하나씩 작성한다.

-- and | or로 구성되는 where절을 포함한 Select문(단일)
select * from Character_table where lv>=80 and gold>=1000000; -- 레벨 80 이상 골드 100만원 이상 들고있는 케릭터의 정보 찾기

-- group by와 집계함수가 포함된 Select문(단일)
select nickname, guild,max(lv) from Character_table group by guild; -- 길드를 그룹으로 길드내에 가장 레벨이 높은사람의 닉네임 찾기

-- 집계함수가 사용된 독립 Sub query를 포함하는 중첩 Query문(Select)
select * from Character_table where guild in (select guild_name from Guild_table where creat_date>= '2020-01-01'); -- 2020년후에 생성된 길드에 속한 길드원의 정보

-- 집계함수가 사용된 상관 Sub query를 포함하는 중첩 Query문(Select)
select c1.guild ,c1.nickname, c1.lv from Character_table as c1 where c1.lv=(select max(c2.lv) from Character_table as c2 where c1.guild = c2.guild); -- 길드에 가입된 사람중에 길드에서 가장 레벨이 높은 사람의 정보

-- 테이블 형태의 결과를 만드는 Sub query를 포함한 중첩 Query문(Join)
select g.guild_name as 길드명 ,c.nickname as 캐릭터명,u._name as 유저아이디 from user_table as u join guild_table as g on g._master=u.id join Character_table as c on c.user_id=g._master; -- 길드장의 캐릭터들 조회

-- Query의 결과를 테이블로 저장하고 이 테이블을 사용한 Query문(Join)
create table master_table(select g.guild_name as 길드명 ,c.nickname as 캐릭터명,u._name as 유저아이디 from user_table as u join guild_table as g on g._master=u.id join Character_table as c on c.user_id=g._master); -- 사용한 쿼리를 테이블로 저장
select * from Character_table c join master_table m where m.캐릭터명 = c.nickname; -- 길드장들의 캐릭터 정보 조회

-- View를 포함한 중첩 Query문(Join)
create view user_money AS select c.user_id as 유저아이디, sum(c.gold) as 가진돈 from Character_table c group by c.user_id; -- view 생성 
select u.id,c.guild,c.lv,um.가진돈 from Character_table as c join user_money as um on um.유저아이디=c.user_id join User_table as u on u.id=um.유저아이디 group by um.유저아이디 ; -- 정리 

-- Update문을 사용하여 레코드를 변경하는 Query문
update User_table set _name='가나다' where _name='가가가'; -- 이름 변경

-- Delete문을 사용하여 레코드를 삭제하는 Query문(Where절 필수)
delete from Character_table where lv<=10; -- 레벨 10 이하 데이터 삭제

-- Alter문을 이용하여 테이블의 제약조건을 변경하는 Query문
alter table Character_table add primary key (nickname); -- 프라이머리키 설정
alter table Character_table drop primary key;	-- 프라이머리키 삭제