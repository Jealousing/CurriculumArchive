-- groupname을 입력하여 그 group에 해당하는 제품의 총 판매량을 구하는 프로시저
-- ( 없는 group 명에 대한 오류처리 포함)
-- call totalpriceproc ( '서적', @total);

drop procedure if exists totalpriceproc;
delimiter //
create procedure totalpriceproc(
	in group_name varchar(10),
    out total_price int
    )
-- totalprice_lable:begin
begin
	declare exit handler for not found
	begin 
		show warnings;
		select '없는 그룹입니다 ' as message;
        rollback;
    end ;
--    select price into total_price from buytbl
--    where groupname=group_name;
-- 	declare tmp int;
--    select count(*) into tmp from buytbl where groupname=group_name;
--    if tmp = 0 then
-- 	select '없는 그룹입니다' as message;
--        leave totalprice_lable;
-- 	end if;
        
	with totalprice(groupname,total) as
    (select groupname, sum(price*amount) from buytbl
    group by groupname)
    
	select total into total_price from totalprice
	where groupname=group_name;
end //
delimiter ;

call totalpriceproc('서적', @total);
select @total;

set global log_bin_trust_function_creators = 1;
drop function if exists getAgefunc;
delimiter //
create function getAgefunc(byear int)
returns int 
begin
   declare age int;
    set age = year(curdate()) - byear;
    return age;
end //
delimiter ;

select getagefunc(1979);

select userid, username, getagefunc(birthyear) as '만나이' from usertbl;

show create function getagefunc;

-- 판매된 도서에 대한 이익을 계산하는 함수 (가격이 30,000원 이상이면 10%, 30,000원 미만이면 5%)
drop function if exists fnc_interest;
delimiter //
create function fnc_interest(price int)
returns decimal(7,2) 
begin
	declare interest_price decimal(7,2);
	if price >= 30000 then set interest_price = price *0.1;
    else 
    set interest_price = price *0.05;
    end if;
    return interest_price;
end //
delimiter ;

drop procedure if exists display_interest;
delimiter //
create procedure display_interest()
begin
	select bookid, bookname, price, fnc_interest(price) as interest from book;
end //
delimiter ;

call display_interest();