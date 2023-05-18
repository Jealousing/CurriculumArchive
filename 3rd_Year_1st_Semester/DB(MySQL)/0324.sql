SELECT * FROM cookdb.usertbl;
select username, addr, max(height) from usertbl; -- 잘못된 방식
select username, addr from usertbl where height=183; -- 알고있으면 가능
select username, addr, height from usertbl where 
height=(select max(height) from usertbl)or height=(select min(height) from usertbl); -- 제일 높은 갓을 모를때 서브쿼리
-- 김용만의 키보다 크거나 같은사람 찾기
select username, height from usertbl where height>=(select height from usertbl where username='김용만');
select height from usertbl where addr='경기';
-- 경기지역 사람들보다 큰사람만 출력 all=다만족 any=하나만 만족하면됨
select username, height from usertbl where height > all(select height from usertbl where addr='경기');
-- 구매액이 500을 넘는 고객의 이름과 주소 조회
select userid from buytbl where priceamount > 500;
select username, addr from usertbl where userid = any(select userid from buytbl where priceamount > 500);
select username, addr from usertbl where userid in(select userid from buytbl where price*amount > 500);
-- 2014.7.7 이후에 판매된 책의 이름과 가격 조회
select bookid from orders where orderdate >= '2014-07-07';
select bookname, price from book where bookid in (select bookid from orders where orderdate >= '2014-07-07');

-- 도서를 구매한 적이 있는 고객의 이름 출력
select distinct custid from orders; -- distinct 중복제거
select name from customer where custid in (select custid from orders);

-- 지역별로 가장 키가 큰 사람의 정보 조회usertbl
select max(height) from usertbl group by addr;
select * from usertbl where (addr,height) in (select addr,max(height) from usertbl group by addr);
-- 테이블 3개: 마당에서 대한미디어에서 출판한 도서를 구매한 고객의 이름을 조회
select bookid from book where publisher='대한미디어';
select custid from orders where bookid in (select bookid from book where publisher='대한미디어');
select name from customer where custid in 
(select custid from orders where bookid in (select bookid from book where publisher='대한미디어'));