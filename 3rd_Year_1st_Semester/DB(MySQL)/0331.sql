-- table 형태 결과
select userid, username, height from usertbl where height>=180;
-- 하나의 필드에 들어가는 여러 데이터를 가지는 결과
select userid from usertbl where height>=180;
-- 단 하나의 데이터만 가져오는 결과
select max(height) from usertbl;
select * from usertbl where height=(select max(height) from usertbl);
select * from usertbl where (addr,userid) in(select addr, userid from usertbl where height>=180);
select * from usertbl where (addr,userid) in(('경북', 'KHD'));

-- 각 지역 별로 평균키보다 큰 사람들의 주소, 이름 , 키를 조회
select u.username, u.addr, u.height, mh.평균키 from usertbl u join (select addr, avg(height) as 평균키  from usertbl group by addr) mh 
on u.addr=mh.addr where mh.평균키<u.height ;

-- 각 고객이 주문한 도서의 이름들을 조회( 고객이름 , 도서이름 )
select c.name,b.bookname from customer c join orders od on c.custid=od.custid join book b on od.bookid=b.bookid;

-- 가격이 20,000원 이상인 도서를 주문한 고객의 이름과 도서의 이름을 조회
select c.name,b.bookname, b.price from customer c join orders od on c.custid=od.custid join book b on od.bookid=b.bookid where b.price>=20000;

-- 날자 별로 가장 돈을 많이 쓴 사람, 구매 가격, 날짜 조회 (고객이름, 구매가격, 구매날짜)
select c.name, od.saleprice, od.orderdate from customer c 
join (select * from orders where (orderdate, saleprice) in (select orderdate, max(saleprice) from orders group by orderdate)) as od on c.custid = od.custid;

select * from orders where (orderdate, saleprice) in (select orderdate, max(saleprice) from orders group by orderdate); -- 독립
select * from orders od1 where saleprice = (select max(saleprice) from orders od2 where od1.orderdate=od2.orderdate); -- 상관

-- 총 구매액이 1000이 넘는 고객들의 정보 조회
select userid, username, addr from usertbl where userid in (select userid  from buytbl group by userid having sum(price*amount)>1000);
select userid, username, addr from usertbl where userid in (select userid  from (select userid , sum(price*amount) as 총구매액 from buytbl group by userid) as tb where 총구매액>1000);
 select u.userid, u.username, u.addr, tb.총구매액 from usertbl u join (select userid, sum(price*amount) 총구매액 from buytbl group by userid)as tb on u.userid = tb.userid where tb.총구매액 > 1000;
 
 -- 제품 분류 별로 가장 구매액이 큰 유저의 이름과, 연락처, 구매액을 조회
 update buytbl set groupname='없음' where groupname is null;
 select u.username, tb.groupname,u.tel, tb.price*tb.amount from usertbl u join (select * from buytbl where (groupname,price*amount) in (select groupname, max(price*amount) from buytbl group by groupname))as tb on u.userid=tb.userid;