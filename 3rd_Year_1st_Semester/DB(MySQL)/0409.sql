-- 테이블 생성(테스트테이블)
create table testtbl
(id int auto_increment primary key,
username char(3),
age int
);

-- 테이블 값들 추가
insert into testtbl values(null, '우디', 20);
insert into testtbl(username,age) values('버즈',18);
insert into testtbl(username,age) values('제시',17);
insert into testtbl(username,age) values('나나',20);

-- 업데이트 활용
update testtbl set username='키키' where username='버즈';
update testtbl set age=age+2 where username='제시';

-- 판매가격을 정가로 바꿈
update orders set saleprice=(select price from book where bookid=10) where orderid=10;

-- cookdb
delete from testtbl where age>=20; -- 20살이상인 사람만 지워짐
delete from testtbl; -- 내용 다사라짐 틀은 남음 (조건용)
truncate testtbl;	-- 전부다 삭제 auto_increment값까지 삭제

USE cookDB;
create TABLE bigTBL1(select * from employees.employees);
create TABLE bigTBL2(select * from employees.employees);
create TABLE bigTBL(select * from employees.employees);

delete from bigtbl;
truncate bigtbl1;
-- 완전삭제
drop table bigtbl2;
drop table bigtbl1;
drop table bigtbl;


create table membertbl(select userid,username ,addr from usertbl limit 3);
alter table membertbl add primary key (userid); -- 프라이머리키 설정
alter table membertbl drop primary key;	-- 프라이머리키 삭제

drop table buytbl2;
create table buytbl2(select * from buytbl);
alter table buytbl2 add primary key(num);
alter table buytbl2 add constraint fk_buytbl2 foreign key (userid) references usertbl(userid) on update cascade on delete cascade;
alter table buytbl2 drop foreign key fk_buytbl2;
alter table buytbl2 drop index fk_buytbl2;

alter table book add column isbn varchar(13); -- 추가
alter table book drop column isbn; -- 삭제
alter table book modify column isbn int; -- 수정

insert ignore into membertbl values('KHD','강후덜','미국') on duplicate key update username='강후덜',addr='미국';
insert ignore into membertbl values('LSM','이상민','서울');
insert ignore into membertbl values('KSJ','김성주','경기');

