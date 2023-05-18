-- 1. madang.orders에 구매한 서적의 개수를 입력할 필드를 추가한다.(기본 값 1)
alter table orders add salecount int default 1;
-- 2. madang.book에 각 서적에 대해 입고된 서적의 개수가 들어 갈 필드를 추가한다.(기본 값 100)
alter table book add account int default 100;
-- 3. madang.customer에 고객등급을 입력할 필드를 추가한다.
alter table customer add grade varchar(10);
-- 4. 고객등급변경내역 테이블을 생성한다.(일련번호,고객아이디, 변경날짜, 이전등급, 변경등급)
CREATE TABLE change_grade
(
    idx int primary KEY NOT NULL auto_increment, 
    custid int NOT NULL, 
    update_date date, -- 변경된 날짜
    grade varchar(10), -- 변경전 등급
    update_grade varchar(10) -- 변경된 등급
);

-- 5. 총 구입액을 기준(기준액 자유)으로 고객등급을 일괄처리하는 프로시저를 생성한다.(vip, gold, silver, bronze)
drop procedure if exists gradproc;
delimiter //
create procedure gradproc()
begin
	declare id int;
    declare total int;
    declare usergrade varchar(10);
    declare endofrow boolean default false;
    
   declare usercursor cursor for
      select c.custid, sum(o.saleprice)  from orders o
         right outer join customer c on o.custid = c.custid
          group by c.custid;
        
    declare continue handler for not found
      set endofrow = true;
      
    open usercursor;
    
    cursor_loop:loop
      fetch usercursor into id, total;
      
		if endofrow then
         leave cursor_loop;
		end if;
        
        case 
        when total >= 35000 then set usergrade = 'Vip';
		when total >= 18000 then set usergrade = 'Gold';
        when total >= 15000 then set usergrade = 'Silver';
        else set usergrade = 'Bronze';
        end case;
        
        update customer set grade = usergrade where custid = id;
    end loop cursor_loop;
    close usercursor;
end //
delimiter ;

call gradproc();

-- 6. 고객등급이 변경되면 변경정보를 고객등급변경내역 테이블에 추가하는 트리거를 생성하고 고객등급을 일괄처리 프로시저를 실행한다.

drop trigger if exists grade_updatetrg;
delimiter //
create trigger grade_updatetrg
after update on customer for each row
begin 
	if old.grade!=new.grade then
		insert into change_grade values(null,Old.custid,curdate(),old.grade,new.grade);
	end if;
    
end //
delimiter ;

update customer set grade='Vip' where custid= 1;

-- 7. 고객이 서적을 주문 시 실행하는 프로시저를 생성한다.
--   - 고객 아이디, 구입할 책의 이름, 구매 개수를 입력(없는 서적에 대한 오류 처리 포함)
--   - saleprice는 고객의 등급에 따라 결정된다.
drop procedure if exists buyproc;
delimiter //
create procedure buyproc(in cust_id int,in book_name varchar(30),in buy_num int, out sealprice int)
begin
	declare tempdata int;
    declare tempsealprice int;
    declare tempgrade varchar(10);
    
    declare continue handler for sqlwarning-- sqlwarning, not found
    
    begin
      show warnings;
      select '경고! 문제가 있습니다.' as 메시지;
        rollback;
   end;
	select count(*) into tempdata from book where bookname=book_name;
	if data1 =0 then
		select '없는 서적입니다' as 메세지;
	end if;
    
    set tempgrade = (select grade from customer where custid=cust_id); 
    set tempsealprice = (select price from book where bookname=book_name);
    
    case 
    when tempgrade = 'Vip' then
		set sealprice = tempsealprice * 0.8;
	when tempgrade = 'Gold' then
		set sealprice = tempsealprice * 0.85;
	when tempgrade = 'Silver' then
		set sealprice = tempsealprice * 0.90;
	when tempgrade = 'Bronze' then
		set sealprice = tempsealprice * 0.95;
	else set sealprice = tempsealprice;
	end case;
   
   
   -- select height into data1 from usertbl where userid = user_id;
    select height into tempdata from usertbl where userid = 'khd'; -- khd인 사람의 키를 data1에 삽입
    select tempdata;
end//
delimiter ;
--   - order 테이블에 입력하고 주문이 입력되면 book 테이블에 서적 개수를 감소시키는 트리거를 생성한다.(입력한 서적 개수보다 남은 서적 개수가 적으면 오류처리)
--   - 구매실적이 생길 때마다 해당 고객의 등급을 다시계산하여 업데이트하는 트리거를 생성한다.
drop trigger if exists order_trg;
delimiter //
create trigger order_trg
before insert on orders for each row
begin 
	if (new.num>(select num from book b where b.bookid = new.bookid))then
		select '입력한 서적 개수보다 남은 서적 개수가 적습니다.' as 메세지;
    end if;
    
    update book b set b.num = b.num-new.num;
    call gradproc();
end //
delimiter ;


