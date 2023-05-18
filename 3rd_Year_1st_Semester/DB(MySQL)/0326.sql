-- 운동화를 가장 많이 구매한 고객이름과 주소 조회
select max(priceamount) from buytbl where prodName="운동화";
select userID from buytbl where priceamount in (select max(priceamount) from buytbl where prodName="운동화");
select username, addr from usertbl where userID in (select userID from buytbl where priceamount in (select max(priceamount) from buytbl where prodName="운동화"));

-- 출판사 별로 출판사의 평균 도서 가격보다 비싼 도서를 조회 (이전까진 독립서브쿼리 이용 / 이건 상관서브쿼리 이용)
select avg(price) from book group by publisher;
select b1.bookname from book as b1 where b1.price > (select avg(b2.price) from book as b2 where b1.publisher=b2.publisher );

-- 지역별로 가장 키큰사람 조회
    -- 독립
select max(height) from usertbl group by addr;
select username,addr,height from usertbl where height in (select max(height) from usertbl group by addr);
    -- 상관
select u1.username,u1.addr,u1.height from usertbl as u1 where u1.height = (select max(u2.height) from usertbl as u2 where u1.addr=u2.addr);

-- 총 구매액이 500이 넘는 고객들의 정보 조회
select userid from buytbl where priceamount > 500;
select username, addr from usertbl where userid in(select b1.userid from buytbl as b1 where (select sum(b2.priceamount)from buytbl b2 where b1.userid=b2.userid)>1000);

select u.userid, u.username, u.addr from usertbl as u where (select count() from buytbl as b where u.userid=b.userid)>=1;
select u.userid, u.username, u.addr from usertbl as u where exists (select * from buytbl as b where u.userid=b.userid);
-- 조인 두개의 테이블 조건에 따라 결합
select * from usertbl join buytbl on usertbl.userid=buytbl.userid;
select u.username, u.joindate, b.prodName from usertbl as u join buytbl as b on u.userid=b.userid where u.addr='서울';