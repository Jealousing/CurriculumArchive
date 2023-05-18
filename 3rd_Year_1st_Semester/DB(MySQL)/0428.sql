create database moviedb;
use moviedb;

create table movietbl
(
movie_id int,
movie_title varchar(30),
movie_director varchar(20),
movie_star varchar(20),
movie_script longtext,
movie_film longblob
)
default charset=utf8mb4;

show variables like 'secure_file_priv';

insert into movietbl values(1,'쉰들러리스트','스필버그','리암니슨',
load_file('D:/ClassDatum/3-1/DB/0310/Backup/Movies/Schindler.txt'),
load_file('D:/ClassDatum/3-1/DB/0310/Backup/Movies/Schindler.mp4'));
insert into movietbl values(2,'쇼생크탈출','프랭크 다라본트','팀 로빈스',
load_file('D:/ClassDatum/3-1/DB/0310/Backup/Movies/Shawshank.txt'),
load_file('D:/ClassDatum/3-1/DB/0310/Backup/Movies/Shawshank.mp4'));
insert into movietbl values(3,'라스트 모히칸','마이크 만','다니엘 데이 루이스',
load_file('D:/ClassDatum/3-1/DB/0310/Backup/Movies/Mohican.txt'),
load_file('D:/ClassDatum/3-1/DB/0310/Backup/Movies/Mohican.mp4'));

select movie_film from movietbl where movie_id=1 into dumpfile 'D:/ClassDatum/3-1/DB/0310/Backup/Movies/download/Schindler.mp4';
select movie_film from movietbl where movie_id=1 into dumpfile 'D:/ClassDatum/3-1/DB/0310/Backup/Movies/download/Schindler.txt';

set @aaa =100;

-- ifproc 이 존재하면 삭제
drop procedure if exists ifproc;

delimiter $$ -- 끝문장 // 로 변경
create procedure ifproc()
begin
   declare var1 int;
    set var1 = 100;
    
    if var1 = @aaa+100 then
      select '100 입니다.';
   else 
      select '100이 아닙니다.';
   end if;
end $$
delimiter ; -- 끝문장 ; 로 변경

call ifproc();

-- ifproc 이 존재하면 삭제
drop procedure if exists ifproc2;
delimiter $$ 
create procedure ifproc2()
begin
	declare hiredate date;
    declare currdate date;
    declare days int;
    
    select hire_date into hiredate from employees where emp_no='10001';
    
    set currdate=current_date();
    set days=datediff(currdate,hiredate);
    
    if(days/365)>=5 then
		select concat('입사한지',days,'일이나 지났습니다. 축하합니다')as 메시지;
	else 
		select concat('입사한지',days,'일밖에 안되었네요 열심히 일하세요')as 메시지;
	end if;
end $$
delimiter ; -- 끝문장 ; 로 변경

call ifproc2();

-- ifproc 이 존재하면 삭제
drop procedure if exists ifproc2;
delimiter $$ 
create procedure ifproc2(in point int,out credit char(1))
begin
    -- declare credit char(1);
    case
    when point >=90 then
		set credit ='A';
	when point >=80 then
		set credit = 'B';
	when point >=70 then
		set credit = 'C';
	when point >=60 then
		set credit = 'D';
	else
		set credit='F';
	end case;
    
    -- select concat('취득점수==>', point, '학점==>',credit);
end $$
delimiter ; -- 끝문장 ; 로 변경

set @score =97;
call ifproc2(@score,@credit);
select concat('취득점수==>', @score, '학점==>',@credit);

select u.userid, u.username, sum(price*amount)as 총구매액, 
 case
 when sum(price*amount) >= 1500 then '최우수 고객'
 when sum(price*amount) >= 1000 then '우수 고객'
 when sum(price*amount) >= 1 then '일반 고객'
 else '유령고객'
 end as 고객등급
 from buytbl b
 right outer join usertbl u on u.userid = b.userid
  group by u.userid
   order by 총구매액 desc;
   
drop procedure if exists custproc;

delimiter //
create procedure custproc(in high int, in middle int, in low int)
begin
	

   select u.userid, u.username, sum(price*amount)as 총구매액, 
 case
 when sum(price*amount) >= high then '최우수 고객'
 when sum(price*amount) >= middle then '우수 고객'
 when sum(price*amount) >= low then '일반 고객'
 else '유령고객'
 end as 고객등급
 from buytbl b
 right outer join usertbl u on u.userid = b.userid
  group by u.userid
   order by 총구매액 desc;
   
end//
delimiter ;

call custproc(1500,1000,1);

drop procedure if exists custproc;

delimiter //
create procedure custproc(in high int, in middle int, in low int)
begin
	with total_price(userid,total) as (select userid,sum(price*amount) from buytbl group by userid)
    select u.userid,u.username,tp.total,
		case
        when total >=high then '최우수고객'
        when total >=middle then '우수고객'
        when total >=low then '일반고객'
        else '유령고객'
        end as 고객등급
	from total_price tp
    right outer join usertbl u on u.userid=tp.userid
	order by tp.total desc;
    
end//
delimiter ;

call custproc(1500,1000,1);