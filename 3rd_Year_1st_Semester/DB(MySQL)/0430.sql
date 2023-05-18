create table testtbl2(
   id int auto_increment primary key,
    txt varchar(10)
);

drop procedure if exists testproc;
delimiter //
create procedure testproc(
   in txtvalue varchar(20),
    out outvalue int
)
begin
	insert into testtbl2 value(null,txtvalue);
    select max(id) into outvalue from testtbl2;
    
end//
delimiter ;

call testproc('bbbbb',@id);
select @id;

drop procedure if exists whileproc;
delimiter //
create procedure whileproc()
begin
	declare i int default 1;
    declare hap int default 0;
    
    mywhile:while(i<=100) do
		if(i%7=0) then
			set i =i+1;
            iterate mywhile;
		end if;
        set hap= hqp+i;
        if(hap>1000)then
			leave mywhile;
		end if;
            set i=i+1;
	end while;
end//
delimiter ;
call whileproc();

-- 처음으로 1000을 넘길경우
drop procedure if exists whileproc;
delimiter //
create procedure whileproc()
begin
   declare i int default 1;
    declare hap int default 0;
    -- set i = 1; default 안쓸 시
    -- set hap = 0;
    mywhile : while(i <= 100) do
      if(i%7 = 0) then
         set i = i+1;
            iterate mywhile;
      end if;
      set hap = hap+i;
        if(hap > 1000) then
         leave mywhile;
      end if;
        set i = i+1;
    end while;
    select hap;
end//
delimiter ;

call whileproc();


-- 예외처리
drop procedure if exists errorproc;
delimiter //
create procedure errorproc()
begin
	declare data1 int;
    declare continue handler for sqlwarning
	-- declare continue handler for sqlexception 
    -- declare exit handler for sqlexception
    begin
		-- show errors;
		-- select '오류가 났습니다' as 메세지;
        show warnings;
        select '경고! 문제가 있습니다.' as 메세지;
        rollback;
	end ;
    -- insert into usertbl values('yjs','윤정수',1998,'서울',null,170,curdate());
    -- insert into usertbl values('mms','윤정수',1998,'서울',null,170,curdate());
	-- select * from notbl;
    select height into data1 from usertbl where userid='kja';
    select data1;
    select height into data1 from usertbl where userid='khd';
    select data1;
    set data1=(select height from usertbl where userid='kja'); -- 경고가안뜸
end//
delimiter ;
call errorproc();
select height into @data1 from usertbl where userid='kja';
select @data1;

-- 예외처리(경고)
drop procedure if exists errorproc;
delimiter //
create procedure errorproc(in user_id varchar(10))
begin
   declare data1 int;
    declare continue handler for sqlwarning-- sqlwarning, not found
    begin
      show warnings;
      select '경고! 문제가 있습니다.' as 메시지;
        rollback;
   end;
   select count(*) into data1 from usertbl where userid=user_id;
   if data1 =0 then
	select '없는 아이디입니다' as 메세지;
   end if;
   
   -- select height into data1 from usertbl where userid = user_id;
    select height into data1 from usertbl where userid = 'khd'; -- khd인 사람의 키를 data1에 삽입
    select data1;
end//
delimiter ;

call errorproc('kja');

-- book table에 저장된 도서들의 평균 값을 반환하는 프로시저
drop procedure if exists averageProc;
delimiter // 
create procedure averageproc( out avgprice int)
begin
	select cast(avg(price) as signed int) into avgprice from book;
end //
delimiter ;

call averageproc(@avgprice);
select @avgprice;

-- 각 출판사와 최고가(max) , 최저가 (min) 중 하나를 입력하면 그에 해당하는
-- 책의 이름과 가격을 리턴하는 프로시저 구현
-- call getbookprice ('나무수','MAX',@bkname,@bkprice);
drop procedure if exists getbookprice;
delimiter //
create procedure getbookprice(
	in publ varchar(20),
    in sel varchar(3),
    out bk_name varchar(30),
    out bk_price int)
begin 
	declare tmpprice int;
    
    case
    when sel = 'MAX' then 
     set tmpprice = (select max(price) from book_table where publisher = publ);
	when sel = 'MIN' then
     set tmpprice = (select min(price) from book_table where publisher = publ);
	else set tmpprice = -1;
    end case;

	if(tmpprice != -1) then
		select bookname into bk_name from book_table 
         where publisher = publ and price = tmpprice
          order by bookname limit 1;
		select price into bk_price from book_table 
         where publisher = publ and price = tmpprice
          order by bookname limit 1;
	else
		set bk_name = 'input error';
        set bk_price = -1;
    end if;
end//
delimiter ;

call getbookprice('나무수', 'MAX', @bkname, @bkprice);

select @bkname, @bkprice;