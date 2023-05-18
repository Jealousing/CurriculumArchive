use cookDB;
DROP TABLE buyTBL;
CREATE TABLE backup_userTBL
(
    userID char(8) NOT NULL, 
	userName varchar(10) NOT NULL, 
	birthYear int NOT NULL, 
	addr char(2) NOT NULL,
	mobile1 char(3), 
	mobile2 char(8), 
	height smallint, 
	mDate date, 
	modType char(2), -- 변경된 유형(‘수정’ 또는 ‘삭제’) 
	modDate date, -- 변경된 날짜 
	modUser varchar(256) -- 변경한 사용자 
);

drop trigger if exists backUsertbl_updatetrg;
delimiter //
create trigger backUsertbl_updatetrg
after update on usertbl for each row
begin

	insert into backup_userTBL values(old.userID, old.username, old.birthyear,
    old.addr, old.mobile1,old.mobile2, old.height,old.mDate,'수정',curdate(),current_user());
    
end //
delimiter ;

drop trigger if exists backUsertbl_deletetrg;
delimiter //
create trigger backUsertbl_deletetrg
after delete on usertbl for each row
begin

	insert into backup_userTBL values(old.userID, old.username, old.birthyear,
    old.addr, old.mobile1,old.mobile2, old.height,old.mDate,'삭제',curdate(),current_user());
    
end //
delimiter ;

update usertbl set addr='제주' where userid='kjd';
delete from usertbl where height>=180;

drop trigger if exists usertbl_inserttrg;
delimiter //
create trigger usertbl_inserttrg
before insert on usertbl for each row
begin
	 if NEW.userID='abc' then
		 signal sqlstate '45000'
		 set message_text='입력을 시도했습니다. 입력불가 테이블입니다.';
	 end if;
end //
delimiter ;

drop trigger if exists usertbl_inserttrg;
delimiter //
create trigger usertbl_inserttrg
before insert on usertbl for each row
begin
	 if new.birthYear<1900 then
		 set new.birthYear=0;
	elseif new.birthYear>year(curdate())then
		set new.birthYear=year(curdate());
	end if;
end //
delimiter ;

insert into usertbl values('bbb','에비씨',1888,'서울','010','11111111',181,'2019-12.25');


DROP DATABASE IF EXISTS triggerDB;
CREATE DATABASE IF NOT EXISTS triggerDB;
USE triggerDB;
CREATE TABLE orderTBL -- 구매 테이블 
( orderNo INT AUTO_INCREMENT PRIMARY KEY, -- 구매 일련번호 
  userID VARCHAR(5), -- 구매한 회원 아이디 
  prodName VARCHAR(5), -- 구매한 물건 
  orderamount INT -- 구매한 개수 
);
CREATE TABLE prodTBL -- 물품 테이블 
( prodName VARCHAR(5), -- 물건 이름 
  account INT -- 남은 물건 수량 
);
CREATE TABLE deliverTBL -- 배송 테이블 
( deliverNo INT AUTO_INCREMENT PRIMARY KEY, -- 배송 일련번호 
  prodName VARCHAR(5), -- 배송할 물건 
  account INT  -- 배송할 물건 개수 
);
INSERT INTO prodTBL VALUES ('사과', 100);
INSERT INTO prodTBL VALUES ('배', 100);
INSERT INTO prodTBL VALUES ('귤', 100);

drop trigger if exists order_inserttrg;
delimiter //
create trigger order_inserttrg
after insert on orderTBL for each row
begin 
	update prodTBL set account = account -new.orderamount where prodname=new.prodname;
end //
delimiter ;

drop trigger if exists prodtbl_updatetrg;
delimiter //
create trigger prodtbl_updatetrg
after update on prodtbl for each row
begin 
	declare orderamount int;
    set orderamount=old.account - new.account;
    insert into delivertbl values(null, new.prodname, orderamount);
end //
delimiter ;

insert into ordertbl values(null,'john','배',5);
insert into ordertbl values(null,'kja','귤',10);
insert into ordertbl values(null,'sujee','사과',8);