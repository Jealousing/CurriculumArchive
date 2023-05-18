-- 1.
alter table order_table add column count int default 1;
-- 2.
alter table book_table add column count int default 100;
-- 3.
alter table customer_table add column cust_grade varchar(10) default 'Bronze';
-- 4.
create table cust_grade_updateinfo(
	id int auto_increment primary key,
	custid int,
	update_date date,
    pre_grade varchar(10),
    update_grade varchar(10),
    constraint fk_custid foreign key(custid) references customer_table(custid)
);
-- 5.
drop procedure if exists batch_update_grade;
delimiter //
create procedure batch_update_grade()
begin
	declare c_id int;
    declare t_saleprice int;
    declare old_grade varchar(10);
    declare new_grade varchar(10);
    declare endofrow bool default false;
    
    declare usercursor cursor for
		select c.custid, sum(od.saleprice), c.cust_grade from customer_table c
         right outer join order_table od on od.custid = c.custid group by c.custid;
        
	declare continue handler for not found
		set endofrow = true;
	
    open usercursor;
    
    cursor_loop : loop
		fetch usercursor into c_id, t_saleprice,old_grade;
        
        if endofrow then
			leave cursor_loop;
        end if;
        
        case
        when t_saleprice >= 100000 then set new_grade = 'Vip';
        when t_saleprice >= 50000 then set new_grade = 'Gold';
        when t_saleprice >= 30000 then set new_grade = 'Shilver';
        else set new_grade = 'Bronze';
        end case;
        
        if new_grade != old_grade then
			update customer_table set cust_grade = new_grade where custid = c_id;
        end if;
	end loop cursor_loop;
    close usercursor;
end //
delimiter ;
-- 6.
drop trigger if exists update_grade_trg;
delimiter //
create trigger update_grade_trg
after update on customer_table for each row
begin
	insert into cust_grade_updateinfo values(null, old.custid, curdate(), old.cust_grade, new.cust_grade);
end //
delimiter ;

call batch_update_grade();
-- 7.
drop procedure if exists orderbook;
delimiter //
create procedure orderbook(in c_id int, in b_name varchar(20), in cnt int)
begin
	declare b_id int;
    declare b_price int;
    declare c_grade varchar(10);
    declare s_price int;
    
    declare exit handler for not found
    begin
		select '없는 도서입니다';
        rollback; -- 명령하나 취소
    end;
    
    select bookid, price into b_id, b_price from book_table where bookname = b_name;
    select cust_grade into c_grade from customer_table where custid = c_id;
    
    case 
    when c_grade = 'Vip' then set s_price = (b_price*cnt)*0.85;
	when c_grade = 'Gold' then set s_price = (b_price*cnt)*0.90;
	when c_grade = 'Shilver' then set s_price = (b_price*cnt)*0.95;
    else set s_price = (b_price*cnt);
    end case;
    
    insert into order_table values(null, c_id, b_id, s_price, curdate(), cnt);
    
end //
delimiter ;

drop trigger if exists order_insert_trg;
delimiter //
create trigger order_insert_trg
before insert on order_table for each row
begin
	declare b_cnt int;
    select count into b_cnt from book_table where bookid = new.bookid;
    if b_cnt < new.count then
		signal sqlstate '45000'
			set message_text = '서적 수량이 부족합니다';
	end if;
    
    update book_table set count = count - new.count where bookid = new.bookid;
end //
delimiter ;

drop trigger if exists calculate_grade_trg;
delimiter //
create trigger calculate_grade_trg
after insert on order_table for each row
begin
	declare t_saleprice int;
    declare old_grade varchar(10);
    declare new_grade varchar(10);
    
    select sum(saleprice) into t_saleprice from order_table where custid = new.custid;
    select cust_grade into old_grade from customer_table where custid = new.custid;
    
    case
	when t_saleprice >= 100000 then set new_grade = 'Vip';
    when t_saleprice >= 50000 then set new_grade = 'Gold';
    when t_saleprice >= 30000 then set new_grade = 'Shilver';
    else set new_grade = 'Bronze';
    end case;
    
    if new_grade != old_grade then
		update customer_table set cust_grade = new_grade where custid = new.custid;
    end if;
    
end //
delimiter ;


describe order_table;
desc order_table;

-- 기본키 제약 조건 2-2


