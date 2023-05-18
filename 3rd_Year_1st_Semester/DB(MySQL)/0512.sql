alter table usertbl add grade varchar(10);
delimiter //
create procedure gradproc()
begin
	declare id varchar(10);
    declare total int;
    declare usergrade varchar(10);
    declare endofrow boolean default false;
    
    declare usercursor cursor for
		select u.userid, sum(b.price * b.amount) from buytbl b
        right outer join usertbl u on b.userid=u.userid
        group by u.userid;
        
	declare continue handler for not found
		set endofrow = true;
        
	open usercursor;
    
    cursor_loop:loop
		fetch usercursor into id, total;
        
        if endofrow then
			leave cursor_loop;
		end if;
        
        case
        when total >= 1500 then set usergrade  = '최우수고객';
        when total >= 1000 then set usergrade  = '우수고객';
        when total >= 1 then set usergrade  ='일반고객';
        else set usergrade='유령고객';
        end case;
        
        update usertbl set grade =usergrade where userid = id;
        
	end loop cursor_loop;
    close usercursor;
end//
delimiter ;

call gradproc;

set @height = 180;
prepare testquery from 'select * from usertbl where height=?';
execute testquery using @height;

set @t_query='select * from usertbl where height=?';
prepare testquery from @t_query;
execute testquery using @height;

set @t_name =concat('tbl_',date_format(curdate(),'%Y_%m_%d'));
select @t_name;
set @create_query = concat('create table ',@t_name,'(id int,txt varchar(10))');
select @create_query;
prepare create_query from @create_query;
execute create_query;

-- 매달 그 때 까지의 1. 구매실적을 바탕으로 2. 가입된지 기간을 바탕으로 할인율을 책정해서 저장하는 테이블
-- -------------------------------------------------
-- 매달 그때까지의 유저의 구매실적이나 가입된기간을 바탕으로 할인율 을 측정해서 저장하는 테이블
-- 1. 변수 선언
-- 2. 커서 선언 (userid, joindate, sum())
-- 3. 커서가 끝났을 때 예외처리 (declare continue handler for not found)
-- 4. create tbl_2021_05_12.....
-- 5. 커서 열기
-- 6. 커서 루프
-- (fetch 커서 into id, joindate, total)
 -- joindate, total 로 할인률 구하기 case when....
 -- insert into tbl values (id, 할인률);
-- 7. 커서 닫기
-- -------------------------------------------------
drop procedure if exists update_discount_rate;
delimiter //
create procedure update_discount_rate()
begin
	declare t_name varchar(20);
	declare u_id varchar(10);
    declare total int;
    declare jdate date;
    declare rate dec(3,2);
    declare t_month int;
    declare endofrow boolean default false;
    
    declare usercursor cursor for
		select u.userid ,u.joindate,sum(b.price * b.amount)  from buytbl b
        right outer join usertbl u on b.userid=u.userid
        group by u.userid;
        
	declare continue handler for not found
		set endofrow = true;
        
	set t_name = concat('tbl_',date_format(curdate(),'%Y_%m_%d'));
	set @create_query=concat('create table ',t_name, '(userid varchar(10),discount_rate dec(3,2))');
	set @drop_query=concat('drop table if exists ',t_name);
	prepare create_query from @create_query;
    prepare drop_query from @drop_query;
    execute drop_query;
	execute create_query;
        
	open usercursor;
    
    cursor_loop:loop
		fetch usercursor into u_id,jdate, total;
        
        if endofrow then
			leave cursor_loop;
		end if;
        
        set t_month=datediff(curdate(),jdate)/30;
        
        case
        when total >= 1500 or t_month >= 190 then set rate  = 0.3;
        when total >= 1000 or t_month >= 100 then set rate  = 0.2;
        when total >= 100 or t_month >= 30  then set rate  =0.1;
        else set rate=0;
        end case;
        
        set @insert_query=concat('insert into ',t_name,' values("',u_id,'",',rate,')');
        prepare insert_query from @insert_query;
        execute insert_query;
        
	end loop cursor_loop;
    close usercursor;
end//
delimiter ;

call update_discount_rate;
