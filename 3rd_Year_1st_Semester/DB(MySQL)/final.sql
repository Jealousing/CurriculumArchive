
-- 1) GameDB를 생성하고 그 안에 다음과 같은 세 개의 Table를 생성한 다음 레코드를 추가한다.
create schema GameDB; -- DB생성
use GameDB; -- 사용

-- User table
create table User_table
(
	User_code int auto_increment not null primary key,
	User_id char(20) not null unique,
    Pw int not null,
    Name char(20)
);
-- Character table
create table Character_table
(
	Nick_name char(20) not null primary key,
    User_id char(20) not null,
    Lv int not null,
    Job char(20),
    Gold int not null,
    
    foreign key (User_id) references User_table(User_id) on update cascade on delete cascade
);

-- Item table
create table item_table
(
	Item_code int auto_increment not null primary key,
	item_name char(20) not null,
    Owner_name char(20),
    Limit_Lv int not null,
    
    foreign key (Owner_name) references Character_table(Nick_name) on update cascade on delete cascade
);
--  - 모든 테이블은 primary key가 존재하고, Foreign key가 존재해야 한다.
-- - 설정된 Foreign key는 참조되는 테이블에서 수정 및 삭제작업이 일어날 시 같이 연동되어야 한다.
-- - 필드 구성은 필요에 따라 자유롭게 한다.
-- (Table 생성 Query, Insert Query, 결과 화면 Capture)

-- User_table insert into ( uesr_id, pw, name )
insert into User_table values (0,'aaa',1111,'가가가');
insert into User_table values (0,'bbb',2222,'나나나');
insert into User_table values (0,'ccc',3333,'다다다');
insert into User_table values (0,'ddd',4444,'라라라');
insert into User_table values (0,'eee',5555,'마마마');

-- Character table insert into ( nick_name, user_id, Lv, Job, Gold )
 insert into Character_table values ('nink_a','aaa',1,'Beginner',0);
 insert into Character_table values ('nink_b','bbb',1,'Beginner',0);
 insert into Character_table values ('nink_c','ccc',1,'Beginner',0);
 insert into Character_table values ('nink_d','ddd',1,'Beginner',0);
 insert into Character_table values ('nink_e','eee',1,'Beginner',0);
 
 -- Item table insert into ( item_name, Owner_name, Limit_Lv )
 insert into item_table values (0,'Beginner_sword','nink_a',0);
 insert into item_table values (0,'Beginner_sword','nink_b',0);
 insert into item_table values (0,'Beginner_sword','nink_c',0);
 insert into item_table values (0,'Beginner_sword','nink_d',0);
 insert into item_table values (0,'Beginner_sword','nink_e',0);

-- 2) 각 Table에 update가 일어날 때 변경 내용을 저장하는 Log Table을 생성하고, 각 Table에 Update Trigger를 생성하여 Log Table에 record를 insert 하게 한다.
-- (Table 생성 Query, Update Trigger 생성 Query, 각 Table에 Update가 일어나고 변화된 Log Table의 화면 Capture)
CREATE TABLE User_log_table
(
    Log_code int primary KEY auto_increment, 
    User_id  char(20) not null, 
    Update_date date, 
    pre_pw varchar(10), 
    update_pw varchar(10), 
    foreign key(User_id) references User_table(User_id)
);

drop trigger if exists Update_UserPw_trg;
delimiter //
create trigger Update_UserPw_trg
after update on User_table for each row
begin 
	insert into User_log_table values(null, old.User_id, curdate(), old.Pw,new.Pw);
end //
delimiter ;

CREATE TABLE Character_log_table
(
    Log_code int primary KEY auto_increment, 
    User_id  char(20) not null, 
    Update_date date, 
    pre_Lv varchar(10), 
    update_Lv varchar(10), 
    foreign key(User_id) references User_table(User_id)
);

drop trigger if exists Update_CharacterLV_trg;
delimiter //
create trigger Update_CharacterLV_trg
after update on Character_table for each row
begin 
	insert into Character_log_table values(null, old.User_id, curdate(), old.Lv,new.Lv);
end //
delimiter ;

CREATE TABLE item_log_table
(
    Log_code int primary KEY auto_increment, 
    Item_code int , 
    Update_date date, 
    pre_Owner_name varchar(10), 
    update_Owner_name varchar(10), 
    foreign key(Item_code) references item_table(Item_code)
);

drop trigger if exists Update_ItemOwner_trg;
delimiter //
create trigger Update_ItemOwner_trg
after update on item_table for each row
begin 
	insert into item_log_table values(null, old.Item_code, curdate(), old.Owner_name,new.Owner_name);
end //
delimiter ;

-- 3) User Table에 Insert할 수 있는 조건을 만들고 그 조건에 맞지 않는 데이터를 Insert하려고 하면 Error Message를 출력하고 Insert를 취소하는 Trigger 생성
-- (Trigger 생성 Query, 조건에 맞지 않는 데이터 Insert 시 결과화면 Capture)
drop trigger if exists Insert_User_Table_trg;
delimiter //
create trigger Insert_User_Table_trg
after insert on User_table for each row
begin
	
    if new.User_id like '%admin%' then
		signal sqlstate '45000'
			set message_text = '아이디에에 admin을 포함할 수 없습니다.';
	end if;
    
end //
delimiter ;


-- 4) CurSor를 활용한 Procedure를 하나 이상 구현한다.
-- - Procedure 내부에 RollBack이 될 수 있는 Transaction을 포함한다. (Procedure 내용 자유)
-- (Procedure 생성 및 실행화면 Capture)
alter table Character_table add grade varchar(10);
drop procedure if exists gradproc;
delimiter //
create procedure gradproc()
begin
	declare p_user_id char(20);
    declare p_lv int;
    declare p_grade varchar(10);
    declare endofrow boolean default false;
    declare err int default '0';
    
    declare usercursor cursor for
		select c.User_id , c.Lv from Character_table c;
        
	declare continue handler for not found
		set endofrow = true;
        
	start transaction;
	open usercursor;
    
    cursor_loop:loop
		fetch usercursor into p_user_id, p_lv;
        
        if endofrow then
			leave cursor_loop;
		end if;
        
        case
        when p_lv >= 70 then set p_grade  = '고수';
        when p_lv >= 30 then set p_grade  = '중수';
        when p_lv >= 2 then set p_grade  ='초보';
        when p_lv >= 1 then set p_grade  ='신규유저';
        else set err = -1;
        end case;
        
        update Character_table set grade =p_grade where User_id = p_user_id;
        
	end loop cursor_loop;
    close usercursor;
    
    if err < 0 then
		set p_lv=1;
		rollback;
	else
		commit;
	end if;
    
end//
delimiter ;

call gradproc;

-- 5) Mysql Server 안에 GameDB를 접근하여 내용을 입력, 수정, 검색, 삭제기능을 하는 C++ Application을 구현하여 코드를 제출한다.